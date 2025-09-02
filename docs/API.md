# Robohand API Documentation

## Class: Robohand

### Constructor
- `Robohand(const char* device_id)`: Creates instance with unique device ID.

### Configuration
- `void setWiFiCredentials(const char* ssid, const char* password)`: Set WiFi credentials.
- `void setMQTTServer(const char* server, int port = 1883)`: Set MQTT broker.
- `void setMQTTCredentials(const char* user, const char* password)`: Set MQTT authentication (optional).
- `void setHeartbeatInterval(unsigned long interval_ms)`: Set heartbeat interval in milliseconds.
- `bool addServo(uint8_t pin, const char* name, int min_angle = 0, int max_angle = 180)`: Add a servo with pin, name, and angle range.

### Initialization
- `bool begin()`: Initialize WiFi, MQTT, and servos. Returns true if successful.

### Main Loop
- `void loop()`: Handles WiFi, MQTT, and heartbeat. Call in Arduino `loop()`.

### Servo Control
- `bool setServoAngle(const char* name, int angle)`: Set servo angle by name.
- `bool setServoAngle(int index, int angle)`: Set servo angle by index.
- `int getServoAngle(const char* name)`: Get current angle by name.
- `int getServoCount()`: Get number of configured servos.

### Status Checks
- `bool isWiFiConnected()`: Check WiFi connection status.
- `bool isMQTTConnected()`: Check MQTT connection status.
- `bool isConnected()`: Check if both WiFi and MQTT are connected.

## MQTT Topics
- **Publish to**: `robohand/[device_id]/cmd/servo`
  - Single servo: `{"servo": "thumb", "angle": 45}`
  - Multiple servos: `{"servos": [{"name": "thumb", "angle": 0}, {"name": "index", "angle": 90}]}`
  - Gestures: `{"gesture": "open"}` (open, fist, peace, point, thumbs_up)
  - Real-time: `{"fingers": [0.0, 0.5, 1.0, 0.8, 0.2]}` (0.0 = open, 1.0 = closed)
- **Subscribe to**:
  - `robohand/[device_id]/servo_status`: Servo positions (`{"servos": [{"name": "thumb", "angle": 90}, ...]}`)
  - `robohand/[device_id]/status`: Connection status
  - `robohand/[device_id]/heartbeat`: Periodic heartbeats