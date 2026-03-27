#define TOUCH_PIN 4
#define LED_PIN 2

int touchValue;
bool ledState = false;

bool lastTouched = false;
bool triggered = false;
unsigned long lastDebounceTime = 0;
const unsigned long debounceDelay = 200;

void setup() {
  Serial.begin(115200);
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, ledState);  
}

void loop() {
  touchValue = touchRead(TOUCH_PIN);
  Serial.print("Touch Value: ");
  Serial.println(touchValue);

  bool isTouched = (touchValue < 400);


  if (isTouched && !lastTouched && !triggered) {
    if (millis() - lastDebounceTime > debounceDelay) {
      ledState = !ledState;
      digitalWrite(LED_PIN, ledState);
      lastDebounceTime = millis();
      triggered = true;
    }
  }

 
  if (!isTouched) {
    triggered = false;
  }

  lastTouched = isTouched;
}