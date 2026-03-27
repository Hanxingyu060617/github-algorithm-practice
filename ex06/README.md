## ESP32 警车双闪灯效（双通道PWM反相渐变）

## 实验名称
完成警车双闪灯效（双通道PWM）

## 关联基础
实验3 (PWM呼吸灯)

## 实验要求
- 使用两个LED，分别连接不同GPIO引脚
- 初始化两路独立PWM输出通道
- 实现两个LED**反相渐变**亮灭
- 一个LED亮度递增时，另一个LED亮度递减
- 实现平滑柔和的交替闪烁效果

## 硬件接线
| 器件 | ESP32 引脚 |
|------|------------|
| LED1 | GPIO2 |
| LED2 | GPIO4 |
| 两个LED负极 | GND |

## 功能说明
- 双路独立PWM输出，使用 `ledcAttach` 初始化通道
- 两个LED亮度**反向同步变化**
- LED1 渐亮时，LED2 渐暗
- LED1 渐暗时，LED2 渐亮
- 效果柔和流畅，无闪烁突变

## 代码特点
1. **双通道PWM控制**：两个LED独立控制，互不干扰
2. **反相渐变逻辑**：`ledcWrite(ledPin2, 255 - duty)`
3. **平滑呼吸过渡**：亮度 0~255 连续渐变
4. **简洁易懂**：适合嵌入式PWM基础教学

## 预期实验结果
两个LED呈现平滑交替的渐变闪烁效果，一个变亮的同时另一个逐渐变暗，过渡非常柔和。

## 开发环境
- Arduino IDE
- ESP32 开发板
- 波特率：115200

const int ledPin1 = 2;
const int ledPin2 = 4;

const int freq = 5000;
const int resolution = 8;  

void setup() {
  Serial.begin(115200);

  // 初始化两个独立PWM通道
  ledcAttach(ledPin1, freq, resolution);
  ledcAttach(ledPin2, freq, resolution);
}

void loop() {
  // 反相渐变：LED1变亮，LED2变暗
  for (int duty = 0; duty <= 255; duty++) {
    ledcWrite(ledPin1, duty);
    ledcWrite(ledPin2, 255 - duty);
    delay(8);
  }

  // 反向渐变：LED1变暗，LED2变亮
  for (int duty = 255; duty >= 0; duty--) {
    ledcWrite(ledPin1, duty);
    ledcWrite(ledPin2, 255 - duty);
    delay(8);
  }
}


## 代码优化点
1. 使用 millis() 非阻塞计时替代 delay()，避免程序阻塞，提升运行流畅度。
2. 保留原有双LED反相渐变逻辑、串口输出格式与硬件接线。
3. 代码结构简洁，效果与原版完全一致，符合实验要求。

// 2=黄灯，4=红灯
const int ledYellow = 2;
const int ledRed    = 4;

const int freq = 5000;
const int resolution = 8;
const int interval = 8; // 渐变间隔

int duty = 0;
int fadeStep = 1;
unsigned long previousMillis = 0;

void setup() {
  Serial.begin(115200);
  ledcAttach(ledYellow, freq, resolution);
  ledcAttach(ledRed, freq, resolution);
  Serial.println("警车双闪已启动");
}

void loop() {
  unsigned long currentMillis = millis();

  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;

    duty += fadeStep;

    // 到达最亮 → 反转方向
    if (duty >= 255) {
      duty = 255;
      fadeStep = -1;
      Serial.println("黄灯亮，红灯暗");
    }

    // 到达最暗 → 反转方向
    if (duty <= 0) {
      duty = 0;
      fadeStep = 1;
      Serial.println("黄灯暗，红灯亮");
    }

    // 反相输出
    ledcWrite(ledYellow, duty);
    ledcWrite(ledRed, 255 - duty);
  }
}
