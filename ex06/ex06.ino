const int ledPin1 = 2;
const int ledPin2 = 4;

const int freq = 5000;
const int resolution = 8;  

void setup() {
  Serial.begin(115200);

  // 初始化两个独立PWM通道
  ledcAttach(ledPin1, freq, resolution);
  ledcAttach(ledPin2, freq, resolution);
}

void loop() {
  // 反相渐变：LED1变亮，LED2变暗
  for (int duty = 0; duty <= 255; duty++) {
    ledcWrite(ledPin1, duty);
    ledcWrite(ledPin2, 255 - duty);
    delay(8);
  }

  // 反向渐变：LED1变暗，LED2变亮
  for (int duty = 255; duty >= 0; duty--) {
    ledcWrite(ledPin1, duty);
    ledcWrite(ledPin2, 255 - duty);
    delay(8);
  }
}
