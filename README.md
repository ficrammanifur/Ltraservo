# Robohand Library

A simple Arduino library for ESP32 to control a robotic hand's servos via MQTT, bridging with Mediapipe gesture tracking.

## Features
- Control up to 5 servos (fingers) with custom pins and angle ranges.
- Connects to WiFi and MQTT broker.
- Supports single/multiple servo control, predefined gestures, and real-time finger tracking.
- Publishes servo status, connection status, and heartbeats.
- Compatible with Mediapipe hand tracking via provided Python script.

## Installation
1. Download this repository as a ZIP.
2. In Arduino IDE: Sketch > Include Library > Add .ZIP Library.
3. Install dependencies via Library Manager:
   - PubSubClient (>=2.8)
   - ESP32Servo (>=1.1.1)
   - ArduinoJson (>=6.21.0)

## Usage
See `examples/` folder:
- `basic_control/BasicServo.ino`: Basic MQTT servo control.
- `mediapipe_demo/mediapipe_demo.ino`: Real-time hand tracking with Mediapipe.
- `advanced_config/advanced_config.ino`: Custom pin and topic configuration.

## Mediapipe Integration
- Run `mediapipe_hand_tracker.py` on a PC with a webcam.
- Install Python dependencies: `pip install mediapipe opencv-python paho-mqtt numpy`.
- Configure MQTT settings to match the Arduino sketch.

## License
MIT License