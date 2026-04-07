#include <WiFi.h>
#include <WebServer.h>


const char* ssid     = "xswl";
const char* password = "hxy060617";

const int TOUCH_PIN = 4;
const int LED1 = 2;
const int LED2 = 15;
const int THRESHOLD = 400;  // 触摸阈值

WebServer server(80);

// 网页仪表盘
void handleRoot() {
  String html = R"HTML(
<!DOCTYPE html>
<html>
<head>
  <meta charset="UTF-8">
  <meta name="viewport" content="width=device-width, initial-scale=1.0">
  <title>实时触摸传感器</title>
  <style>
    body{text-align:center; font-family:Arial; margin-top:60px; background:#111; color:white;}
    .val{font-size:90px; color:#00ff99; font-weight:bold;}
  </style>
</head>
<body>
  <h2>实时触摸数值</h2>
  <div class="val" id="val">0</div>
  <script>
    setInterval(()=>{fetch("/data").then(r=>r.text()).then(d=>document.getElementById("val").innerText=d)},50);
  </script>
</body>
</html>
)HTML";
  server.send(200, "text/html; charset=utf-8", html);
}

// 数据接口
void handleData() {
  int val = touchRead(TOUCH_PIN);
  server.send(200, "text/plain", String(val));
}

void setup() {
  Serial.begin(115200);
  pinMode(LED1, OUTPUT);
  pinMode(LED2, OUTPUT);

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) delay(500);
  
  Serial.println("WiFi 已连接");
  Serial.print("IP地址: ");
  Serial.println(WiFi.localIP());

  server.on("/", handleRoot);
  server.on("/data", handleData);
  server.begin();
}

void loop() {
  server.handleClient();
  int tv = touchRead(TOUCH_PIN);

  // 阈值控制双灯
  if (tv < THRESHOLD) {
    digitalWrite(LED1, HIGH);
    digitalWrite(LED2, LOW);
  } else {
    digitalWrite(LED1, LOW);
    digitalWrite(LED2, HIGH);
  }
}