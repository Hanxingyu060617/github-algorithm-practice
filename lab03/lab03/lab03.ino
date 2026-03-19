// 板载LED引脚（ESP32自带灯，GPIO2）
const int ledPin = 2;
// 外接红灯引脚（改成你实际接线的引脚号）
const int extLedPin = 4;  

// PWM属性设置
const int freq = 5000;          // 频率 5000Hz
const int resolution = 8;       // 分辨率 8位 (0-255)

void setup() {
  Serial.begin(115200);

  // 同时绑定两个引脚到PWM
  ledcAttach(ledPin, freq, resolution);
  ledcAttach(extLedPin, freq, resolution);
}

void loop() {
  // 第一阶段：板载灯变强 → 外接红灯变暗
  for(int dutyCycle = 0; dutyCycle <= 255; dutyCycle++){   
    ledcWrite(ledPin, dutyCycle);               // 板载灯：0→255（越来越亮）
    ledcWrite(extLedPin, 255 - dutyCycle);     // 外接红灯：255→0（越来越暗）
    delay(10);
  }

  // 第二阶段：板载灯变弱 → 外接红灯变亮
  for(int dutyCycle = 255; dutyCycle >= 0; dutyCycle--){
    ledcWrite(ledPin, dutyCycle);               // 板载灯：255→0（越来越暗）
    ledcWrite(extLedPin, 255 - dutyCycle);     // 外接红灯：0→255（越来越亮）
    delay(10);
  }
  
  Serial.println("Breathing cycle completed");
}