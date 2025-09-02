/*
  Robohand Basic Control Example
  
  This example demonstrates basic servo control via MQTT.
  The ESP32 connects to WiFi and MQTT broker, then listens for servo commands.
  
  Hardware Setup:
  - Connect servos to pins 18, 19, 21, 22, 23 (or modify as needed)
  - Ensure proper power supply for servos (external 5V recommended)
  
  MQTT Topics:
  - robohand/[device_id]/cmd/servo - Send servo commands
  - robohand/[device_id]/servo_status - Receive servo status
  - robohand/[device_id]/status - Device status
  
  Author: Robohand Library
  Version: 1.0.0
*/

#include <Robohand.h>

// WiFi credentials
const char* WIFI_SSID = "YOUR_WIFI_SSID";
const char* WIFI_PASSWORD = "YOUR_WIFI_PASSWORD";

// MQTT broker settings
const char* MQTT_SERVER = "broker.hivemq.com";  // Free public broker for testing
const int MQTT_PORT = 1883;

// Device configuration
const char* DEVICE_ID = "robohand_basic_001";

// Create Robohand instance
Robohand robohand(DEVICE_ID);

void setup() {
  Serial.begin(115200);
  Serial.println("=== Robohand Basic Control Example ===");
  
  // Configure WiFi
  robohand.setWiFiCredentials(WIFI_SSID, WIFI_PASSWORD);
  
  // Configure MQTT
  robohand.setMQTTServer(MQTT_SERVER, MQTT_PORT);
  
  // Add servos (pin, name, min_angle, max_angle)
  robohand.addServo(18, "thumb", 0, 180);
  robohand.addServo(19, "index", 0, 180);
  robohand.addServo(21, "middle", 0, 180);
  robohand.addServo(22, "ring", 0, 180);
  robohand.addServo(23, "pinky", 0, 180);
  
  // Set heartbeat interval (30 seconds)
  robohand.setHeartbeatInterval(30000);
  
  // Initialize the system
  if (robohand.begin()) {
    Serial.println("Robohand initialized successfully!");
    Serial.println("Device ID: " + String(DEVICE_ID));
    Serial.println("Servo count: " + String(robohand.getServoCount()));
    
    // Demonstrate initial servo positions
    demonstrateServos();
  } else {
    Serial.println("Failed to initialize Robohand!");
    Serial.println("Check WiFi and MQTT settings");
  }
  
  printMQTTInstructions();
}

void loop() {
  // Main Robohand loop - handles WiFi, MQTT, and servo updates
  robohand.loop();
  
  // Check connection status every 10 seconds
  static unsigned long lastStatusCheck = 0;
  if (millis() - lastStatusCheck > 10000) {
    printConnectionStatus();
    lastStatusCheck = millis();
  }
  
  // Small delay to prevent watchdog issues
  delay(10);
}

void demonstrateServos() {
  Serial.println("\n=== Demonstrating Servo Control ===");
  
  // Move all servos to center position
  Serial.println("Moving all servos to center (90°)...");
  robohand.setServoAngle("thumb", 90);
  robohand.setServoAngle("index", 90);
  robohand.setServoAngle("middle", 90);
  robohand.setServoAngle("ring", 90);
  robohand.setServoAngle("pinky", 90);
  
  delay(2000);
  
  // Open hand (0 degrees)
  Serial.println("Opening hand (0°)...");
  for (int i = 0; i < robohand.getServoCount(); i++) {
    robohand.setServoAngle(i, 0);
    delay(200);
  }
  
  delay(1000);
  
  // Close hand (180 degrees)
  Serial.println("Closing hand (180°)...");
  for (int i = 0; i < robohand.getServoCount(); i++) {
    robohand.setServoAngle(i, 180);
    delay(200);
  }
  
  delay(1000);
  
  // Return to center
  Serial.println("Returning to center position...");
  for (int i = 0; i < robohand.getServoCount(); i++) {
    robohand.setServoAngle(i, 90);
    delay(200);
  }
  
  Serial.println("Servo demonstration complete!\n");
}

void printConnectionStatus() {
  Serial.println("\n=== Connection Status ===");
  Serial.println("WiFi: " + String(robohand.isWiFiConnected() ? "Connected" : "Disconnected"));
  Serial.println("MQTT: " + String(robohand.isMQTTConnected() ? "Connected" : "Disconnected"));
  Serial.println("Overall: " + String(robohand.isConnected() ? "Ready" : "Not Ready"));
  
  if (robohand.isWiFiConnected()) {
    Serial.println("IP Address: " + WiFi.localIP().toString());
    Serial.println("WiFi RSSI: " + String(WiFi.RSSI()) + " dBm");
  }
  
  Serial.println("Free Heap: " + String(ESP.getFreeHeap()) + " bytes");
  Serial.println("Uptime: " + String(millis() / 1000) + " seconds");
}

void printMQTTInstructions() {
  Serial.println("\n=== MQTT Control Instructions ===");
  Serial.println("Use an MQTT client to send commands to:");
  Serial.println("Topic: robohand/" + String(DEVICE_ID) + "/cmd/servo");
  Serial.println();
  Serial.println("Example commands:");
  Serial.println();
  Serial.println("1. Control single servo:");
  Serial.println("   {\"servo\": \"thumb\", \"angle\": 45}");
  Serial.println();
  Serial.println("2. Control multiple servos:");
  Serial.println("   {\"servos\": [");
  Serial.println("     {\"name\": \"thumb\", \"angle\": 0},");
  Serial.println("     {\"name\": \"index\", \"angle\": 90},");
  Serial.println("     {\"name\": \"middle\", \"angle\": 180}");
  Serial.println("   ]}");
  Serial.println();
  Serial.println("3. Hand gestures:");
  Serial.println("   Open hand: {\"servos\": [{\"name\": \"thumb\", \"angle\": 0}, {\"name\": \"index\", \"angle\": 0}, {\"name\": \"middle\", \"angle\": 0}, {\"name\": \"ring\", \"angle\": 0}, {\"name\": \"pinky\", \"angle\": 0}]}");
  Serial.println("   Fist: {\"servos\": [{\"name\": \"thumb\", \"angle\": 180}, {\"name\": \"index\", \"angle\": 180}, {\"name\": \"middle\", \"angle\": 180}, {\"name\": \"ring\", \"angle\": 180}, {\"name\": \"pinky\", \"angle\": 180}]}");
  Serial.println("   Peace sign: {\"servos\": [{\"name\": \"thumb\", \"angle\": 180}, {\"name\": \"index\", \"angle\": 0}, {\"name\": \"middle\", \"angle\": 0}, {\"name\": \"ring\", \"angle\": 180}, {\"name\": \"pinky\", \"angle\": 180}]}");
  Serial.println();
  Serial.println("Status topics (subscribe to monitor):");
  Serial.println("- robohand/" + String(DEVICE_ID) + "/status");
  Serial.println("- robohand/" + String(DEVICE_ID) + "/servo_status");
  Serial.println("- robohand/" + String(DEVICE_ID) + "/heartbeat");
  Serial.println();
  Serial.println("Recommended MQTT clients:");
  Serial.println("- MQTT Explorer (Desktop)");
  Serial.println("- MyMQTT (Mobile)");
  Serial.println("- mosquitto_pub/sub (Command line)");
  Serial.println("================================");
}
