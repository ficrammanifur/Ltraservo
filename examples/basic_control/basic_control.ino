#include <Robohand.h>

Robohand hand;

void setup() {
    Serial.begin(115200);
    hand.begin("YOUR_WIFI_SSID", "YOUR_WIFI_PASSWORD", "broker.hivemq.com", 1883);
    // Optional: int customPins[5] = {12, 13, 14, 15, 16}; hand.setServoPins(customPins);
    // Optional: hand.setMqttTopic("my/custom/topic");
}

void loop() {
    hand.loop();
}