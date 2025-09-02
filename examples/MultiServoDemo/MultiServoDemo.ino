#include <RoboHand.h>

// definisi pin servo jari
RoboHand myHand(13, 14, 15, 16, 17);

void setup() {
  Serial.begin(115200);
  myHand.begin();
}

void loop() {
  // gerak semua jari bersamaan
  myHand.moveAll(0, 0, 0, 0, 0);
  delay(1000);
  myHand.moveAll(90, 90, 90, 90, 90);
  delay(1000);
  myHand.moveAll(180, 180, 180, 180, 180);
  delay(1000);

  // gerak per jari
  myHand.moveFinger("thumb", 45);
  delay(500);
  myHand.moveFinger("index", 120);
  delay(500);
  myHand.moveFinger("middle", 60);
  delay(500);
  myHand.moveFinger("ring", 90);
  delay(500);
  myHand.moveFinger("pinky", 150);
  delay(500);
}
