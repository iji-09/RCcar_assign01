#include <Arduino.h>

// 함수 프로토타입 선언
void handleCH3();                // CH3 외부 인터럽트 핸들러: 밝기 조절용
void handleCH9();                // CH9 외부 인터럽트 핸들러: ON/OFF 제어용
void updateSpectrumColor();      // HSV → RGB 변환 후 RGB 출력 적용 함수

// ===================== 핀 설정 =====================
const int ch3Pin = 2;    // CH3: LED 밝기 제어용 PWM 입력 (외부 인터럽트 지원 핀)
const int ch9Pin = 3;    // CH9: LED ON/OFF 제어용 PWM 입력 (외부 인터럽트 지원 핀)
const int ch1Pin = 7;    // CH1: RGB 색상 제어용 PWM 입력 (핀체인지 인터럽트 사용 핀)

const int ledPin = 6;    // LED 밝기 조절용 출력 핀 (PWM 출력 가능)
const int ledPin2 = 9;   // ON/OFF 제어 LED 출력 핀
const int redPin = 5;    // RGB LED의 빨간색 채널 핀 (PWM 출력 가능)
const int greenPin = 10; // RGB LED의 초록색 채널 핀 (PWM 출력 가능)
const int bluePin = 11;  // RGB LED의 파란색 채널 핀 (PWM 출력 가능)

// ===================== PWM 측정용 변수 =====================
// 각 채널의 PWM 신호 폭(pulse width)을 측정하기 위한 변수들

// CH3: 밝기 제어용
volatile unsigned long ch3Start = 0, ch3Width = 0;
volatile bool ch3Updated = false;

// CH9: ON/OFF 제어용
volatile unsigned long ch9Start = 0, ch9Width = 0;
volatile bool ch9Updated = false;

// CH1: RGB 색상 제어용
volatile unsigned long ch1Start = 0, ch1Width = 0;
volatile bool ch1Updated = false;
volatile bool ch1LastState = LOW;  // CH1 핀의 직전 상태 기록

// ===================== 동작 변수 =====================
int brightness = 0;   // LED 밝기 (0~255)
float hue = 0;        // 현재 RGB 색상을 나타내는 hue 값 (0~360도)

// ===================== 초기 설정 =====================
void setup() {
  Serial.begin(9600);  // 시리얼 통신 시작 (디버깅 출력용)

  // PWM 입력 핀들을 입력으로 설정
  pinMode(ch3Pin, INPUT);
  pinMode(ch9Pin, INPUT);
  pinMode(ch1Pin, INPUT);

  // 출력 핀들 설정
  pinMode(ledPin, OUTPUT);     // 밝기 제어 LED
  pinMode(ledPin2, OUTPUT);    // ON/OFF 제어 LED
  pinMode(redPin, OUTPUT);     // RGB LED - RED
  pinMode(greenPin, OUTPUT);   // RGB LED - GREEN
  pinMode(bluePin, OUTPUT);    // RGB LED - BLUE

  // CH3, CH9는 외부 인터럽트를 사용
  attachInterrupt(digitalPinToInterrupt(ch3Pin), handleCH3, CHANGE); // 밝기용
  attachInterrupt(digitalPinToInterrupt(ch9Pin), handleCH9, CHANGE); // ON/OFF용

  // CH1 (D7, PCINT23)은 핀체인지 인터럽트 사용
  PCICR |= (1 << PCIE2);        // PORTD 전체에 핀체인지 인터럽트 허용
  PCMSK2 |= (1 << PCINT23);     // 그 중 D7(=PCINT23) 핀만 마스크 설정
}

// ===================== 메인 루프 =====================
void loop() {
  // ========== [김예진 담당] LED 밝기 제어 ==========
  if (ch3Updated) {
    ch3Updated = false;

    // PWM 신호 폭이 정상 범위일 때 밝기 계산 및 적용
    if (ch3Width >= 1000 && ch3Width <= 2000) {
      brightness = map(ch3Width, 1000, 2000, 0, 255);  // 1000~2000us → 0~255
      analogWrite(ledPin, brightness);  // 밝기 PWM 출력
      Serial.print("CH3 PWM: ");
      Serial.print(ch3Width);
      Serial.print(" us → Brightness: ");
      Serial.println(brightness);
    } else {
      // 범위 벗어나면 LED OFF
      analogWrite(ledPin, 0);
    }
  }

  // ========== [김예진 담당] LED ON/OFF 제어 ==========
  if (ch9Updated) {
    ch9Updated = false;

    Serial.print("CH9 PWM: ");
    Serial.print(ch9Width);
    Serial.print(" us → ");

    if (ch9Width >= 1500) {
      digitalWrite(ledPin2, HIGH);   // 스위치가 ON 상태
      Serial.println("LED ON");
    } else if (ch9Width > 500) {
      digitalWrite(ledPin2, LOW);    // 스위치가 OFF 상태
      Serial.println("LED OFF");
    } else {
      // 매우 짧은 신호는 무효로 간주
      Serial.println("NO SIGNAL");
    }
  }

  // ========== [정이지 담당] RGB 색상 전환 ==========
  if (ch1Updated) {
    ch1Updated = false;

    if (ch1Width >= 1000 && ch1Width <= 2000) {
      hue = map(ch1Width, 1000, 2000, 0, 360);  // PWM 신호 폭을 0~360도 색상으로 변환
      Serial.print("CH1 PWM: ");
      Serial.print(ch1Width);
      Serial.print(" us → Hue: ");
      Serial.println(hue);
    }
  }

  // 현재 hue 값에 따라 RGB LED 색상 적용
  updateSpectrumColor();
}

// ===================== 외부 인터럽트 핸들러 =====================
// CH3 핀의 상승/하강 에지를 감지하여 PWM 폭 계산
void handleCH3() {
  if (digitalRead(ch3Pin) == HIGH) {
    ch3Start = micros();  // 상승 에지: 시작 시간 기록
  } else {
    ch3Width = micros() - ch3Start;  // 하강 에지: 폭 계산
    ch3Updated = true;
  }
}

// CH9 핀의 상승/하강 에지를 감지하여 PWM 폭 계산
void handleCH9() {
  if (digitalRead(ch9Pin) == HIGH) {
    ch9Start = micros();  // 상승 에지: 시작 시간 기록
  } else {
    ch9Width = micros() - ch9Start;  // 하강 에지: 폭 계산
    ch9Updated = true;
  }
}

// ===================== 핀체인지 인터럽트 핸들러 (CH1 색상) =====================
// PORTD 전체에 대해 인터럽트가 발생하면 실행됨
ISR(PCINT2_vect) {
  unsigned long now = micros();                        // 현재 시간
  bool ch1State = PIND & (1 << PIND7);                 // D7 핀의 상태 읽기

  if (ch1State != ch1LastState) {                      // 상태 변화가 있을 경우에만 처리
    ch1LastState = ch1State;

    if (ch1State == HIGH) {
      ch1Start = now;                                  // 상승 에지: 시작 시간 기록
    } else {
      ch1Width = now - ch1Start;                       // 하강 에지: 폭 계산
      ch1Updated = true;                               // main loop에서 처리하도록 플래그 설정
    }
  }
}

// ===================== HSV → RGB 변환 및 출력 =====================
// 현재 hue(색상 각도)를 기반으로 RGB 색상을 계산하고 출력
void updateSpectrumColor() {
  float r, g, b;                        // RGB 비율
  int i = int(hue / 60.0) % 6;         // 색상 범위 구간 (60도 단위)
  float f = (hue / 60.0) - i;          // 보간용 소수 부분
  float q = 1 - f;

  // HSV → RGB 변환 공식 (V=1, S=1 가정)
  switch (i) {
    case 0: r = 1; g = f; b = 0; break;
    case 1: r = q; g = 1; b = 0; break;
    case 2: r = 0; g = 1; b = f; break;
    case 3: r = 0; g = q; b = 1; break;
    case 4: r = f; g = 0; b = 1; break;
    case 5: r = 1; g = 0; b = q; break;
  }

  // RGB 비율을 0~255 범위로 변환하여 PWM 출력
  analogWrite(redPin,   int(r * 255));
  analogWrite(greenPin, int(g * 255));
  analogWrite(bluePin,  int(b * 255));
}
