#ifndef ROBOHAND_H
#define ROBOHAND_H

#include <Arduino.h>
#include <Servo.h>
#include <WiFi.h>
#include <PubSubClient.h>

class RoboHand {
  private:
    Servo thumb, index, middle, ring, pinky;
    int thumbPin, indexPin, middlePin, ringPin, pinkyPin;

    WiFiClient espClient;
    PubSubClient mqttClient;

    String ssid, password, broker;
    int port;
    String topic;

    static RoboHand* instance;  // pointer untuk callback MQTT

    static void mqttCallback(char* topic, byte* payload, unsigned int length);

  public:
    RoboHand(int t, int i, int m, int r, int p);

    void begin();
    void moveFinger(String finger, int angle);
    void moveAll(int t, int i, int m, int r, int p);

    void connectWiFi(const char* ssid, const char* password);
    void connectMQTT(const char* broker, int port, const char* topic);
    void loop();
};

#endif
