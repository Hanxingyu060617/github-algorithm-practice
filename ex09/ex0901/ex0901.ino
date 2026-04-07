#include <WiFi.h>
#include <WebServer.h>


const char* ssid = "xswl";
const char* password = "hxy060617";

// 硬件
const int TOUCH_PIN = 4;  // 触摸引脚
const int LED1 = 2;
const int LED2 = 15;

WebServer server(80);

// 主页 
void handleRoot() {
  String html = R"HTML(
<!DOCTYPE html>
<html>
<head>
  <meta charset="UTF-8">
  <meta name="viewport" content="width=device-width, initial-scale=1.0"/>
  <title>实时触摸传感器仪表盘</title>
  <style>
    body{text-align:center; font-family:Arial; margin-top:60px; background:#111; color:white;}
    .value{font-size:80px; color:#00ff99; margin:40px 0;}
    .title{font-size:24px; color:#ccc;}
  </style>
</head>
<body>
  <div class="title">📡 ESP32 实时触摸传感器数值</div>
  <div class="value" id="val">0</div>

  <script>
    // AJAX 实时获取数值
    function update() {
      fetch("/data").then(res=>res.text()).then(num=>{
        document.getElementById("val").innerText = num;
      });
    }
    setInterval(update, 50); // 每50ms刷新一次
  </script>
</body>
</html>
  )HTML";
  server.send(200, "text/html; charset=utf-8", html);
}

// 返回传感器数据 
void handleData() {
  int val = touchRead(TOUCH_PIN);
  server.send(200, "text/plain", String(val));
}

//  初始化 
void setup() {
  Serial.begin(115200);
  pinMode(LED1, OUTPUT);
  pinMode(LED2, OUTPUT);
  digitalWrite(LED1, LOW);
  digitalWrite(LED2, LOW);

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) { delay(500); Serial.print("."); }
  Serial.println("\nWiFi OK! IP: " + WiFi.localIP().toString());

  server.on("/", handleRoot);
  server.on("/data", handleData);
  server.begin();
}

//  主循环 
void loop() {
  server.handleClient();
}