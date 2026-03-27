// 作业6：警车双闪灯效
// 2=黄灯，4=红灯
const int ledYellow = 2;
const int ledRed    = 4;

const int freq = 5000;
const int resolution = 8;

void setup() {
  Serial.begin(115200);
  ledcAttach(ledYellow, freq, resolution);
  ledcAttach(ledRed, freq, resolution);
  Serial.println("警车双闪已启动");
}

void loop() {
  // 黄灯渐亮，红灯渐暗
  for (int duty = 0; duty <= 255; duty++) {
    ledcWrite(ledYellow, duty);
    ledcWrite(ledRed, 255 - duty);
    delay(8);
  }
  Serial.println("黄灯亮，红灯暗");

  // 黄灯渐暗，红灯渐亮
  for (int duty = 255; duty >= 0; duty--) {
    ledcWrite(ledYellow, duty);
    ledcWrite(ledRed, 255 - duty);
    delay(8);
  }
  Serial.println("黄灯暗，红灯亮");
}
