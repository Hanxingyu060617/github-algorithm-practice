#include <WiFi.h>
#include <WebServer.h>


const char* ssid     = "xswl";
const char* password = "hxy060617";

const int TOUCH_PIN = 4;
const int LED1 = 2;
const int LED2 = 15;
const int THRESHOLD = 400;

WebServer server(80);

//进阶网页仪表盘 
void handleRoot() {
  String html = R"HTML(
<!DOCTYPE html>
<html lang="zh-CN">
<head>
  <meta charset="UTF-8">
  <meta name="viewport" content="width=device-width, initial-scale=1.0">
  <title>ESP32 实时触摸监测仪</title>
  <style>
    body{text-align:center; font-family:Arial; background:#121212; color:white; padding-top:40px;}
    .panel{max-width:400px; margin:0 auto; padding:20px; background:#1e1e1e; border-radius:16px;}
    .title{font-size:22px; margin-bottom:20px; color:#ccc;}
    .value{font-size:100px; font-weight:bold; margin:40px 0; transition:0.2s;}
    .normal{color:#00ff99;}
    .alert{color:#ff4444; animation: flash 0.6s infinite;}
    @keyframes flash{50%{opacity:0.5;}}
    .tip{color:#888; font-size:16px;}
  </style>
</head>
<body>
  <div class="panel">
    <div class="title">📡 ESP32 实时触摸传感器仪表盘</div>
    <div class="value normal" id="val">0</div>
    <div class="tip">触摸 → 数值变红闪烁</div>
  </div>

  <script>
    function update() {
      fetch("/data").then(r=>r.text()).then(v=>{
        let num = parseInt(v);
        let ele = document.getElementById("val");
        ele.innerText = v;
        
        // 数值 < 400 → 报警红色
        if(num < 400){
          ele.className = "value alert";
        }else{
          ele.className = "value normal";
        }
      });
    }
    setInterval(update, 50);
  </script>
</body>
</html>
)HTML";
  server.send(200, "text/html; charset=utf-8", html);
}

// 返回数据
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

  Serial.println("WiFi OK");
  Serial.print("IP: ");
  Serial.println(WiFi.localIP());

  server.on("/", handleRoot);
  server.on("/data", handleData);
  server.begin();
}

void loop() {
  server.handleClient();

  int tv = touchRead(TOUCH_PIN);
  static unsigned long t = 0;
  static bool ledState = false;

  if (tv < THRESHOLD) {
    // 触摸 → LED1 快闪报警
    if (millis() - t > 100) {
      t = millis();
      ledState = !ledState;
      digitalWrite(LED1, ledState);
      digitalWrite(LED2, LOW);
    }
  } else {
    // 未触摸 → LED2 常亮
    digitalWrite(LED1, LOW);
    digitalWrite(LED2, HIGH);
  }
}