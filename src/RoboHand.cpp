#include "RoboHand.h"
#include <ArduinoJson.h>

RoboHand* RoboHand::instance = nullptr;

RoboHand::RoboHand(int t, int i, int m, int r, int p) 
  : mqttClient(espClient) {
  thumbPin = t;
  indexPin = i;
  middlePin = m;
  ringPin = r;
  pinkyPin = p;
  instance = this; // simpan pointer instance
}

void RoboHand::begin() {
  thumb.attach(thumbPin);
  index.attach(indexPin);
  middle.attach(middlePin);
  ring.attach(ringPin);
  pinky.attach(pinkyPin);
}

void RoboHand::moveFinger(String finger, int angle) {
  angle = constrain(angle, 0, 180);
  if (finger == "thumb") thumb.write(angle);
  else if (finger == "index") index.write(angle);
  else if (finger == "middle") middle.write(angle);
  else if (finger == "ring") ring.write(angle);
  else if (finger == "pinky") pinky.write(angle);
}

void RoboHand::moveAll(int t, int i, int m, int r, int p) {
  thumb.write(constrain(t, 0, 180));
  index.write(constrain(i, 0, 180));
  middle.write(constrain(m, 0, 180));
  ring.write(constrain(r, 0, 180));
  pinky.write(constrain(p, 0, 180));
}

void RoboHand::connectWiFi(const char* s, const char* p) {
  ssid = s; password = p;
  WiFi.begin(ssid.c_str(), password.c_str());
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi connected!");
}

void RoboHand::connectMQTT(const char* b, int prt, const char* tp) {
  broker = b; port = prt; topic = tp;
  mqttClient.setServer(broker.c_str(), port);
  mqttClient.setCallback(mqttCallback);

  while (!mqttClient.connected()) {
    if (mqttClient.connect("RoboHandClient")) {
      Serial.println("MQTT connected!");
      mqttClient.subscribe(topic.c_str());
    } else {
      delay(1000);
    }
  }
}

void RoboHand::loop() {
  mqttClient.loop();
}

// static callback
void RoboHand::mqttCallback(char* topic, byte* payload, unsigned int length) {
  String msg;
  for (unsigned int i = 0; i < length; i++) {
    msg += (char)payload[i];
  }

  StaticJsonDocument<200> doc;
  DeserializationError error = deserializeJson(doc, msg);
  if (error) return;

  int t = doc["thumb"];
  int i = doc["index"];
  int m = doc["middle"];
  int r = doc["ring"];
  int p = doc["pinky"];

  instance->moveAll(t, i, m, r, p);
}
