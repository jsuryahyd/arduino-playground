# 🤖 Arduino & Robotics Learning Playground

Welcome to my personal hands-on **Arduino UNO R4 WiFi & Robotics Learning Repository**! 

This repository documents my progressive journey in microcontrollers, embedded C++, hardware control, networking, and robotics using the **Arduino UNO R4 WiFi** (32-bit ARM Cortex-M4 CPU, 48 MHz) in **Antigravity IDE**.

---

## 🧭 Learning Modules & Roadmap

| Module | Location | Description | Key Concepts Learned | Status |
| :--- | :--- | :--- | :--- | :---: |
| **0. Offline Testing** | [`tests/`](file:///home/surya/development/arduino/arduino-playground/tests) | Native C++ mock test framework | `g++` compilation, hardware API mocking, 100% offline validation | ✅ Complete |
| **1. Built-in LED Matrix** | [`led_matrix_demo/`](file:///home/surya/development/arduino/arduino-playground/led_matrix_demo) | 12x8 LED Matrix graphics & animations | 2D pixel arrays, 32-bit bitmask frames, gallery animation sequences | ✅ Complete |
| **2. Wi-Fi Web Server** | [`wifi_dashboard/`](file:///home/surya/development/arduino/arduino-playground/wifi_dashboard) | Web server dashboard & Wi-Fi control | `WiFiS3.h`, credential security with `secrets.h`, HTML dynamic responses | ✅ Complete |
| **3. Servo & Robotics** | [`servo_control/`](file:///home/surya/development/arduino/arduino-playground/servo_control) | Object-Oriented servo motor controller | PWM control, OOP state encapsulation, Sinusoidal Easing, Live LED Matrix Gauge | ✅ Complete |
| **4. Next Steps** | *TBD* | Sensors, Stepper Motors, & ROS / IoT | Sensor integration, I2C/SPI buses, closed-loop feedback control | 🔄 Planned |

---

## 🛠️ Hardware Stack

- **Microcontroller**: Arduino UNO R4 WiFi (Renesas RA4M1 32-bit ARM Cortex-M4 CPU, 48 MHz, 32 KB SRAM, 256 KB Flash)
- **Actuators**: SG90 / MG995 Micro Servo Motors
- **Displays**: Built-in 12x8 LED Matrix Grid
- **Connectivity**: 2.4GHz Wi-Fi (ESP32-S3 co-processor)

---

## 🚀 Projects Overview

### 1. 🤖 Object-Oriented Servo Motor Controller (`servo_control/`)
* **Features**:
  * **Direct Angle Positioning**: Type `0` – `180` in the Serial Monitor for instant angular motion.
  * **5 Operating Modes**: `MANUAL`, `SWEEP` (smooth oscillation), `STEP` (discrete angles), `EASE` (sinusoidal ease-in/ease-out), and `BOUNCE` (expressive twitches).
  * **R4 LED Matrix Visual Gauge**: Live 12x8 visual needle bar and horizontal track tracking physical servo position in real time.
  * **Power & Jitter Saver**: `attach` and `detach` commands to turn off PWM signals when idle.
  * **Clean Architecture**: Refactored from loose global variables into an Object-Oriented C++ `ServoController` class.

### 2. 🌐 Wi-Fi Web Dashboard (`wifi_dashboard/`)
* **Features**:
  * Connects to local Wi-Fi networks using `WiFiS3.h`.
  * Protects Wi-Fi network credentials using a `.gitignore`'d `secrets.h` header file.
  * Hosts a lightweight HTTP web server returning live hardware status pages to browser clients.

### 3. 🎨 Built-in LED Matrix Demo (`led_matrix_demo/`)
* **Features**:
  * Renders custom 8x12 2D pixel grids (Smileys, Hearts).
  * Renders official packed bitmask icons (`LEDMATRIX_UNO`, `LEDMATRIX_CLOUD_WIFI`, `LEDMATRIX_LIKE`).
  * Plays official multi-frame animations (`LEDMATRIX_ANIMATION_BOUNCING_BALL`).

---

## 💻 Development Workflow in Antigravity IDE

This repository is configured to compile, flash, and test directly inside **Antigravity IDE** using `arduino-cli`:

### 1. CLI Commands
```bash
# Compile Servo Sketch
arduino-cli compile --fqbn arduino:renesas_uno:unor4wifi servo_control

# Upload to Board
arduino-cli upload -p /dev/ttyACM0 --fqbn arduino:renesas_uno:unor4wifi servo_control

# Open Serial Monitor (115200 baud)
arduino-cli monitor -p /dev/ttyACM0 -c baudrate=115200
```

### 2. VS Code Task Integration
Press `Ctrl+Shift+B` (or **Terminal -> Run Build Task**) in Antigravity IDE to execute preset tasks:
- `Arduino: Compile & Upload Active Sketch`
- `Arduino: Servo Control - Compile & Upload`
- `Arduino: Open Serial Monitor`
- `Arduino: Run Native C++ Tests (100% Offline)`

---

## 🧪 Native C++ Offline Unit Testing (`tests/`)

To test hardware logic, edge cases, state transitions, and Serial command parsing without needing physical hardware attached:

```bash
./tests/run_tests.sh
```

Tests use custom C++ mocks (`Arduino.h`, `Servo.h`, `WiFiS3.h`, `Arduino_LED_Matrix.h`) compiled via `g++` to validate code 100% offline.

---

## 💡 Key Architectural Concepts Mastered

- **Non-blocking Timing (`millis()`)**: Avoiding `delay()` to maintain responsive multi-tasking.
- **Object-Oriented Embedded C++**: Encapsulating hardware states inside C++ classes to prevent global state corruption.
- **Sinusoidal Easing Mathematics**: Using cosine wave curves ($0.5 \times (1 - \cos(\pi t))$) for smooth robotic arm movement.
- **Pulse Width Calibration**: Calibrating microsecond timing ($500\mu s - 2500\mu s$) for micro-servo accuracy.
- **Security Practices**: Isolating secrets (`secrets.h`) out of source control.
