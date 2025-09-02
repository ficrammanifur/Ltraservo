/*
  Robohand Mediapipe Integration Example
  
  This example demonstrates real-time hand gesture control using Google Mediapipe.
  A Python script captures hand landmarks from webcam and sends servo commands via MQTT.
  
  Hardware Setup:
  - Connect servos to pins 18, 19, 21, 22, 23
  - Ensure proper power supply for servos (external 5V recommended)
  
  Software Requirements:
  - Python 3.7+ with mediapipe, opencv-python, paho-mqtt
  - Webcam or camera device
  - MQTT broker (local or cloud)
  
  How it works:
  1. Python script detects hand landmarks using Mediapipe
  2. Calculates finger bend angles from landmarks
  3. Maps angles to servo positions (0-180 degrees)
  4. Sends MQTT commands to ESP32
  5. ESP32 moves servos to match detected hand pose
  
  Author: Robohand Library
  Version: 1.0.0
*/

#include <Robohand.h>

// WiFi credentials
const char* WIFI_SSID = "YOUR_WIFI_SSID";
const char* WIFI_PASSWORD = "YOUR_WIFI_PASSWORD";

// MQTT broker settings
const char* MQTT_SERVER = "localhost";  // Change to your MQTT broker
const int MQTT_PORT = 1883;
const char* MQTT_USER = "";  // Leave empty if no authentication
const char* MQTT_PASSWORD = "";

// Device configuration
const char* DEVICE_ID = "robohand_mediapipe_001";

// Servo calibration settings
struct ServoCalibration {
  int openAngle;    // Angle when finger is fully extended
  int closedAngle;  // Angle when finger is fully bent
  bool inverted;    // True if servo rotation is inverted
};

// Calibration for each finger (adjust based on your servo setup)
ServoCalibration fingerCalibration[5] = {
  {10, 170, false},   // Thumb
  {0, 180, false},    // Index
  {0, 180, false},    // Middle  
  {0, 180, false},    // Ring
  {0, 180, false}     // Pinky
};

// Create Robohand instance
Robohand robohand(DEVICE_ID);

// Gesture detection variables
unsigned long lastGestureTime = 0;
String currentGesture = "unknown";
bool gestureMode = true;  // Enable automatic gesture detection

void setup() {
  Serial.begin(115200);
  Serial.println("=== Robohand Mediapipe Integration ===");
  
  // Configure WiFi
  robohand.setWiFiCredentials(WIFI_SSID, WIFI_PASSWORD);
  
  // Configure MQTT
  robohand.setMQTTServer(MQTT_SERVER, MQTT_PORT);
  if (strlen(MQTT_USER) > 0) {
    robohand.setMQTTCredentials(MQTT_USER, MQTT_PASSWORD);
  }
  
  // Add servos with calibrated ranges
  robohand.addServo(18, "thumb", fingerCalibration[0].openAngle, fingerCalibration[0].closedAngle);
  robohand.addServo(19, "index", fingerCalibration[1].openAngle, fingerCalibration[1].closedAngle);
  robohand.addServo(21, "middle", fingerCalibration[2].openAngle, fingerCalibration[2].closedAngle);
  robohand.addServo(22, "ring", fingerCalibration[3].openAngle, fingerCalibration[3].closedAngle);
  robohand.addServo(23, "pinky", fingerCalibration[4].openAngle, fingerCalibration[4].closedAngle);
  
  // Set faster heartbeat for real-time control
  robohand.setHeartbeatInterval(5000);  // 5 seconds
  
  // Initialize the system
  if (robohand.begin()) {
    Serial.println("Robohand initialized successfully!");
    Serial.println("Ready for Mediapipe hand tracking...");
    
    // Calibration sequence
    performCalibrationSequence();
  } else {
    Serial.println("Failed to initialize Robohand!");
    Serial.println("Check WiFi and MQTT settings");
  }
  
  printMediapipeInstructions();
}

void loop() {
  // Main Robohand loop
  robohand.loop();
  
  // Monitor gesture detection
  if (gestureMode && millis() - lastGestureTime > 1000) {
    // Check if we're receiving gesture data
    if (millis() - lastGestureTime > 10000) {
      Serial.println("No gesture data received. Check Mediapipe script.");
    }
  }
  
  // Status update every 30 seconds
  static unsigned long lastStatusUpdate = 0;
  if (millis() - lastStatusUpdate > 30000) {
    printSystemStatus();
    lastStatusUpdate = millis();
  }
  
  delay(10);
}

void performCalibrationSequence() {
  Serial.println("\n=== Servo Calibration Sequence ===");
  
  // Test each servo individually
  String fingerNames[] = {"thumb", "index", "middle", "ring", "pinky"};
  
  for (int i = 0; i < 5; i++) {
    Serial.println("Testing " + fingerNames[i] + "...");
    
    // Move to open position
    Serial.println("  -> Open position");
    robohand.setServoAngle(fingerNames[i], fingerCalibration[i].openAngle);
    delay(1000);
    
    // Move to closed position
    Serial.println("  -> Closed position");
    robohand.setServoAngle(fingerNames[i], fingerCalibration[i].closedAngle);
    delay(1000);
    
    // Return to neutral (middle) position
    int neutralAngle = (fingerCalibration[i].openAngle + fingerCalibration[i].closedAngle) / 2;
    Serial.println("  -> Neutral position");
    robohand.setServoAngle(fingerNames[i], neutralAngle);
    delay(500);
  }
  
  Serial.println("Calibration complete!\n");
}

void printSystemStatus() {
  Serial.println("\n=== System Status ===");
  Serial.println("WiFi: " + String(robohand.isWiFiConnected() ? "Connected" : "Disconnected"));
  Serial.println("MQTT: " + String(robohand.isMQTTConnected() ? "Connected" : "Disconnected"));
  Serial.println("Current Gesture: " + currentGesture);
  Serial.println("Gesture Mode: " + String(gestureMode ? "Enabled" : "Disabled"));
  Serial.println("Free Heap: " + String(ESP.getFreeHeap()) + " bytes");
  
  // Print current servo positions
  Serial.println("\nServo Positions:");
  String fingerNames[] = {"thumb", "index", "middle", "ring", "pinky"};
  for (int i = 0; i < 5; i++) {
    int angle = robohand.getServoAngle(fingerNames[i]);
    Serial.println("  " + fingerNames[i] + ": " + String(angle) + "°");
  }
  Serial.println();
}

void printMediapipeInstructions() {
  Serial.println("\n=== Mediapipe Setup Instructions ===");
  Serial.println();
  Serial.println("1. Install Python dependencies:");
  Serial.println("   pip install mediapipe opencv-python paho-mqtt numpy");
  Serial.println();
  Serial.println("2. Run the Python hand tracking script:");
  Serial.println("   python mediapipe_hand_tracker.py");
  Serial.println();
  Serial.println("3. Position your hand in front of the camera");
  Serial.println("   - Keep hand clearly visible");
  Serial.println("   - Good lighting recommended");
  Serial.println("   - Distance: 30-60cm from camera");
  Serial.println();
  Serial.println("4. The robotic hand will mirror your movements!");
  Serial.println();
  Serial.println("MQTT Topics for manual control:");
  Serial.println("- Publish to: robohand/" + String(DEVICE_ID) + "/cmd/servo");
  Serial.println("- Subscribe to: robohand/" + String(DEVICE_ID) + "/servo_status");
  Serial.println();
  Serial.println("Gesture Commands (via MQTT):");
  Serial.println("- {\"gesture\": \"open\"} - Open hand");
  Serial.println("- {\"gesture\": \"fist\"} - Close fist");
  Serial.println("- {\"gesture\": \"peace\"} - Peace sign");
  Serial.println("- {\"gesture\": \"point\"} - Point gesture");
  Serial.println("- {\"gesture\": \"thumbs_up\"} - Thumbs up");
  Serial.println();
  Serial.println("Real-time finger control:");
  Serial.println("- {\"fingers\": [0.0, 0.5, 1.0, 0.8, 0.2]}");
  Serial.println("  (Values 0.0-1.0 for thumb, index, middle, ring, pinky)");
  Serial.println("=====================================");
}

// Custom MQTT message handler for gesture commands
void handleGestureCommand(String gesture) {
  currentGesture = gesture;
  lastGestureTime = millis();
  
  Serial.println("Gesture detected: " + gesture);
  
  if (gesture == "open") {
    // Open hand - all fingers extended
    for (int i = 0; i < 5; i++) {
      robohand.setServoAngle(i, fingerCalibration[i].openAngle);
    }
  }
  else if (gesture == "fist") {
    // Closed fist - all fingers bent
    for (int i = 0; i < 5; i++) {
      robohand.setServoAngle(i, fingerCalibration[i].closedAngle);
    }
  }
  else if (gesture == "peace") {
    // Peace sign - index and middle extended, others closed
    robohand.setServoAngle("thumb", fingerCalibration[0].closedAngle);
    robohand.setServoAngle("index", fingerCalibration[1].openAngle);
    robohand.setServoAngle("middle", fingerCalibration[2].openAngle);
    robohand.setServoAngle("ring", fingerCalibration[3].closedAngle);
    robohand.setServoAngle("pinky", fingerCalibration[4].closedAngle);
  }
  else if (gesture == "point") {
    // Pointing - only index extended
    robohand.setServoAngle("thumb", fingerCalibration[0].closedAngle);
    robohand.setServoAngle("index", fingerCalibration[1].openAngle);
    robohand.setServoAngle("middle", fingerCalibration[2].closedAngle);
    robohand.setServoAngle("ring", fingerCalibration[3].closedAngle);
    robohand.setServoAngle("pinky", fingerCalibration[4].closedAngle);
  }
  else if (gesture == "thumbs_up") {
    // Thumbs up - only thumb extended
    robohand.setServoAngle("thumb", fingerCalibration[0].openAngle);
    robohand.setServoAngle("index", fingerCalibration[1].closedAngle);
    robohand.setServoAngle("middle", fingerCalibration[2].closedAngle);
    robohand.setServoAngle("ring", fingerCalibration[3].closedAngle);
    robohand.setServoAngle("pinky", fingerCalibration[4].closedAngle);
  }
}

// Handle real-time finger position data from Mediapipe
void handleFingerPositions(float positions[5]) {
  lastGestureTime = millis();
  currentGesture = "real-time";
  
  String fingerNames[] = {"thumb", "index", "middle", "ring", "pinky"};
  
  for (int i = 0; i < 5; i++) {
    // Map 0.0-1.0 range to servo angles
    float bendRatio = constrain(positions[i], 0.0, 1.0);
    
    int targetAngle;
    if (fingerCalibration[i].inverted) {
      targetAngle = fingerCalibration[i].openAngle + 
                   (bendRatio * (fingerCalibration[i].closedAngle - fingerCalibration[i].openAngle));
    } else {
      targetAngle = fingerCalibration[i].openAngle + 
                   (bendRatio * (fingerCalibration[i].closedAngle - fingerCalibration[i].openAngle));
    }
    
    robohand.setServoAngle(fingerNames[i], targetAngle);
  }
}