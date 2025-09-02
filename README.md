# 🤖 Robohand Library untuk ESP32

Library Arduino yang mudah digunakan untuk mengontrol tangan robot dengan servo melalui MQTT. Dapat diintegrasikan dengan Mediapipe untuk tracking gerakan tangan secara real-time!

## 📋 Apa yang Bisa Dilakukan?

- ✅ Kontrol hingga 5 servo (jari-jari) dengan pin dan sudut yang dapat disesuaikan
- ✅ Koneksi WiFi dan MQTT broker otomatis
- ✅ Kontrol servo tunggal atau multiple sekaligus
- ✅ Gesture/gerakan tangan yang sudah terdefinisi
- ✅ Tracking jari secara real-time menggunakan kamera
- ✅ Status monitoring dan heartbeat
- ✅ Kompatibel dengan Mediapipe hand tracking

## 🛠️ Instalasi

### Langkah 1: Download Library
1. Klik tombol **"Code"** → **"Download ZIP"** di repository ini
2. Simpan file ZIP ke komputer Anda

### Langkah 2: Install ke Arduino IDE
1. Buka **Arduino IDE**
2. Pilih menu **Sketch** → **Include Library** → **Add .ZIP Library...**
3. Pilih file ZIP yang sudah didownload
4. Tunggu hingga instalasi selesai

### Langkah 3: Install Library Pendukung
Buka **Tools** → **Manage Libraries** dan install library berikut:

| Library | Versi Minimum | Cara Install |
|---------|---------------|--------------|
| **PubSubClient** | 2.8+ | Cari "PubSubClient" → Install |
| **ESP32Servo** | 1.1.1+ | Cari "ESP32Servo" → Install |
| **ArduinoJson** | 6.21.0+ | Cari "ArduinoJson" → Install |

## 🚀 Cara Penggunaan

### Contoh Dasar - Kontrol Servo via MQTT

```cpp
#include <Robohand.h>

// Konfigurasi WiFi
const char* ssid = "NAMA_WIFI_ANDA";
const char* password = "PASSWORD_WIFI_ANDA";

// Konfigurasi MQTT
const char* mqtt_server = "broker.hivemq.com";
const int mqtt_port = 1883;

Robohand robot;

void setup() {
  Serial.begin(115200);
  
  // Setup servo pins (thumb, index, middle, ring, pinky)
  int servoPins[] = {18, 19, 21, 22, 23};
  robot.begin(servoPins, 5);
  
  // Koneksi WiFi dan MQTT
  robot.connectWiFi(ssid, password);
  robot.connectMQTT(mqtt_server, mqtt_port);
}

void loop() {
  robot.loop(); // Jangan lupa panggil ini!
}
```

### 📁 Contoh Lengkap

Lihat folder `examples/` untuk contoh-contoh berikut:

#### 1. **BasicServo.ino** - Kontrol Dasar
- Kontrol servo sederhana via MQTT
- Cocok untuk pemula
- Setup minimal yang diperlukan

#### 2. **mediapipe_demo.ino** - Hand Tracking Real-time  
- Integrasi dengan kamera untuk tracking tangan
- Gerakan tangan otomatis menggerakkan servo
- Butuh Python script tambahan

#### 3. **advanced_config.ino** - Konfigurasi Lanjutan
- Custom pin assignment
- Topic MQTT yang dapat disesuaikan
- Multiple gesture presets

## 🎥 Integrasi dengan Mediapipe (Hand Tracking)

### Persiapan di Komputer

1. **Install Python** (versi 3.7+)
2. **Install dependencies:**
   ```bash
   pip install mediapipe opencv-python paho-mqtt numpy
   ```

3. **Jalankan script Python:**
   ```bash
   python mediapipe_hand_tracker.py
   ```

### Konfigurasi MQTT
Pastikan pengaturan MQTT di Arduino dan Python script sama:

**Arduino:**
```cpp
const char* mqtt_server = "broker.hivemq.com";
const char* topic = "robohand/control";
```

**Python (mediapipe_hand_tracker.py):**
```python
MQTT_BROKER = "broker.hivemq.com"
MQTT_TOPIC = "robohand/control"
```

## 📡 MQTT Topics

| Topic | Fungsi | Format Data |
|-------|--------|-------------|
| `robohand/control` | Kontrol servo | `{"finger": 0, "angle": 90}` |
| `robohand/gesture` | Gesture preset | `{"gesture": "fist"}` |
| `robohand/status` | Status servo | `{"servo0": 90, "servo1": 45}` |
| `robohand/heartbeat` | Monitoring koneksi | `{"status": "online"}` |

## 🎮 Gesture yang Tersedia

- **"open"** - Tangan terbuka (semua jari 0°)
- **"fist"** - Kepalan tangan (semua jari 180°)
- **"peace"** - Tanda peace (jari telunjuk & tengah terbuka)
- **"thumbs_up"** - Jempol ke atas
- **"point"** - Menunjuk (hanya telunjuk terbuka)

## 🔧 Troubleshooting

### ❌ Servo tidak bergerak
- ✅ Cek koneksi pin servo ke ESP32
- ✅ Pastikan power supply cukup (servo butuh arus besar)
- ✅ Periksa kode pin di sketch

### ❌ Tidak bisa connect WiFi
- ✅ Periksa SSID dan password
- ✅ Pastikan ESP32 dalam jangkauan WiFi
- ✅ Coba restart ESP32

### ❌ MQTT tidak connect
- ✅ Periksa alamat broker MQTT
- ✅ Pastikan port benar (biasanya 1883)
- ✅ Cek koneksi internet

### ❌ Mediapipe error
- ✅ Pastikan webcam tidak digunakan aplikasi lain
- ✅ Install ulang dependencies Python
- ✅ Periksa versi Python (minimal 3.7)

## 📚 Dokumentasi API

### Fungsi Utama

```cpp
// Inisialisasi dengan pin servo
robot.begin(int pins[], int count);

// Koneksi WiFi
robot.connectWiFi(const char* ssid, const char* password);

// Koneksi MQTT
robot.connectMQTT(const char* server, int port);

// Kontrol servo individual
robot.moveServo(int servoIndex, int angle);

// Kontrol gesture
robot.setGesture(const char* gestureName);

// Loop utama (wajib dipanggil di loop())
robot.loop();
```

## 🤝 Kontribusi

Ingin berkontribusi? Silakan:
1. Fork repository ini
2. Buat branch baru untuk fitur Anda
3. Commit perubahan
4. Buat Pull Request

## 📄 Lisensi

MIT License - Bebas digunakan untuk project apapun!

## 📞 Bantuan

Butuh bantuan? 
- 📧 Buat issue di GitHub
- 💬 Diskusi di tab Discussions
- 📖 Baca dokumentasi lengkap di Wiki

---

**Selamat berkreasi dengan tangan robot Anda! 🤖✋**
