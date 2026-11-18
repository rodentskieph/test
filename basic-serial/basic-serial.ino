const int ledPin = 7;

void setup() {
  pinMode(ledPin, OUTPUT);
  Serial.begin(9600);
}

void loop() {
  if (Serial.available() > 0) {
    char c = Serial.read();

    if (c == 'a') {
      digitalWrite(ledPin, HIGH);
    }

    if (c == 'b') {
      digitalWrite(ledPin, LOW);
    }

    if (c == 'c') {
      while (true) {
        digitalWrite(ledPin, HIGH);
        delay(500);
        digitalWrite(ledPin, LOW);
        delay(500);

        if (Serial.available() > 0) {
          break;
        }
      }
    }
  }
}
