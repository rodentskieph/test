const int trigPin = 2;
const int echoPin = 3;
const unsigned long echoTimeout = 20000;

const int redLedPin = 4;
const int greenLedPin = 5;
const int yellowLedPin = 6;


const float redDistance = 42;
const float yellowDistance = 180;

void setup() {
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);


  pinMode(redLedPin, OUTPUT);
  pinMode(greenLedPin, OUTPUT);
  pinMode(yellowLedPin, OUTPUT);

  Serial.begin(9600);
}

void loop() {

  float distance = readDistance();
  Serial.println(distance);
  if (distance <= redDistance) {
    digitalWrite(redLedPin, HIGH);
    digitalWrite(greenLedPin, LOW);
    digitalWrite(yellowLedPin, LOW);
    return;
  }

  if (distance > redDistance && distance <= yellowDistance) {
    digitalWrite(redLedPin, LOW);
    digitalWrite(greenLedPin, LOW);
    digitalWrite(yellowLedPin, HIGH);
    return;
  }


  digitalWrite(redLedPin, LOW);
  digitalWrite(greenLedPin, HIGH);
  digitalWrite(yellowLedPin, LOW);
}

int readDistance() {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  long dur = pulseIn(echoPin, HIGH, echoTimeout);
  if (dur == 0) {
    return -1;  // no echo received within timeout
  }
  return dur * 0.034 / 2;
}
