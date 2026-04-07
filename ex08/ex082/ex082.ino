#include <WiFi.h>
#include <WebServer.h>

const char* ssid = "xswl";
const char* password = "hxy060617";

const int TOUCH_PIN = 4;
const int LED1 = 2;
const int LED2 = 15;
int threshold = 400;

bool isArmed = false;
bool isAlarming = false;

unsigned long flashMillis = 0;
bool state = false;

WebServer server(80);

String makePage() {
  String s;
  if (isAlarming) s = "<span style='color:red'>🚨 报警中！</span>";
  else if (isArmed) s = "<span style='color:blue'>🛡️ 已布防</span>";
  else s = "<span style='color:gray'>⭕ 已撤防</span>";

  String html = "<!DOCTYPE html><html><head><meta charset='UTF-8'><meta http-equiv='refresh' content='2'></head><body style='text-align:center;margin-top:50px'><h1>安防报警</h1><h2>" + s + "</h2><a href='/arm'><button>布防</button></a> <a href='/disarm'><button>撤防</button></a></body></html>";
  return html;
}

void handleRoot() { server.send(200, "text/html; charset=utf-8", makePage()); }
void handleArm()  { isArmed = true;  isAlarming = false; server.sendHeader("Location", "/"); server.send(303); }
void handleDisarm(){isArmed = false; isAlarming = false; digitalWrite(LED1, LOW); digitalWrite(LED2, LOW); server.sendHeader("Location", "/"); server.send(303); }

void setup() {
  Serial.begin(115200);
  pinMode(LED1, OUTPUT);
  pinMode(LED2, OUTPUT);
  digitalWrite(LED1, LOW);
  digitalWrite(LED2, LOW);

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) { delay(500); Serial.print("."); }
  Serial.println("\nIP: " + WiFi.localIP().toString());

  server.on("/", handleRoot);
  server.on("/arm", handleArm);
  server.on("/disarm", handleDisarm);
  server.begin();
}

void loop() {
  server.handleClient();

  if (!isArmed) {
    digitalWrite(LED1, LOW);
    digitalWrite(LED2, LOW);
    return;
  }

  // 布防时：两个灯都不亮（待命）
  if (isArmed && !isAlarming) {
    digitalWrite(LED1, LOW);
    digitalWrite(LED2, LOW);
    
    if (touchRead(TOUCH_PIN) < threshold) {
      isAlarming = true;
    }
    return;
  }


  // 报警：双灯 真 · 交替闪烁

  if (isAlarming) {
    if (millis() - flashMillis > 120) {
      flashMillis = millis();
      state = !state;
      
      digitalWrite(LED1,  state);
      digitalWrite(LED2, !state);  // 反向！一定交替！
    }
  }
}