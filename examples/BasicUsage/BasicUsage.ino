#include <UltraServo.h>

// pin trigger, echo, servo
UltraServo myRobot(5, 18, 19); 

void setup() {
  Serial.begin(115200);
  myRobot.begin();
}

void loop() {
  myRobot.update();  
  long jarak = myRobot.readDistance();
  Serial.print("Jarak: ");
  Serial.print(jarak);
  Serial.println(" cm");
  delay(200);
}
