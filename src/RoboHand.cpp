#include "Robohand.h"

Robohand::Robohand(const char* device_id) : client(espClient), device_id(device_id) {
    // Initialize MQTT topics
    cmd_topic = "robohand/" + String(device_id) + "/cmd/servo";
    status_topic = "robohand/" + String(device_id) + "/status";
    servo_status_topic = "robohand/" + String(device_id) + "/servo_status";
    heartbeat_topic = "robohand/" + String(device_id) + "/heartbeat";
    for (int i = 0; i < MAX_SERVOS; i++) {
        servos[i].active = false;
    }
}

void Robohand::setWiFiCredentials(const char* ssid, const char* password) {
    wifi_ssid = ssid;
    wifi_password = password;
}

void Robohand::setMQTTServer(const char* server, int port) {
    mqtt_server = server;
    mqtt_port = port;
}

void Robohand::setMQTTCredentials(const char* user, const char* password) {
    mqtt_user = user;
    mqtt_password = password;
}

void Robohand::setHeartbeatInterval(unsigned long interval_ms) {
    heartbeat_interval = interval_ms;
}

bool Robohand::addServo(uint8_t pin, const char* name, int min_angle, int max_angle) {
    if (servo_count >= MAX_SERVOS) {
        Serial.println("Error: Maximum servo count reached");
        return false;
    }
    servos[servo_count].pin = pin;
    servos[servo_count].name = String(name);
    servos[servo_count].min_angle = min_angle;
    servos[servo_count].max_angle = max_angle;
    servos[servo_count].current_angle = (min_angle + max_angle) / 2;
    servos[servo_count].active = true;
    servo_count++;
    return true;
}

bool Robohand::begin() {
    // Validate configuration
    if (!wifi_ssid || !wifi_password || !mqtt_server) {
        Serial.println("Error: WiFi or MQTT configuration missing");
        return false;
    }

    // Connect to WiFi
    WiFi.begin(wifi_ssid, wifi_password);
    unsigned long startAttemptTime = millis();
    while (WiFi.status() != WL_CONNECTED && millis() - startAttemptTime < 10000) {
        delay(500);
        Serial.print(".");
    }
    if (WiFi.status() != WL_CONNECTED) {
        Serial.println("WiFi connection failed");
        return false;
    }
    Serial.println("WiFi connected. IP: " + WiFi.localIP().toString());

    // Setup MQTT
    client.setServer(mqtt_server, mqtt_port);
    client.setCallback([this](char* topic, byte* payload, unsigned int length) {
        this->callback(topic, payload, length);
    });

    // Attach servos
    for (int i = 0; i < servo_count; i++) {
        if (servos[i].active) {
            servos[i].servo.attach(servos[i].pin);
            servos[i].servo.write(servos[i].current_angle);
        }
    }

    // Connect to MQTT
    reconnect();
    return true;
}

void Robohand::loop() {
    if (!client.connected()) {
        reconnect();
    }
    client.loop();

    // Publish heartbeat if interval elapsed
    if (heartbeat_interval > 0 && millis() - last_heartbeat >= heartbeat_interval) {
        publishHeartbeat();
        last_heartbeat = millis();
    }
}

void Robohand::reconnect() {
    while (!client.connected()) {
        Serial.print("Connecting to MQTT...");
        String clientId = "Robohand-" + String(device_id);
        bool connected = mqtt_user ? client.connect(clientId.c_str(), mqtt_user, mqtt_password) :
                                    client.connect(clientId.c_str());
        if (connected) {
            Serial.println("connected");
            client.subscribe(cmd_topic.c_str());
            publishStatus();
        } else {
            Serial.print("failed, rc=");
            Serial.print(client.state());
            Serial.println(" retrying in 5 seconds");
            delay(5000);
        }
    }
}

void Robohand::publishStatus() {
    StaticJsonDocument<128> doc;
    doc["wifi"] = isWiFiConnected() ? "connected" : "disconnected";
    doc["mqtt"] = isMQTTConnected() ? "connected" : "disconnected";
    doc["heap"] = ESP.getFreeHeap();
    doc["uptime"] = millis() / 1000;
    String json;
    serializeJson(doc, json);
    client.publish(status_topic.c_str(), json.c_str());
}

void Robohand::publishServoStatus() {
    StaticJsonDocument<256> doc;
    JsonArray servoArray = doc.createNestedArray("servos");
    for (int i = 0; i < servo_count; i++) {
        if (servos[i].active) {
            JsonObject servo = servoArray.createNestedObject();
            servo["name"] = servos[i].name;
            servo["angle"] = servos[i].current_angle;
        }
    }
    String json;
    serializeJson(doc, json);
    client.publish(servo_status_topic.c_str(), json.c_str());
}

void Robohand::publishHeartbeat() {
    StaticJsonDocument<64> doc;
    doc["device_id"] = device_id;
    doc["timestamp"] = millis();
    String json;
    serializeJson(doc, json);
    client.publish(heartbeat_topic.c_str(), json.c_str());
}

bool Robohand::setServoAngle(const char* name, int angle) {
    int index = findServoIndex(name);
    if (index == -1) return false;
    return setServoAngle(index, angle);
}

bool Robohand::setServoAngle(int index, int angle) {
    if (index < 0 || index >= servo_count || !servos[index].active) {
        Serial.println("Error: Invalid servo index");
        return false;
    }
    angle = constrain(angle, servos[index].min_angle, servos[index].max_angle);
    servos[index].servo.write(angle);
    servos[index].current_angle = angle;
    publishServoStatus();
    return true;
}

int Robohand::getServoAngle(const char* name) {
    int index = findServoIndex(name);
    if (index == -1) return -1;
    return servos[index].current_angle;
}

int Robohand::findServoIndex(const char* name) {
    for (int i = 0; i < servo_count; i++) {
        if (servos[i].active && servos[i].name.equalsIgnoreCase(name)) {
            return i;
        }
    }
    Serial.println("Error: Servo " + String(name) + " not found");
    return -1;
}

void Robohand::callback(char* topic, byte* payload, unsigned int length) {
    Serial.print("Message received [");
    Serial.print(topic);
    Serial.print("] ");
    char message[length + 1];
    for (int i = 0; i < length; i++) {
        message[i] = (char)payload[i];
    }
    message[length] = '\0';
    Serial.println(message);

    StaticJsonDocument<512> doc;
    DeserializationError error = deserializeJson(doc, message);
    if (error) {
        Serial.print("JSON parse error: ");
        Serial.println(error.c_str());
        return;
    }

    // Handle single servo command
    if (doc.containsKey("servo") && doc.containsKey("angle")) {
        const char* name = doc["servo"];
        int angle = doc["angle"];
        setServoAngle(name, angle);
    }
    // Handle multiple servo commands
    else if (doc.containsKey("servos")) {
        JsonArray servosArray = doc["servos"];
        for (JsonObject servo : servosArray) {
            const char* name = servo["name"];
            int angle = servo["angle"];
            setServoAngle(name, angle);
        }
    }
    // Handle gesture command
    else if (doc.containsKey("gesture")) {
        String gesture = doc["gesture"].as<String>();
        handleGestureCommand(gesture);
    }
    // Handle real-time finger positions
    else if (doc.containsKey("fingers")) {
        JsonArray fingers = doc["fingers"];
        if (fingers.size() == servo_count) {
            float positions[MAX_SERVOS];
            for (int i = 0; i < servo_count; i++) {
                positions[i] = fingers[i];
            }
            handleFingerPositions(positions);
        }
    }
}

void Robohand::handleGestureCommand(const String& gesture) {
    Serial.println("Handling gesture: " + gesture);
    if (gesture == "open") {
        for (int i = 0; i < servo_count; i++) {
            setServoAngle(i, servos[i].min_angle);
        }
    }
    else if (gesture == "fist") {
        for (int i = 0; i < servo_count; i++) {
            setServoAngle(i, servos[i].max_angle);
        }
    }
    else if (gesture == "peace") {
        setServoAngle("thumb", servos[findServoIndex("thumb")].max_angle);
        setServoAngle("index", servos[findServoIndex("index")].min_angle);
        setServoAngle("middle", servos[findServoIndex("middle")].min_angle);
        setServoAngle("ring", servos[findServoIndex("ring")].max_angle);
        setServoAngle("pinky", servos[findServoIndex("pinky")].max_angle);
    }
    else if (gesture == "point") {
        setServoAngle("thumb", servos[findServoIndex("thumb")].max_angle);
        setServoAngle("index", servos[findServoIndex("index")].min_angle);
        setServoAngle("middle", servos[findServoIndex("middle")].max_angle);
        setServoAngle("ring", servos[findServoIndex("ring")].max_angle);
        setServoAngle("pinky", servos[findServoIndex("pinky")].max_angle);
    }
    else if (gesture == "thumbs_up") {
        setServoAngle("thumb", servos[findServoIndex("thumb")].min_angle);
        setServoAngle("index", servos[findServoIndex("index")].max_angle);
        setServoAngle("middle", servos[findServoIndex("middle")].max_angle);
        setServoAngle("ring", servos[findServoIndex("ring")].max_angle);
        setServoAngle("pinky", servos[findServoIndex("pinky")].max_angle);
    }
}

void Robohand::handleFingerPositions(float positions[]) {
    for (int i = 0; i < servo_count; i++) {
        float bendRatio = constrain(positions[i], 0.0, 1.0);
        int angle = servos[i].min_angle + (bendRatio * (servos[i].max_angle - servos[i].min_angle));
        setServoAngle(i, angle);
    }
}