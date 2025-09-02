# 🤖 Robohand – MQTT Servo Driver for ESP32

[![Arduino](https://img.shields.io/badge/Arduino-Compatible-blue.svg)](https://www.arduino.cc/)
[![ESP32](https://img.shields.io/badge/ESP32-Supported-green.svg)](https://www.espressif.com/en/products/socs/esp32)
[![License](https://img.shields.io/badge/License-MIT-yellow.svg)](LICENSE)

Robohand adalah library sederhana untuk ESP32 yang menghubungkan servo dengan MQTT untuk kontrol robotic hand. Library ini dirancang sebagai jembatan universal antara gesture tracking (Mediapipe di PC/Server) dan robotic hand berbasis ESP32.

Dengan Robohand, ESP32 cukup subscribe ke topic MQTT dan otomatis menerjemahkan data tracking menjadi gerakan servo yang presisi.

## ✨ Fitur Utama

- 🔌 **Integrasi MQTT** - Subscribe & publish status servo secara real-time
- 🦾 **Kontrol Multi-Servo** - Dukungan hingga 5 jari (atau lebih sesuai kebutuhan)
- 🖥️ **Kompatibel Mediapipe** - Data tracking jari dikirim langsung dari PC/Server ke ESP32
- ⚡ **Mudah Digunakan** - Hanya butuh beberapa baris kode untuk kontrol robotic hand
- 🔧 **Konfigurasi Fleksibel** - Pin servo dan parameter MQTT dapat disesuaikan
- 📊 **Status Monitoring** - Feedback real-time status servo

## 📂 Struktur Proyek

```
Robohand/
├── src/
│   ├── Robohand.h          # Header library utama
│   └── Robohand.cpp        # Implementasi fungsi library
├── examples/
│   ├── basic_control/      # Contoh kontrol servo dari MQTT
│   ├── mediapipe_demo/     # Contoh integrasi dengan Mediapipe
│   └── advanced_config/    # Konfigurasi lanjutan
├── docs/
│   ├── API.md             # Dokumentasi API lengkap
│   └── WIRING.md          # Diagram wiring dan setup hardware
├── README.md
├── library.properties
└── LICENSE
```

## 🚀 Instalasi

### Metode 1: Arduino Library Manager
1. Buka Arduino IDE
2. Pergi ke **Sketch** → **Include Library** → **Manage Libraries**
3. Cari "Robohand" dan klik **Install**

### Metode 2: Manual Installation
```bash
# Clone repository ke folder Arduino libraries
git clone https://github.com/ficrammanifur/Robohand.git

# Atau download dan extract ke folder:
# Documents/Arduino/libraries/Robohand/
```

### Metode 3: PlatformIO
```ini
# Tambahkan ke platformio.ini
lib_deps = 
    ficrammanifur/Robohand
```

## 🛠️ Dependencies

Pastikan library berikut sudah terinstall:

- **ESP32 Arduino Core** (v2.0.0+)
- **PubSubClient** (v2.8.0+) - untuk komunikasi MQTT
- **ESP32Servo** (v0.11.0+) - untuk kontrol servo

```bash
# Install via Arduino Library Manager atau PlatformIO
```

## ⚡ Quick Start

### Contoh Dasar

```cpp
#include <Robohand.h>

// Inisialisasi dengan MQTT broker dan client ID
Robohand robo("broker.hivemq.com", 1883, "esp32-robohand");

void setup() {
  Serial.begin(115200);
  
  // Mulai koneksi WiFi dan MQTT
  robo.begin("WIFI_SSID", "WIFI_PASSWORD");
  
  // Attach servo ke pin GPIO
  robo.attachServo(0, 5);   // Servo jari 0 (jempol) di pin GPIO5
  robo.attachServo(1, 18);  // Servo jari 1 (telunjuk) di pin GPIO18
  robo.attachServo(2, 19);  // Servo jari 2 (tengah) di pin GPIO19
  robo.attachServo(3, 21);  // Servo jari 3 (manis) di pin GPIO21
  robo.attachServo(4, 22);  // Servo jari 4 (kelingking) di pin GPIO22
  
  Serial.println("Robohand siap!");
}

void loop() {
  robo.loop();  
  // Servo otomatis bergerak sesuai data dari MQTT topic: robohand/servo/[id]
}
```

### Contoh dengan Konfigurasi Lanjutan

```cpp
#include <Robohand.h>

Robohand robo;

void setup() {
  Serial.begin(115200);
  
  // Konfigurasi custom
  robo.setMQTTBroker("your-broker.com", 1883);
  robo.setClientID("my-robohand-01");
  robo.setTopicPrefix("myrobot/hand");  // Custom topic prefix
  
  // Mulai koneksi
  robo.begin("WIFI_SSID", "WIFI_PASSWORD");
  
  // Setup servo dengan kalibrasi
  robo.attachServo(0, 5, 0, 180);    // Pin, min_angle, max_angle
  robo.attachServo(1, 18, 10, 170);  // Dengan batas sudut custom
  
  // Enable status publishing
  robo.enableStatusPublish(true);
}

void loop() {
  robo.loop();
  
  // Optional: manual control
  if (Serial.available()) {
    int servoId = Serial.parseInt();
    int angle = Serial.parseInt();
    robo.moveServo(servoId, angle);
  }
}
```

## 📡 Protokol MQTT

### Subscribe Topics (ESP32 menerima)
```
robohand/servo/0    # Kontrol servo jari 0 (0-180)
robohand/servo/1    # Kontrol servo jari 1 (0-180)
robohand/servo/2    # Kontrol servo jari 2 (0-180)
robohand/servo/3    # Kontrol servo jari 3 (0-180)
robohand/servo/4    # Kontrol servo jari 4 (0-180)
robohand/command    # Perintah khusus (reset, calibrate, etc.)
```

### Publish Topics (ESP32 mengirim)
```
robohand/status/0   # Status servo 0 (current angle)
robohand/status/1   # Status servo 1 (current angle)
robohand/online     # Status koneksi ESP32
robohand/battery    # Level baterai (jika ada sensor)
```

### Format Data
```json
// Contoh payload untuk kontrol servo
{
  "angle": 90,
  "speed": 100,
  "timestamp": 1640995200
}

// Atau format sederhana (hanya angka)
90
```

## 🖥️ Integrasi dengan Mediapipe

### Python Script (Pengirim)

```python
import cv2
import mediapipe as mp
import paho.mqtt.client as mqtt
import json

# Setup MQTT
client = mqtt.Client()
client.connect("broker.hivemq.com", 1883, 60)

# Setup Mediapipe
mp_hands = mp.solutions.hands
hands = mp_hands.Hands()

cap = cv2.VideoCapture(0)

while True:
    ret, frame = cap.read()
    if not ret:
        break
    
    # Process hand landmarks
    results = hands.process(cv2.cvtColor(frame, cv2.COLOR_BGR2RGB))
    
    if results.multi_hand_landmarks:
        for hand_landmarks in results.multi_hand_landmarks:
            # Kalkulasi sudut jari berdasarkan landmarks
            finger_angles = calculate_finger_angles(hand_landmarks)
            
            # Kirim ke ESP32 via MQTT
            for i, angle in enumerate(finger_angles):
                topic = f"robohand/servo/{i}"
                client.publish(topic, str(int(angle)))
    
    cv2.imshow('Hand Tracking', frame)
    if cv2.waitKey(1) & 0xFF == ord('q'):
        break

cap.release()
cv2.destroyAllWindows()
```

## 🔧 API Reference

### Konstruktor
```cpp
Robohand(const char* broker, int port, const char* clientId);
Robohand(); // Default constructor
```

### Metode Utama
```cpp
// Inisialisasi dan koneksi
bool begin(const char* ssid, const char* password);
void loop();

// Konfigurasi servo
bool attachServo(int servoId, int pin);
bool attachServo(int servoId, int pin, int minAngle, int maxAngle);
void detachServo(int servoId);

// Kontrol servo
bool moveServo(int servoId, int angle);
bool moveServo(int servoId, int angle, int speed);
int getServoAngle(int servoId);

// Konfigurasi MQTT
void setMQTTBroker(const char* broker, int port);
void setClientID(const char* clientId);
void setTopicPrefix(const char* prefix);

// Status dan monitoring
bool isConnected();
void enableStatusPublish(bool enable);
void setStatusInterval(int milliseconds);
```

## 🔌 Wiring Diagram

```
ESP32 DevKit V1        Servo Motors
================       =============
GPIO 5        ------>  Servo 0 (Signal)
GPIO 18       ------>  Servo 1 (Signal)
GPIO 19       ------>  Servo 2 (Signal)
GPIO 21       ------>  Servo 3 (Signal)
GPIO 22       ------>  Servo 4 (Signal)

VIN (5V)      ------>  Servo VCC (Red)
GND           ------>  Servo GND (Brown/Black)

Note: Gunakan external power supply 5V/3A untuk servo
```

## 🐛 Troubleshooting

### Masalah Umum

**ESP32 tidak connect ke WiFi**
- Pastikan SSID dan password benar
- Cek jangkauan WiFi
- Reset ESP32 dan coba lagi

**Servo tidak bergerak**
- Cek wiring dan power supply
- Pastikan pin GPIO sudah benar
- Cek serial monitor untuk error

**MQTT tidak connect**
- Pastikan broker address benar
- Cek koneksi internet
- Gunakan MQTT client untuk test broker

### Debug Mode
```cpp
// Enable debug output
robo.setDebugMode(true);
```

## 📌 Roadmap

- [ ] **v1.1** - Konfigurasi jumlah servo dinamis
- [ ] **v1.2** - Web UI untuk manual override dan monitoring
- [ ] **v1.3** - Integrasi feedback sensor (flex sensor/FSR)
- [ ] **v1.4** - Support untuk protokol lain (WebSocket, UDP)
- [ ] **v1.5** - Machine learning untuk gesture recognition offline

## 🤝 Contributing

Kontribusi sangat diterima! Silakan:

1. Fork repository ini
2. Buat branch fitur (`git checkout -b feature/AmazingFeature`)
3. Commit perubahan (`git commit -m 'Add some AmazingFeature'`)
4. Push ke branch (`git push origin feature/AmazingFeature`)
5. Buat Pull Request

## 📄 License

Distributed under the MIT License. See `LICENSE` for more information.

## 👨‍💻 Author

**Ficram Manifur Farissa**  
Teknik Elektro - Universitas Cendekia Abditama

- 📧 Email: ficramm@gmail.com
- 🐙 GitHub: [ficrammanifur](https://github.com/ficrammanifur)
- 💼 LinkedIn: [Ficram Manifur](https://linkedin.com/in/ficrammanifur)

## 🙏 Acknowledgments

- [ESP32 Arduino Core](https://github.com/espressif/arduino-esp32)
- [PubSubClient](https://github.com/knolleary/pubsubclient)
- [ESP32Servo](https://github.com/madhephaestus/ESP32Servo)
- [MediaPipe](https://mediapipe.dev/) - Google's ML framework
- Komunitas Arduino Indonesia

---

⭐ **Jika project ini membantu, jangan lupa kasih star!** ⭐
