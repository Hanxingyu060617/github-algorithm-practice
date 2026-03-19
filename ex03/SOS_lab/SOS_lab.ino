const int ledPin = 2;

unsigned long lastTime = 0;

// 速度适中，好观察
int dotTime   = 150;   // 短亮
int dashTime  = 400;   // 长亮
int gapTime   = 150;   // 熄灭间隔
int pauseTime = 1500;  // 一组结束停顿

// 0=短亮 1=短灭 2=长亮 3=长灭 4=结束停顿
int mode = 0;
int count = 0;

void setup() {
  pinMode(ledPin, OUTPUT);
  Serial.begin(9600);
  Serial.println("SOS 开始");
}

void loop() {
  unsigned long now = millis();

  // ================== 短亮 3 次 ==================
  if (mode == 0) {
    digitalWrite(ledPin, HIGH);
    Serial.println("短亮");
    if (now - lastTime >= dotTime) {
      lastTime = now;
      mode = 1;
    }
  }
  else if (mode == 1) {
    digitalWrite(ledPin, LOW);
    if (now - lastTime >= gapTime) {
      lastTime = now;
      count++;
      mode = 0;
      if (count >= 3) {
        count = 0;
        mode = 2;
        Serial.println("--- 3短亮完成，开始长亮 ---");
      }
    }
  }

  // ================== 长亮 3 次 ==================
  else if (mode == 2) {
    digitalWrite(ledPin, HIGH);
    Serial.println("长亮");
    if (now - lastTime >= dashTime) {
      lastTime = now;
      mode = 3;
    }
  }
  else if (mode == 3) {
    digitalWrite(ledPin, LOW);
    if (now - lastTime >= gapTime) {
      lastTime = now;
      count++;
      mode = 2;
      if (count >= 3) {
        count = 0;
        mode = 4;
        Serial.println("--- 3长亮完成，开始短亮 ---");
      }
    }
  }

  // ================== 最后 3 次短亮 ==================
  else if (mode == 4) {
    digitalWrite(ledPin, HIGH);
    Serial.println("短亮");
    if (now - lastTime >= dotTime) {
      lastTime = now;
      mode = 5;
    }
  }
  else if (mode == 5) {
    digitalWrite(ledPin, LOW);
    if (now - lastTime >= gapTime) {
      lastTime = now;
      count++;
      mode = 4;
      if (count >= 3) {
        count = 0;
        mode = 6;
        Serial.println("=== 一组SOS完成 ===");
      }
    }
  }

  // ================== 结束停顿 ==================
  else if (mode == 6) {
    digitalWrite(ledPin, LOW);
    if (now - lastTime >= pauseTime) {
      lastTime = now;
      mode = 0;
      Serial.println("\n=== 重新开始下一组 ===");
    }
  }
}
