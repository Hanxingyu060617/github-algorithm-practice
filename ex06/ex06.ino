// 2=黄灯，4=红灯
const int ledYellow = 2;
const int ledRed    = 4;

const int freq = 5000;
const int resolution = 8;
const int interval = 8; // 渐变间隔

int duty = 0;
int fadeStep = 1;
unsigned long previousMillis = 0;

void setup() {
  Serial.begin(115200);
  ledcAttach(ledYellow, freq, resolution);
  ledcAttach(ledRed, freq, resolution);
  Serial.println("警车双闪已启动");
}

void loop() {
  unsigned long currentMillis = millis();

  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;

    duty += fadeStep;

    // 到达最亮 → 反转方向
    if (duty >= 255) {
      duty = 255;
      fadeStep = -1;
      Serial.println("黄灯亮，红灯暗");
    }

    // 到达最暗 → 反转方向
    if (duty <= 0) {
      duty = 0;
      fadeStep = 1;
      Serial.println("黄灯暗，红灯亮");
    }

    // 反相输出
    ledcWrite(ledYellow, duty);
    ledcWrite(ledRed, 255 - duty);
  }
}