#include <RoboHand.h>

// jari: thumb, index, middle, ring, pinky
RoboHand myHand(13, 14, 15, 16, 17);

const char* ssid = "YOUR_WIFI";
const char* pass = "YOUR_PASS";
const char* broker = "broker.hivemq.com";
const int port = 1883;
const char* topic = "robohand/fingers";

void setup() {
  Serial.begin(115200);
  myHand.begin();
  myHand.connectWiFi(ssid, pass);
  myHand.connectMQTT(broker, port, topic);
}

void loop() {
  myHand.loop();
}
