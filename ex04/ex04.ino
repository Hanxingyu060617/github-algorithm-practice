#define TOUCH_PIN 4
#define LED_PIN 2

int touchValue;
bool ledState = false;

bool lastTouched = false;
bool triggered = false;
unsigned long lastDebounceTime = 0;
const unsigned long debounceDelay = 200;  // 防抖时间

void setup() {
  Serial.begin(115200);
  pinMode(LED_PIN, OUTPUT);
}

void loop() {
  touchValue = touchRead(TOUCH_PIN);
  Serial.print("Touch Value: ");
  Serial.println(touchValue);

  bool isTouched = (touchValue < 400);

  // 防抖 + 边沿检测 + 只触发一次
  if (isTouched && !lastTouched && !triggered && 
      millis() - lastDebounceTime > debounceDelay) {
    
    ledState = !ledState;
    digitalWrite(LED_PIN, ledState);
    
    lastDebounceTime = millis();
    triggered = true;  // 锁定，按住不再触发
  }

  // 只有手松开后才解锁，允许下一次触摸
  if (!isTouched) {
    triggered = false;
  }

  lastTouched = isTouched;
  delay(20);
}
