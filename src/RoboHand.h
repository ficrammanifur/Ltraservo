#include "UltraServo.h"

UltraServo::UltraServo(int trig, int echo, int servo) {
  trigPin = trig;
  echoPin = echo;
  servoPin = servo;
}

void UltraServo::begin() {
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  myServo.attach(servoPin);
}

long UltraServo::readDistance() {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  long duration = pulseIn(echoPin, HIGH);
  long distance = duration * 0.034 / 2; // cm
  return distance;
}

void UltraServo::moveServo(int angle) {
  myServo.write(angle);
}

void UltraServo::update() {
  long d = readDistance();
  int angle = map(d, 0, 100, 0, 180); // mapping jarak ke sudut servo
  angle = constrain(angle, 0, 180);
  moveServo(angle);
}
