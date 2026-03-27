### ESP32 触摸中断调速呼吸灯

## 实验功能
基于 ESP32 触摸中断实现 **三档位 PWM 呼吸灯调速**，支持触摸切换呼吸速度，具备软件防抖，LED 持续呼吸不中断。

## 实验要求
- 使用 ESP32 内置触摸传感器（GPIO4）
- 通过中断方式检测触摸信号
- 实现 3 档呼吸速度切换（慢 → 中 → 快 → 循环）
- 加入防抖机制，避免误触
- 使用 PWM 驱动 LED 实现平滑呼吸效果
- 串口输出当前档位信息

## 硬件接线
| 器件 | ESP32 引脚 |
|------|------------|
| LED 正极 | GPIO2 |
| LED 负极 | GND |
| 触摸端 / 金属片 | GPIO4 |

> 可直接使用 ESP32 板载 LED，无需外接电路。

## 代码特点
1. **中断驱动触摸检测**  
   采用 `touchAttachInterrupt` 实现触摸触发，不占用主循环资源。
2. **三档位速度调节**  
   3 档呼吸速度，档位差异明显，肉眼可清晰辨别。
3. **300ms 软件防抖**  
   防止手抖、干扰导致的档位频繁跳动。
4. **非阻塞式档位切换**  
   触摸响应灵敏，不影响呼吸灯流畅度。
5. **PWM 呼吸灯**  
   使用 `ledcAttach` / `ledcWrite` 实现 0~255 亮度渐变。
6. **串口实时输出档位**  
   方便调试与观察状态。

## 运行效果
- 上电自动启动慢速呼吸
- 触摸一次 → 切换为中速呼吸
- 再触摸一次 → 切换为快速呼吸
- 再触摸一次 → 回到慢速呼吸
- 每次切换串口打印：`当前档位：x`

## 开发环境
- Arduino IDE
- ESP32 开发板
- 波特率：115200

## 档位说明
- 档位 1：慢速呼吸（delay 12ms）
- 档位 2：中速呼吸（delay 6ms）
- 档位 3：快速呼吸（delay 2ms）
const int ledPin = 2;
const int touchPin = 4;

const int freq = 5000;
const int resolution = 8;
#define THRESHOLD 400

volatile int speedGear = 1;
volatile bool triggerFlag = false;
unsigned long lastTouchTime = 0;
const unsigned long debounceTime = 300; 

int delayMs[] = {12, 6, 2}; 

void touchISR() {
  triggerFlag = true; 
}

void setup() {
  Serial.begin(115200);
  ledcAttach(ledPin, freq, resolution);
  touchAttachInterrupt(touchPin, touchISR, THRESHOLD);
  Serial.println("系统启动：带防抖触摸调速呼吸灯");
}

void loop() {

  if (triggerFlag) {
    unsigned long currentTime = millis();
    if (currentTime - lastTouchTime > debounceTime) {
      
      speedGear++;
      if (speedGear > 3) speedGear = 1;

      Serial.print("当前档位：");
      Serial.println(speedGear);

      lastTouchTime = currentTime; 
    }
    triggerFlag = false; 
  }

  
  for (int duty = 0; duty <= 255; duty++) {
    ledcWrite(ledPin, duty);
    delay(delayMs[speedGear - 1]);
  }
  for (int duty = 255; duty >= 0; duty--) {
    ledcWrite(ledPin, duty);
    delay(delayMs[speedGear - 1]);
  }
}
