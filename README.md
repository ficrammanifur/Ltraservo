# UltraServo

Library sederhana untuk ESP32 yang menghubungkan sensor ultrasonic (HC-SR04) dengan servo.  
Servo akan bergerak sesuai jarak yang terbaca dari sensor.

## 📌 Cara Pakai
```cpp
#include <UltraServo.h>

UltraServo myRobot(5, 18, 19);

void setup() {
  Serial.begin(115200);
  myRobot.begin();
}

void loop() {
  myRobot.update(); // otomatis mapping jarak ke servo
}
# Ltraservo
