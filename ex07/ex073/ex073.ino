#include <WiFi.h>
#include <WebServer.h>

const char* ssid = "xswl";
const char* password = "hxy060617";

// 定义两个灯的引脚
const int led1Pin = 2; // 左灯
const int led2Pin = 4; // 右灯

WebServer server(80);

// --- 极简网页 ---
void handleRoot() {
  String html = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
  <meta charset="UTF-8">
  <meta name="viewport" content="width=device-width, initial-scale=1.0">
  <title>光效跷跷板</title>
  <style>
    body { font-family: sans-serif; text-align: center; margin-top: 50px; background-color: #f0f0f0;}
    input[type=range] { width: 80%; max-width: 300px; margin: 30px 0; height: 30px; }
    .lights { display: flex; justify-content: center; gap: 80px; font-size: 20px; font-weight: bold; color: #333;}
    .val { color: #007bff; }
  </style>
</head>
<body>
  <h1>⚖️ 光效跷跷板</h1>
  <p>拖动滑块，控制亮度的“重心”</p>
  
  <div class="lights">
    <div>左灯亮度<br><span class="val" id="val1">128</span></div>
    <div>右灯亮度<br><span class="val" id="val2">127</span></div>
  </div>

  <input type="range" min="0" max="255" value="128" oninput="updateBalance(this.value)">

  <script>
    function updateBalance(val) {
      // 前端显示数值变化
      document.getElementById('val1').innerText = val;
      document.getElementById('val2').innerText = 255 - val;
      
      // 发送当前滑块的数值给 ESP32
      fetch('/setBalance?v=' + val);
    }
  </script>
</body>
</html>
)rawliteral";
  server.send(200, "text/html; charset=UTF-8", html);
}

// --- 处理亮度分配逻辑 ---
void handleBalance() {
  if (server.hasArg("v")) {
    int val = server.arg("v").toInt(); // 接收滑块的数值 (0-255)
    
    // 【核心进阶逻辑】数据分配
    int leftVal = val;            // 左灯亮度 = 滑块数值
    int rightVal = 255 - val;     // 右灯亮度 = 255 减去 滑块数值
    
    // 分别写入两个 PWM 通道
    ledcWrite(led1Pin, leftVal);
    ledcWrite(led2Pin, rightVal);
    
    server.send(200, "text/plain", "OK");
  }
}

void setup() {
  Serial.begin(115200);
  
  // 初始化两个引脚的 PWM
  ledcAttach(led1Pin, 5000, 8);
  ledcAttach(led2Pin, 5000, 8);
  
  // 初始状态：重心在中间，各一半亮度
  ledcWrite(led1Pin, 128);
  ledcWrite(led2Pin, 127);

  WiFi.begin(ssid, password);
  Serial.print("连接中...");
  while (WiFi.status() != WL_CONNECTED) { delay(500); Serial.print("."); }
  Serial.println("\n连接成功! 访问IP: " + WiFi.localIP().toString());

  server.on("/", handleRoot);
  server.on("/setBalance", handleBalance);
  server.begin();
}

void loop() {
  server.handleClient();
}