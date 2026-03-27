const int ledPin = 2;
const int touchPin = 4;

const int freq = 5000;
const int resolution = 8;
#define THRESHOLD 400

volatile int speedGear = 1;
volatile bool triggerFlag = false;


volatile bool touchLocked = false;

int delayMs[] = {12, 6, 2}; 


void touchISR() {
  triggerFlag = true;
}

void setup() {
  Serial.begin(115200);
  ledcAttach(ledPin, freq, resolution);
  touchAttachInterrupt(touchPin, touchISR, THRESHOLD);
  Serial.println("OK"); 
}

void loop() {
  if (triggerFlag) {
    triggerFlag = false;

    
    if (!touchLocked) {
      speedGear++;
      if (speedGear > 3) speedGear = 1;
      
      Serial.println(speedGear); 
      touchLocked = true; 
    }
  }

  
  if (touchRead(touchPin) > THRESHOLD) {
    touchLocked = false;
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