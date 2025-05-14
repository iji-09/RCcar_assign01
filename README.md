# Arduino RC PWM LED Controller

이 프로젝트는 RC 조종기에서 발생하는 PWM 신호(CH1, CH3, CH9)를 Arduino로 읽어 다양한 방식으로 LED를 제어하는 시스템입니다.  
- CH3: 밝기 조절  
- CH9: ON/OFF 제어  
- CH1: RGB 색상 전환 (조이스틱 방향에 따라 스펙트럼 색상 변화)

---

## 기능 설명

| 채널 | 핀 번호 | 기능 | 설명 |
|------|---------|------|------|
| CH3  | D2      | LED 밝기 제어 | PWM 신호 폭(1000~2000μs)을 `analogWrite()` 값(0~255)으로 매핑하여 `D6` 핀에 연결된 LED 밝기 조절 |
| CH9  | D3      | LED ON/OFF 제어 | PWM 신호가 1500μs 이상이면 LED ON, 그 이하이면 OFF (`D9` 핀 제어) |
| CH1  | D7      | RGB 색상 제어 | PWM 신호 폭에 따라 hue(색상 각도) 설정, `HSV → RGB`로 변환하여 RGB LED 색상 변화 |

---

## 회로 구성도

아래는 전체 회로 연결 구성입니다. (핀 번호, LED, RGB, 수신기 연결 참고)
![0514과제사진](https://github.com/user-attachments/assets/e67c86f4-b634-4f79-89a0-b0dace2238a4)


---

## 핀 연결

| 기능 | 아두이노 핀 | 설명 |
|------|-------------|------|
| CH3 입력 | D2 | 외부 인터럽트 사용 |
| CH9 입력 | D3 | 외부 인터럽트 사용 |
| CH1 입력 | D7 | 핀체인지 인터럽트 사용 |
| 밝기 LED 출력 | D6 | PWM 출력 가능 |
| ON/OFF LED 출력 | D9 | 디지털 출력 |
| RGB 출력 - RED | D5 | PWM |
| RGB 출력 - GREEN | D10 | PWM |
| RGB 출력 - BLUE | D11 | PWM |

---

## 작동 조건

- PWM 입력 신호는 RC 수신기 또는 시뮬레이터에서 공급
- PWM 폭은 일반적으로 1000μs ~ 2000μs 사이여야 함
- 아두이노 우노 기준으로 작성되었으며, 인터럽트 핀 번호는 보드에 따라 다를 수 있음

---

## 색상 전환 방식 (CH1)

- 조이스틱의 위치에 따라 0~360도 사이의 hue 값으로 매핑
- hue 값을 기반으로 HSV → RGB 변환을 수행
- 각 색상은 자연스럽게 전환됨 (예: 빨강 → 주황 → 노랑 → 초록 → 파랑 → 보라)

---

## 코드 구성

- `setup()`  
  - 각 핀 모드 설정 및 인터럽트 설정 수행
- `loop()`  
  - 세 채널의 PWM 신호 변화에 따라 LED 상태를 실시간으로 제어
- `handleCH3()`, `handleCH9()`  
  - 상승/하강 에지를 감지하여 PWM 폭 측정
- `PCINT2_vect`  
  - D7 핀(PIND7)의 핀체인지 인터럽트 처리
- `updateSpectrumColor()`  
  - hue 값을 기반으로 RGB LED 출력 설정

---

## 기여자 역할 분담

- **김예진**: CH3 (밝기), CH9 (ON/OFF) 제어 코드 구현
- **정이지**: CH1 (RGB 색상 전환) 및 HSV 색상 전환 로직 구현

---

## 시연 영상

>  [YouTube 링크](https://youtu.be/QlPy-ukTKvs) ← 실제 시연 영상 URL 삽입

---
