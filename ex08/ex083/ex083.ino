#include <WiFi.h>
#include <WebServer.h>

const char* ssid = "xswl";
const char* password = "hxy060617";


const int TOUCH_PIN = 4;
const int LED1 = 2;
const int LED2 = 15;
int threshold = 400;

//  系统状态 
bool isArmed = false;
bool isAlarming = false;

unsigned long previousMillis = 0;
const long ALARM_BLINK = 100;
const long STANDBY_BLINK = 800;

WebServer server(80);

//  网页界面
String makePage() {
  String sysState;
  
  if (isAlarming) {
    sysState = "<span style='color:red; font-size:24px'>🚨 警报触发！双灯报警中</span>";
  } else if (isArmed) {
    sysState = "<span style='color:blue; font-size:24px'>🛡️ 已布防 · 待命状态</span>";
  } else {
    sysState = "<span style='color:gray; font-size:24px'>⭕ 已撤防 · 系统关闭</span>";
  }

  String html = "<!DOCTYPE html><html lang='zh-CN'><head>";
  html += "<meta charset='UTF-8'>";
  html += "<meta name='viewport' content='width=device-width, initial-scale=1.0'>";
  html += "<meta http-equiv='refresh' content='1'>";
  html += "<title>安防报警 V3.0 最终版</title>";
  html += "<style>body{text-align:center; font-family:Arial; margin-top:60px;}";
  html += "button{padding:20px 40px; font-size:18px; margin:10px; border:none; border-radius:8px; color:white;}";
  html += ".arm{background:#007bff;} .disarm{background:#dc3545;}</style>";
  html += "</head><body>";
  html += "<h1>ESP32 物联网安防报警器 V3.0</h1>";
  html += "<h3>" + sysState + "</h3>";
  html += "<br><a href='/arm'><button class='arm'>布防 (Arm)</button></a>";
  html += "<a href='/disarm'><button class='disarm'>撤防 (Disarm)</button></a>";
  html += "</body></html>";
  
  return html;
}

// 网页路由 
void handleRoot() {
  server.send(200, "text/html; charset=utf-8", makePage());
}

void handleArm() {
  isArmed = true;
  isAlarming = false;
  server.sendHeader("Location", "/");
  server.send(303);
}

void handleDisarm() {
  isArmed = false;
  isAlarming = false;
  digitalWrite(LED1, LOW);
  digitalWrite(LED2, LOW);
  server.sendHeader("Location", "/");
  server.send(303);
}

//  初始化
void setup() {
  Serial.begin(115200);
  pinMode(LED1, OUTPUT);
  pinMode(LED2, OUTPUT);
  digitalWrite(LED1, LOW);
  digitalWrite(LED2, LOW);

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi 连接成功");
  Serial.print("控制面板地址：http://");
  Serial.println(WiFi.localIP());

  server.on("/", handleRoot);
  server.on("/arm", handleArm);
  server.on("/disarm", handleDisarm);
  server.begin();
}

//  主程序
void loop() {
  server.handleClient();
  unsigned long currentMillis = millis();

  if (!isArmed) {
    digitalWrite(LED1, LOW);
    digitalWrite(LED2, LOW);
    return;
  }

  if (isArmed && !isAlarming) {
    if (currentMillis - previousMillis >= STANDBY_BLINK) {
      previousMillis = currentMillis;
      bool s = !digitalRead(LED1);
      digitalWrite(LED1, s);
      digitalWrite(LED2, s);
    }

    if (touchRead(TOUCH_PIN) < threshold) {
      isAlarming = true;
      Serial.println("=== 报警触发 ===");
    }
  }

  if (isAlarming) {
    if (currentMillis - previousMillis >= ALARM_BLINK) {
      previousMillis = currentMillis;
      bool s = !digitalRead(LED1);
      digitalWrite(LED1, s);
      digitalWrite(LED2, !s);
    }
  }
}