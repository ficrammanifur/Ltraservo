# Basic Control Example

This example demonstrates the fundamental usage of the Robohand library for controlling servos via MQTT.

## Features

- WiFi connection management
- MQTT broker connection
- 5-servo robotic hand control
- Real-time servo status reporting
- Automatic reconnection handling
- Built-in servo demonstrations

## Hardware Requirements

- ESP32 development board
- 5x servo motors (SG90 or similar)
- External 5V power supply (recommended for servos)
- Breadboard and jumper wires

## Wiring Diagram

\`\`\`
ESP32 Pin | Servo | Function
----------|-------|----------
18        | Servo 1 | Thumb
19        | Servo 2 | Index finger
21        | Servo 3 | Middle finger
22        | Servo 4 | Ring finger
23        | Servo 5 | Pinky finger
5V        | VCC   | Power (external supply recommended)
GND       | GND   | Ground
\`\`\`

## Configuration

1. Update WiFi credentials in the sketch:
   \`\`\`cpp
   const char* WIFI_SSID = "YOUR_WIFI_SSID";
   const char* WIFI_PASSWORD = "YOUR_WIFI_PASSWORD";
   \`\`\`

2. Configure MQTT broker (default uses free public broker):
   \`\`\`cpp
   const char* MQTT_SERVER = "broker.hivemq.com";
   const int MQTT_PORT = 1883;
   \`\`\`

3. Set unique device ID:
   \`\`\`cpp
   const char* DEVICE_ID = "robohand_basic_001";
   \`\`\`

## MQTT Topics

### Command Topics (Publish to these)

- `robohand/[device_id]/cmd/servo` - Send servo control commands

### Status Topics (Subscribe to these)

- `robohand/[device_id]/status` - Device status and info
- `robohand/[device_id]/servo_status` - Current servo positions
- `robohand/[device_id]/heartbeat` - Periodic device heartbeat

## Command Examples

### Single Servo Control
\`\`\`json
{
  "servo": "thumb",
  "angle": 45
}
\`\`\`

### Multiple Servo Control
\`\`\`json
{
  "servos": [
    {"name": "thumb", "angle": 0},
    {"name": "index", "angle": 90},
    {"name": "middle", "angle": 180}
  ]
}
\`\`\`

### Predefined Gestures

**Open Hand:**
\`\`\`json
{
  "servos": [
    {"name": "thumb", "angle": 0},
    {"name": "index", "angle": 0},
    {"name": "middle", "angle": 0},
    {"name": "ring", "angle": 0},
    {"name": "pinky", "angle": 0}
  ]
}
\`\`\`

**Closed Fist:**
\`\`\`json
{
  "servos": [
    {"name": "thumb", "angle": 180},
    {"name": "index", "angle": 180},
    {"name": "middle", "angle": 180},
    {"name": "ring", "angle": 180},
    {"name": "pinky", "angle": 180}
  ]
}
\`\`\`

**Peace Sign:**
\`\`\`json
{
  "servos": [
    {"name": "thumb", "angle": 180},
    {"name": "index", "angle": 0},
    {"name": "middle", "angle": 0},
    {"name": "ring", "angle": 180},
    {"name": "pinky", "angle": 180}
  ]
}
\`\`\`

## Testing with MQTT Clients

### MQTT Explorer (Desktop)
1. Download from [mqtt-explorer.com](http://mqtt-explorer.com/)
2. Connect to `broker.hivemq.com:1883`
3. Navigate to `robohand/[your_device_id]/cmd/servo`
4. Publish JSON commands

### Mosquitto Command Line
\`\`\`bash
# Install mosquitto clients
sudo apt-get install mosquitto-clients

# Subscribe to status
mosquitto_sub -h broker.hivemq.com -t "robohand/robohand_basic_001/servo_status"

# Send command
mosquitto_pub -h broker.hivemq.com -t "robohand/robohand_basic_001/cmd/servo" -m '{"servo": "thumb", "angle": 90}'
\`\`\`

## Troubleshooting

### WiFi Connection Issues
- Verify SSID and password are correct
- Check WiFi signal strength
- Ensure 2.4GHz network (ESP32 doesn't support 5GHz)

### MQTT Connection Issues
- Verify broker address and port
- Check firewall settings
- Try different public brokers if needed

### Servo Issues
- Ensure adequate power supply (servos need 5V, 1-2A)
- Check wiring connections
- Verify servo compatibility (standard PWM servos)

### Serial Monitor Output
Monitor the serial output for connection status and debugging information:
- Baud rate: 115200
- Look for "Robohand initialized successfully!" message
- Check WiFi and MQTT connection status

## Next Steps

- Try the [Mediapipe Integration Example](../mediapipe_demo/) for gesture control
- Explore [Advanced Configuration](../advanced_config/) for custom settings
- Build your own gesture recognition system
