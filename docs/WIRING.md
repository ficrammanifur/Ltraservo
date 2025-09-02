# Wiring Diagram and Hardware Setup

## Hardware Required
- ESP32 Dev Board (e.g., ESP32-WROOM-32).
- 5x Servo Motors (e.g., SG90).
- External 5V Power Supply for servos (do not power servos directly from ESP32 USB).
- MQTT Broker (e.g., broker.hivemq.com for testing or local Mosquitto).

## Wiring
- Servo 1 (Thumb): GPIO 18 (Signal), VCC to 5V, GND to GND.
- Servo 2 (Index): GPIO 19.
- Servo 3 (Middle): GPIO 21.
- Servo 4 (Ring): GPIO 22.
- Servo 5 (Pinky): GPIO 23.

Use a breadboard or PCB for connections. Example:
ESP32
GPIO18 -- Signal Servo1 (Thumb)
GPIO19 -- Signal Servo2 (Index)
GPIO21 -- Signal Servo3 (Middle)
GPIO22 -- Signal Servo4 (Ring)
GPIO23 -- Signal Servo5 (Pinky)
+5V    -- VCC All Servos (External Power)
GND    -- GND All Servos + ESP32 GND


**Note**: Ensure common ground between ESP32 and external power supply.

## Mediapipe Setup
On PC/Server:
- Install Python 3.7+ and dependencies: `pip install mediapipe opencv-python paho-mqtt numpy`.
- Run `mediapipe_hand_tracker.py` (provided).
- Ensure MQTT broker settings match the Arduino sketch.
- Position hand 30-60cm from webcam with good lighting.

For visual wiring diagrams, use tools like Fritzing or Tinkercad.