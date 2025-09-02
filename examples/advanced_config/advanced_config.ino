#include <Robohand.h>

Robohand hand;

int customPins[5] = {18, 19, 21, 22, 23};  // Custom GPIO pins

void setup() {
    Serial.begin(115200);
    hand.begin("YOUR_WIFI_SSID", "YOUR_WIFI_PASSWORD", "your.mqtt.server", 1883);
    hand.setServoPins(customPins);
    hand.setMqttTopic("advanced/robohand");
}

void loop() {
    hand.loop();
}