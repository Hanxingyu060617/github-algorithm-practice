#include <WiFi.h>
#include <WebServer.h>

const char* ssid = "xswl";      
const char* password = "hxy060617";  

const int TOUCH_PIN = 4;        // 触摸引脚 
const int STATUS_LED_PIN = 2;   // 状态指示灯 (布防时常亮)
const int ALARM_LED_PIN = 15;   // 警报指示灯 (报警时狂闪)
int threshold = 400;             // 触摸阈值 

// 系统状态
bool isArmed = false;     
bool isAlarming = false;  

// 非阻塞闪烁变量
unsigned long previousMillis = 0;
const long blinkInterval = 100; // 狂闪频率

WebServer server(80);

String makePage() {
  String sysState = isArmed ? (isAlarming ? "<span style='color:red;'>🚨 警报触发中！</span>" : "<span style='color:green;'>🛡️ 已布防，安全</span>") : "<span style='color:gray;'>⭕ 已撤防</span>";
  
  String html = R"rawliteral(
<!DOCTYPE html>
<html lang="zh-CN">
<head>
  <meta charset="UTF-8">
  <meta name="viewport" content="width=device-width, initial-scale=1.0">
  <meta http-equiv="refresh" content="3">
  <title>双灯版安防主机</title>
</head>
<body style="font-family:Arial; text-align:center; margin-top:50px;">
  <h1>物联网安防报警模拟 (双灯增强版)</h1>
  <h2>系统状态: )rawliteral" + sysState + R"rawliteral(</h2>
  <br><br>
  <a href="/arm"><button style="padding:15px 30px; font-size:16px; background-color:#4CAF50; color:white; border:none; border-radius:5px;">开始布防 (Arm)</button></a>
  &nbsp;&nbsp;&nbsp;
  <a href="/disarm"><button style="padding:15px 30px; font-size:16px; background-color:#f44336; color:white; border:none; border-radius:5px;">解除布防 (Disarm)</button></a>
</body>
</html>
)rawliteral";
  return html;
}

void handleRoot() {
  server.send(200, "text/html; charset=UTF-8", makePage());
}

void handleArm() {
  isArmed = true;
  digitalWrite(STATUS_LED_PIN, HIGH); // 布防时点亮状态灯
  server.sendHeader("Location", "/");
  server.send(303);
}

void handleDisarm() {
  isArmed = false;
  isAlarming = false; 
  digitalWrite(STATUS_LED_PIN, LOW); // 撤防时熄灭状态灯
  digitalWrite(ALARM_LED_PIN, LOW);  // 确保警报灯熄灭
  server.sendHeader("Location", "/");
  server.send(303);
}

void setup() {
  Serial.begin(115200);
  
  pinMode(STATUS_LED_PIN, OUTPUT);
  pinMode(ALARM_LED_PIN, OUTPUT);
  digitalWrite(STATUS_LED_PIN, LOW);
  digitalWrite(ALARM_LED_PIN, LOW);
  
  WiFi.begin(ssid, password);
  Serial.print("连接WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\n连接成功");
  Serial.print("访问控制面板: http://");
  Serial.println(WiFi.localIP());

  server.on("/", handleRoot);
  server.on("/arm", handleArm);
  server.on("/disarm", handleDisarm);
  server.begin();
}

void loop() {
  server.handleClient();

  if (isArmed) {
    if (!isAlarming) {
      // 正在布防且未报警时，检测导线是否被触摸
      int touchValue = touchRead(TOUCH_PIN);
      if (touchValue < threshold && touchValue > 0) { 
        Serial.println("入侵检测！触发报警！");
        isAlarming = true; // 锁定报警状态
      }
    } else {
      // 报警状态：警报灯狂闪，状态灯依然保持常亮
      unsigned long currentMillis = millis();
      if (currentMillis - previousMillis >= blinkInterval) {
        previousMillis = currentMillis;
        int ledState = digitalRead(ALARM_LED_PIN);
        digitalWrite(ALARM_LED_PIN, !ledState); 
      }
    }
  }
}