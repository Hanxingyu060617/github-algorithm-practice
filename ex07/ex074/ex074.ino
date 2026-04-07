#include <WiFi.h>
#include <WebServer.h>

// AP热点配置
const char* ap_ssid = "ESP32-LAB";
const char* ap_pass = "12345678"; // 密码至少需要8位

const int led1Pin = 2; // 左灯
const int led2Pin = 4; // 右灯

WebServer server(80);

// 全局状态变量
bool autoMode = false;      
int currentVal = 128;       // 统一记录当前的亮度基准值 (0-255)
int autoDirection = 1;      
unsigned long lastMillis = 0; 

// 网页界面 
void handleRoot() {
  String html = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
  <meta charset="UTF-8">
  <meta name="viewport" content="width=device-width, initial-scale=1.0">
  <title>光效跷跷板 (便携同步版)</title>
  <style>
    body { font-family: sans-serif; text-align: center; margin-top: 50px; background-color: #f0f0f0;}
    input[type=range] { width: 80%; max-width: 300px; margin: 30px 0; height: 30px; }
    .lights { display: flex; justify-content: center; gap: 80px; font-size: 20px; font-weight: bold; color: #333;}
    .val { color: #007bff; }
    button { padding: 12px 24px; font-size: 16px; font-weight: bold; border-radius: 8px; border: none; background-color: #28a745; color: white; cursor: pointer; margin-bottom: 20px;}
    button.active { background-color: #dc3545; }
  </style>
</head>
<body>
  <h1>⚖️ 光效跷跷板</h1>
  <p>便携式 AP 网络控制台</p>
  
  <button id="autoBtn" onclick="toggleAuto()">启动自动摇摆</button>
  
  <div class="lights">
    <div>左灯亮度<br><span class="val" id="val1">128</span></div>
    <div>右灯亮度<br><span class="val" id="val2">127</span></div>
  </div>

  <input type="range" id="slider" min="0" max="255" value="128" oninput="updateBalance(this.value)">

  <script>
    let isAuto = false;
    let syncTimer = null; // 用于存放定时器的变量

    function updateBalance(val) {
      document.getElementById('val1').innerText = val;
      document.getElementById('val2').innerText = 255 - val;
      document.getElementById('slider').value = val;
    }

    // 手动滑动时触发
    function manualSlide(val) {
      updateBalance(val);
      fetch('/setBalance?v=' + val);
    }
    // 绑定原生滑动事件
    document.getElementById('slider').oninput = function() { manualSlide(this.value); };

    function toggleAuto() {
      isAuto = !isAuto;
      const btn = document.getElementById('autoBtn');
      const slider = document.getElementById('slider');
      
      if (isAuto) {
        btn.innerText = "停止自动摇摆";
        btn.classList.add("active");
        slider.disabled = true; // 禁用滑块防止冲突
        fetch('/setAuto?state=1'); 

        // 开启 AJAX 轮询
        syncTimer = setInterval(() => {
          fetch('/getVal')
            .then(response => response.text())
            .then(val => {
              updateBalance(val); // 自动更新数字和滑块的位置！
            });
        }, 80); 

      } else {
        btn.innerText = "启动自动摇摆";
        btn.classList.remove("active");
        slider.disabled = false; 
        fetch('/setAuto?state=0'); 
        
        // 停止轮询
        if (syncTimer) clearInterval(syncTimer); 
      }
    }
  </script>
</body>
</html>
)rawliteral";
  server.send(200, "text/html; charset=UTF-8", html);
}

// 处理手动请求 
void handleBalance() {
  if (server.hasArg("v")) {
    currentVal = server.arg("v").toInt(); // 保持状态同步
    autoMode = false; 
    
    ledcWrite(led1Pin, currentVal);
    ledcWrite(led2Pin, 255 - currentVal);
    server.send(200, "text/plain", "OK");
  }
}

// 处理自动模式开关
void handleAuto() {
  if (server.hasArg("state")) {
    int state = server.arg("state").toInt();
    autoMode = (state == 1); 
    server.send(200, "text/plain", "OK");
  }
}

// 新增状态回传 API
// 当网页请求此地址时，返回当前的数值
void handleGetVal() {
  server.send(200, "text/plain", String(currentVal));
}

void setup() {
  Serial.begin(115200);
  delay(100); // 等待串口稳定

  // ========== 修复：适配ESP32 Core 3.0+的PWM初始化 ==========
  pinMode(led1Pin, OUTPUT);
  pinMode(led2Pin, OUTPUT);
  
  // 新版API：直接用ledcAttach完成引脚、频率、分辨率配置，自动分配通道
  ledcAttach(led1Pin, 5000, 8);
  ledcAttach(led2Pin, 5000, 8);

  ledcWrite(led1Pin, currentVal);
  ledcWrite(led2Pin, 255 - currentVal);

  // 【启动 AP 模式】
  WiFi.mode(WIFI_AP); 
  WiFi.softAP(ap_ssid, ap_pass);

  Serial.println("\n--- WiFi 热点已开启 ---");
  Serial.print("WiFi名称: "); Serial.println(ap_ssid);
  Serial.print("WiFi密码: "); Serial.println(ap_pass);
  Serial.print("控制台IP: http://"); 
  Serial.println(WiFi.softAPIP()); // 默认通常是 192.168.4.1

  server.on("/", handleRoot);
  server.on("/setBalance", handleBalance);
  server.on("/setAuto", handleAuto);
  server.on("/getVal", handleGetVal); // 注册数据请求路由
  server.begin();
  
  Serial.println("服务器启动成功！");
}

void loop() {
  server.handleClient();

  // 自动摇摆引擎
  if (autoMode) {
    unsigned long currentMillis = millis();
    if (currentMillis - lastMillis >= 15) { // 稍微调慢一点点，配合网络传输频率
      lastMillis = currentMillis;

      currentVal = currentVal + autoDirection;

      if (currentVal >= 255) {
        currentVal = 255;
        autoDirection = -1; 
      } else if (currentVal <= 0) {
        currentVal = 0;
        autoDirection = 1;  
      }

      ledcWrite(led1Pin, currentVal);
      ledcWrite(led2Pin, 255 - currentVal);
    }
  }
}