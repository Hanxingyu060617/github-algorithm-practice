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
