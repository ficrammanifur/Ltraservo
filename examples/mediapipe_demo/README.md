# Mediapipe Integration Example

This example demonstrates real-time hand gesture control using Google Mediapipe for computer vision and MQTT for communication between Python and ESP32.

## Overview

The system consists of two parts:
1. **Python Script**: Captures webcam feed, detects hand landmarks using Mediapipe, and sends servo commands via MQTT
2. **ESP32 Code**: Receives MQTT commands and controls servos to mirror detected hand movements

## Features

- Real-time hand tracking and gesture recognition
- Smooth servo control with position averaging
- Predefined gesture detection (open, fist, peace, point, thumbs up)
- Real-time finger position mirroring
- Automatic calibration and servo testing
- MQTT communication with reconnection handling
- Visual feedback with OpenCV display

## Requirements

### Hardware
- ESP32 development board
- 5x servo motors (SG90 or similar)
- External 5V power supply for servos
- Webcam or built-in camera
- Computer running Python 3.7+

### Software Dependencies

Install Python packages:
\`\`\`bash
pip install mediapipe opencv-python paho-mqtt numpy
\`\`\`

### MQTT Broker
You need an MQTT broker running. Options:
- **Local**: Install Mosquitto locally
- **Cloud**: Use HiveMQ, AWS IoT, or similar
- **Docker**: \`docker run -it -p 1883:1883 eclipse-mosquitto\`

## Setup Instructions

### 1. Configure ESP32

Update the Arduino sketch with your settings:

\`\`\`cpp
// WiFi credentials
const char* WIFI_SSID = "YOUR_WIFI_SSID";
const char* WIFI_PASSWORD = "YOUR_WIFI_PASSWORD";

// MQTT broker settings
const char* MQTT_SERVER = "localhost";  // Your MQTT broker IP
const int MQTT_PORT = 1883;
\`\`\`

### 2. Configure Python Script

Update the Python script settings:

\`\`\`python
# Configuration
MQTT_BROKER = "localhost"  # Same as ESP32 setting
MQTT_PORT = 1883
DEVICE_ID = "robohand_mediapipe_001"  # Match ESP32 device ID
\`\`\`

### 3. Servo Calibration

Adjust servo calibration in the Arduino code based on your hardware:

\`\`\`cpp
ServoCalibration fingerCalibration[5] = {
  {10, 170, false},   // Thumb: open_angle, closed_angle, inverted
  {0, 180, false},    // Index
  {0, 180, false},    // Middle  
  {0, 180, false},    // Ring
  {0, 180, false}     // Pinky
};
\`\`\`

## Usage

### 1. Start MQTT Broker
\`\`\`bash
# If using Mosquitto locally
mosquitto -v
\`\`\`

### 2. Upload and Run ESP32 Code
- Upload the Arduino sketch to your ESP32
- Open Serial Monitor (115200 baud) to see status
- Wait for "Ready for Mediapipe hand tracking..." message

### 3. Run Python Hand Tracker
\`\`\`bash
cd examples/mediapipe_demo
python mediapipe_hand_tracker.py
\`\`\`

### 4. Control the Robotic Hand
- Position your hand in front of the camera
- The robotic hand will mirror your movements
- Use keyboard controls:
  - **'q'**: Quit application
  - **'g'**: Toggle between gesture detection and real-time control
  - **'r'**: Reset to neutral position
  - **'c'**: Show calibration tips

## Control Modes

### Gesture Detection Mode
Recognizes predefined gestures and executes them:
- **Open Hand**: All fingers extended
- **Fist**: All fingers closed
- **Peace Sign**: Index and middle fingers extended
- **Point**: Only index finger extended
- **Thumbs Up**: Only thumb extended

### Real-time Control Mode
Continuously mirrors your hand movements with smooth servo control.

## MQTT Protocol

### Topics Used
- **Command**: \`robohand/[device_id]/cmd/servo\`
- **Status**: \`robohand/[device_id]/servo_status\`
- **Heartbeat**: \`robohand/[device_id]/heartbeat\`

### Message Formats

**Gesture Command:**
\`\`\`json
{
  "gesture": "open"
}
\`\`\`

**Real-time Finger Control:**
\`\`\`json
{
  "fingers": [0.0, 0.5, 1.0, 0.8, 0.2]
}
\`\`\`
Values range from 0.0 (extended) to 1.0 (fully bent) for each finger.

## Troubleshooting

### Camera Issues
- Ensure camera is not being used by other applications
- Try different camera indices (0, 1, 2) in the Python script
- Check camera permissions on your system

### Hand Detection Issues
- Ensure good lighting conditions
- Keep hand 30-60cm from camera
- Make sure entire hand is visible in frame
- Avoid background clutter

### MQTT Connection Issues
- Verify MQTT broker is running and accessible
- Check firewall settings
- Ensure ESP32 and computer are on same network
- Test MQTT connection with tools like MQTT Explorer

### Servo Control Issues
- Check servo calibration values
- Ensure adequate power supply for servos
- Verify wiring connections
- Monitor Serial output for error messages

### Performance Issues
- Reduce UPDATE_RATE in Python script
- Increase SMOOTHING_WINDOW for smoother movement
- Close other applications using camera/CPU

## Customization

### Adding New Gestures
Add gesture templates to the Python script:

\`\`\`python
self.gesture_templates = {
    "custom_gesture": [0.2, 0.8, 0.0, 1.0, 0.5],  # Custom finger positions
    # ... existing gestures
}
\`\`\`

### Adjusting Sensitivity
Modify detection parameters:

\`\`\`python
GESTURE_THRESHOLD = 0.3  # Lower = more sensitive
SMOOTHING_WINDOW = 5     # Higher = smoother but slower
UPDATE_RATE = 30         # Higher = more responsive
\`\`\`

### Custom Servo Mapping
Modify the finger bend calculation or servo calibration to match your specific hardware setup.

## Next Steps

- Experiment with different gesture recognition algorithms
- Add voice commands for mode switching
- Implement gesture recording and playback
- Create custom applications using the MQTT interface
- Integrate with other computer vision libraries
