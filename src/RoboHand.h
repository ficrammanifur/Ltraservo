#ifndef ROBOHAND_H
#define ROBOHAND_H

#include <Arduino.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include <ESP32Servo.h>
#include <ArduinoJson.h>

#define MAX_SERVOS 5  // Maximum number of servos (fingers)

class Robohand {
public:
    Robohand(const char* device_id);  // Constructor with device ID

    // Configuration
    void setWiFiCredentials(const char* ssid, const char* password);
    void setMQTTServer(const char* server, int port = 1883);
    void setMQTTCredentials(const char* user, const char* password);
    void setHeartbeatInterval(unsigned long interval_ms);
    bool addServo(uint8_t pin, const char* name, int min_angle = 0, int max_angle = 180);

    // Initialization
    bool begin();

    // Main loop
    void loop();

    // Servo control
    bool setServoAngle(const char* name, int angle);
    bool setServoAngle(int index, int angle);
    int getServoAngle(const char* name);
    int getServoCount() { return servo_count; }

    // Status checks
    bool isWiFiConnected() { return WiFi.status() == WL_CONNECTED; }
    bool isMQTTConnected() { return client.connected(); }
    bool isConnected() { return isWiFiConnected() && isMQTTConnected(); }

private:
    // Internal structures
    struct ServoConfig {
        Servo servo;
        String name;
        int pin;
        int min_angle;
        int max_angle;
        int current_angle;
        bool active;
    };

    // Member variables
    WiFiClient espClient;
    PubSubClient client;
    String device_id;
    const char* wifi_ssid = nullptr;
    const char* wifi_password = nullptr;
    const char* mqtt_server = nullptr;
    int mqtt_port = 1883;
    const char* mqtt_user = nullptr;
    const char* mqtt_password = nullptr;
    unsigned long heartbeat_interval = 30000;
    unsigned long last_heartbeat = 0;
    ServoConfig servos[MAX_SERVOS];
    int servo_count = 0;

    // MQTT topics
    String cmd_topic;
    String status_topic;
    String servo_status_topic;
    String heartbeat_topic;

    // Internal methods
    void reconnect();
    void publishStatus();
    void publishServoStatus();
    void publishHeartbeat();
    void callback(char* topic, byte* payload, unsigned int length);
    void handleGestureCommand(const String& gesture);
    void handleFingerPositions(float positions[]);
    int findServoIndex(const char* name);
};

#endif