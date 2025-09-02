#!/usr/bin/env python3
"""
Mediapipe Hand Tracker for Robohand

This script uses Google Mediapipe to detect hand landmarks from webcam feed
and sends real-time servo commands to the ESP32 via MQTT.

Requirements:
- Python 3.7+
- mediapipe
- opencv-python
- paho-mqtt
- numpy

Usage:
    python mediapipe_hand_tracker.py

Controls:
- 'q': Quit
- 'c': Toggle calibration mode
- 'g': Toggle gesture detection
- 's': Save current hand pose
- 'r': Reset to neutral position

Author: Robohand Library
Version: 1.0.0
"""

import cv2
import mediapipe as mp
import numpy as np
import json
import time
import math
from paho.mqtt import client as mqtt_client
import threading
from collections import deque

# Configuration
MQTT_BROKER = "localhost"  # Change to your MQTT broker
MQTT_PORT = 1883
MQTT_USER = ""  # Leave empty if no authentication
MQTT_PASSWORD = ""
DEVICE_ID = "robohand_mediapipe_001"
MQTT_TOPIC = f"robohand/{DEVICE_ID}/cmd/servo"

# Camera settings
CAMERA_INDEX = 0
FRAME_WIDTH = 640
FRAME_HEIGHT = 480
FPS = 30

# Hand tracking settings
MIN_DETECTION_CONFIDENCE = 0.7
MIN_TRACKING_CONFIDENCE = 0.5

# Finger bend calculation settings
SMOOTHING_WINDOW = 5  # Number of frames to average
GESTURE_THRESHOLD = 0.3  # Threshold for gesture detection
UPDATE_RATE = 30  # Hz - Rate to send MQTT updates

class HandTracker:
    def __init__(self):
        # Initialize Mediapipe
        self.mp_hands = mp.solutions.hands
        self.hands = self.mp_hands.Hands(
            static_image_mode=False,
            max_num_hands=1,
            min_detection_confidence=MIN_DETECTION_CONFIDENCE,
            min_tracking_confidence=MIN_TRACKING_CONFIDENCE
        )
        self.mp_draw = mp.solutions.drawing_utils
        
        # Initialize camera
        self.cap = cv2.VideoCapture(CAMERA_INDEX)
        self.cap.set(cv2.CAP_PROP_FRAME_WIDTH, FRAME_WIDTH)
        self.cap.set(cv2.CAP_PROP_FRAME_HEIGHT, FRAME_HEIGHT)
        self.cap.set(cv2.CAP_PROP_FPS, FPS)
        
        # MQTT client
        self.mqtt_client = None
        self.mqtt_connected = False
        
        # Tracking variables
        self.finger_positions = deque(maxlen=SMOOTHING_WINDOW)
        self.last_update_time = 0
        self.calibration_mode = False
        self.gesture_mode = True
        self.current_gesture = "unknown"
        
        # Finger landmarks indices (Mediapipe hand model)
        self.finger_tips = [4, 8, 12, 16, 20]  # Thumb, Index, Middle, Ring, Pinky
        self.finger_pips = [3, 6, 10, 14, 18]  # PIP joints
        self.finger_mcps = [2, 5, 9, 13, 17]   # MCP joints
        
        # Gesture templates
        self.gesture_templates = {
            "open": [0.0, 0.0, 0.0, 0.0, 0.0],
            "fist": [1.0, 1.0, 1.0, 1.0, 1.0],
            "peace": [1.0, 0.0, 0.0, 1.0, 1.0],
            "point": [1.0, 0.0, 1.0, 1.0, 1.0],
            "thumbs_up": [0.0, 1.0, 1.0, 1.0, 1.0]
        }
        
        print("Hand tracker initialized")
        print(f"Camera: {FRAME_WIDTH}x{FRAME_HEIGHT} @ {FPS}fps")
        
    def connect_mqtt(self):
        """Connect to MQTT broker"""
        try:
            self.mqtt_client = mqtt_client.Client()
            
            if MQTT_USER:
                self.mqtt_client.username_pw_set(MQTT_USER, MQTT_PASSWORD)
            
            def on_connect(client, userdata, flags, rc):
                if rc == 0:
                    self.mqtt_connected = True
                    print(f"Connected to MQTT broker: {MQTT_BROKER}:{MQTT_PORT}")
                else:
                    print(f"Failed to connect to MQTT broker, return code {rc}")
            
            def on_disconnect(client, userdata, rc):
                self.mqtt_connected = False
                print("Disconnected from MQTT broker")
            
            self.mqtt_client.on_connect = on_connect
            self.mqtt_client.on_disconnect = on_disconnect
            
            self.mqtt_client.connect(MQTT_BROKER, MQTT_PORT, 60)
            self.mqtt_client.loop_start()
            
        except Exception as e:
            print(f"MQTT connection error: {e}")
            self.mqtt_connected = False
    
    def calculate_finger_bend(self, landmarks, finger_idx):
        """Calculate finger bend ratio (0.0 = extended, 1.0 = fully bent)"""
        if finger_idx == 0:  # Thumb (different calculation)
            tip = landmarks[self.finger_tips[finger_idx]]
            pip = landmarks[self.finger_pips[finger_idx]]
            mcp = landmarks[self.finger_mcps[finger_idx]]
            
            # Calculate angle between tip-pip and pip-mcp vectors
            v1 = np.array([tip.x - pip.x, tip.y - pip.y])
            v2 = np.array([mcp.x - pip.x, mcp.y - pip.y])
            
            # Normalize vectors
            v1_norm = v1 / (np.linalg.norm(v1) + 1e-6)
            v2_norm = v2 / (np.linalg.norm(v2) + 1e-6)
            
            # Calculate angle
            dot_product = np.clip(np.dot(v1_norm, v2_norm), -1.0, 1.0)
            angle = math.acos(dot_product)
            
            # Convert to bend ratio (0-1)
            bend_ratio = 1.0 - (angle / math.pi)
            
        else:  # Other fingers
            tip = landmarks[self.finger_tips[finger_idx]]
            pip = landmarks[self.finger_pips[finger_idx]]
            mcp = landmarks[self.finger_mcps[finger_idx]]
            
            # Calculate distances
            tip_to_mcp = math.sqrt((tip.x - mcp.x)**2 + (tip.y - mcp.y)**2)
            pip_to_mcp = math.sqrt((pip.x - mcp.x)**2 + (pip.y - mcp.y)**2)
            
            # Bend ratio based on relative distances
            if pip_to_mcp > 0:
                bend_ratio = 1.0 - (tip_to_mcp / (pip_to_mcp * 2))
                bend_ratio = max(0.0, min(1.0, bend_ratio))
            else:
                bend_ratio = 0.0
        
        return bend_ratio
    
    def detect_gesture(self, finger_bends):
        """Detect predefined gestures from finger bend ratios"""
        best_match = "unknown"
        min_distance = float('inf')
        
        for gesture_name, template in self.gesture_templates.items():
            # Calculate Euclidean distance
            distance = np.linalg.norm(np.array(finger_bends) - np.array(template))
            
            if distance < min_distance and distance < GESTURE_THRESHOLD:
                min_distance = distance
                best_match = gesture_name
        
        return best_match
    
    def send_mqtt_command(self, command):
        """Send command to ESP32 via MQTT"""
        if not self.mqtt_connected or not self.mqtt_client:
            return False
        
        try:
            message = json.dumps(command)
            result = self.mqtt_client.publish(MQTT_TOPIC, message)
            return result.rc == mqtt_client.MQTT_ERR_SUCCESS
        except Exception as e:
            print(f"MQTT send error: {e}")
            return False
    
    def process_frame(self, frame):
        """Process single frame for hand detection"""
        # Convert BGR to RGB
        rgb_frame = cv2.cvtColor(frame, cv2.COLOR_BGR2RGB)
        results = self.hands.process(rgb_frame)
        
        # Draw hand landmarks
        if results.multi_hand_landmarks:
            for hand_landmarks in results.multi_hand_landmarks:
                # Draw landmarks
                self.mp_draw.draw_landmarks(
                    frame, hand_landmarks, self.mp_hands.HAND_CONNECTIONS)
                
                # Calculate finger bends
                finger_bends = []
                for i in range(5):
                    bend = self.calculate_finger_bend(hand_landmarks.landmark, i)
                    finger_bends.append(bend)
                
                # Smooth the values
                self.finger_positions.append(finger_bends)
                if len(self.finger_positions) >= SMOOTHING_WINDOW:
                    smoothed_bends = np.mean(self.finger_positions, axis=0)
                    
                    # Send MQTT updates at specified rate
                    current_time = time.time()
                    if current_time - self.last_update_time > (1.0 / UPDATE_RATE):
                        if self.gesture_mode:
                            # Detect and send gesture
                            gesture = self.detect_gesture(smoothed_bends)
                            if gesture != self.current_gesture:
                                self.current_gesture = gesture
                                self.send_mqtt_command({"gesture": gesture})
                                print(f"Gesture: {gesture}")
                        else:
                            # Send raw finger positions
                            self.send_mqtt_command({"fingers": smoothed_bends.tolist()})
                        
                        self.last_update_time = current_time
                
                # Draw finger bend values on frame
                y_offset = 30
                for i, bend in enumerate(finger_bends):
                    finger_names = ["Thumb", "Index", "Middle", "Ring", "Pinky"]
                    text = f"{finger_names[i]}: {bend:.2f}"
                    cv2.putText(frame, text, (10, y_offset), 
                               cv2.FONT_HERSHEY_SIMPLEX, 0.6, (0, 255, 0), 2)
                    y_offset += 25
                
                # Draw current mode and gesture
                mode_text = f"Mode: {'Gesture' if self.gesture_mode else 'Real-time'}"
                cv2.putText(frame, mode_text, (10, frame.shape[0] - 60),
                           cv2.FONT_HERSHEY_SIMPLEX, 0.7, (255, 255, 0), 2)
                
                if self.gesture_mode:
                    gesture_text = f"Gesture: {self.current_gesture}"
                    cv2.putText(frame, gesture_text, (10, frame.shape[0] - 30),
                               cv2.FONT_HERSHEY_SIMPLEX, 0.7, (255, 255, 0), 2)
        
        # Draw connection status
        mqtt_status = "MQTT: Connected" if self.mqtt_connected else "MQTT: Disconnected"
        color = (0, 255, 0) if self.mqtt_connected else (0, 0, 255)
        cv2.putText(frame, mqtt_status, (frame.shape[1] - 200, 30),
                   cv2.FONT_HERSHEY_SIMPLEX, 0.6, color, 2)
        
        return frame
    
    def run(self):
        """Main tracking loop"""
        print("Starting hand tracking...")
        print("Controls:")
        print("  'q': Quit")
        print("  'g': Toggle gesture/real-time mode")
        print("  'r': Reset to neutral position")
        print("  'c': Calibration info")
        
        # Connect to MQTT
        self.connect_mqtt()
        
        while True:
            ret, frame = self.cap.read()
            if not ret:
                print("Failed to read from camera")
                break
            
            # Flip frame horizontally for mirror effect
            frame = cv2.flip(frame, 1)
            
            # Process frame
            frame = self.process_frame(frame)
            
            # Display frame
            cv2.imshow('Robohand - Mediapipe Hand Tracking', frame)
            
            # Handle keyboard input
            key = cv2.waitKey(1) & 0xFF
            if key == ord('q'):
                break
            elif key == ord('g'):
                self.gesture_mode = not self.gesture_mode
                mode = "Gesture Detection" if self.gesture_mode else "Real-time Control"
                print(f"Switched to {mode} mode")
            elif key == ord('r'):
                # Reset to neutral position
                neutral_cmd = {"fingers": [0.5, 0.5, 0.5, 0.5, 0.5]}
                self.send_mqtt_command(neutral_cmd)
                print("Reset to neutral position")
            elif key == ord('c'):
                print("\nCalibration Tips:")
                print("- Ensure good lighting")
                print("- Keep hand 30-60cm from camera")
                print("- Make clear, distinct gestures")
                print("- Adjust servo calibration in Arduino code if needed")
        
        # Cleanup
        self.cap.release()
        cv2.destroyAllWindows()
        if self.mqtt_client:
            self.mqtt_client.loop_stop()
            self.mqtt_client.disconnect()
        print("Hand tracking stopped")

if __name__ == "__main__":
    try:
        tracker = HandTracker()
        tracker.run()
    except KeyboardInterrupt:
        print("\nStopping hand tracker...")
    except Exception as e:
        print(f"Error: {e}")
