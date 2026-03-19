const int ledPin = 2;
unsigned long lastTime = 0;
const long interval = 500;

void setup() {
  pinMode(ledPin, OUTPUT);
  Serial.begin(9600);  // 打开串口
}

void loop() {
  unsigned long now = millis();

  if (now - lastTime >= interval) {
    lastTime = now;

    int state = digitalRead(ledPin);
    digitalWrite(ledPin, !state);

    // 灯亮就打印 ON，灯灭打印 OFF
    if (digitalRead(ledPin) == HIGH) {
      Serial.println("LED ON");
    } else {
      Serial.println("LED OFF");
    }
  }
}
