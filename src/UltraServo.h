#ifndef ULTRASERVO_H
#define ULTRASERVO_H

#include <Arduino.h>
#include <Servo.h>

class UltraServo {
  private:
    int trigPin;
    int echoPin;
    int servoPin;
    Servo myServo;

  public:
    UltraServo(int trig, int echo, int servo);
    void begin();
    long readDistance();
    void moveServo(int angle);
    void update(); // otomatis baca jarak & gerak servo
};

#endif
