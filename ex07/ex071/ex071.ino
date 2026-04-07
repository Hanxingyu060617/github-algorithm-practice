#include <WiFi.h>
#include <WebServer.h>

const char* ssid = "xswl";
const char* password = "hxy060617";

// 定义 LED 引脚
const int ledPin = 2;  

// 设置 PWM 属性
const int freq = 5000;          // 频率 5000Hz
const int resolution = 8;       // 分辨率 8位 (0-255)

// 创建 Web 服务器对象，端口为 80
WebServer server(80);

// --- HTML 网页源码 ---
void handleRoot() {
  String html = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
  <meta charset="UTF-8">
  <meta name="viewport" content="width=device-width, initial-scale=1.0">
  <title>ESP32 控制面板</title>
  <style>
    body { font-family: sans-serif; text-align: center; padding-top: 50px; background-color: #f4f4f9; }
    .container { max-width: 400px; margin: auto; padding: 20px; background: white; border-radius: 15px; box-shadow: 0 4px 6px rgba(0,0,0,0.1); }
    h2 { color: #333; }
    input[type=range] { width: 100%; margin: 20px 0; cursor: pointer; }
    .val-box { font-size: 2rem; color: #007bff; font-weight: bold; }
  </style>
</head>
<body>
  <div class="container">
    <h2>LED 亮度调节</h2>
    <input type="range" min="0" max="255" value="0" oninput="sendPWM(this.value)">
    <div class="val-box" id="disp">0</div>
    <p>滑动以实时调整</p>
  </div>

  <script>
    function sendPWM(val) {
      document.getElementById('disp').innerText = val;
      // 使用 fetch 发送异步 GET 请求，不刷新页面
      fetch('/setPWM?v=' + val).catch(err => console.log(err));
    }
  </script>
</body>
</html>
)rawliteral";
  server.send(200, "text/html; charset=UTF-8", html);
}

// --- 处理亮度请求的 API ---
void handlePWM() {
  if (server.hasArg("v")) {
    String valString = server.arg("v");
    int brightness = valString.toInt();
    
    // 使用 ESP32 新版 PWM 写入函数
    ledcWrite(ledPin, brightness); 
    
    server.send(200, "text/plain", "OK");
    Serial.printf("当前亮度值: %d\n", brightness);
  }
}

void setup() {
  Serial.begin(115200);

  // 1. 初始化 PWM (适配 ESP32 Arduino Core 3.0+ 新标准)
  ledcAttach(ledPin, freq, resolution);
  ledcWrite(ledPin, 0); // 初始关闭

  // 2. 连接 WiFi
  WiFi.begin(ssid, password);
  Serial.print("正在连接 WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi 已连接！");
  Serial.print("在浏览器输入此地址访问: http://");
  Serial.println(WiFi.localIP());

  // 3. 配置路由
  server.on("/", handleRoot);          // 访问根目录显示网页
  server.on("/setPWM", handlePWM);     // 接收滑动条数据
  
  // 4. 启动服务器
  server.begin();
}

void loop() {
  // 持续监听客户端请求
  server.handleClient();
}