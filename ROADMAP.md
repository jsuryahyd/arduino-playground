# 🗺️ Arduino & Robotics Learning Roadmap

This document outlines the project roadmap, future learning modules, and procurement recommendations for the **Arduino UNO R4 WiFi & Robotics Playground**.

---

## 🎯 Active Project

### 🌐 Module 4: IoT Wireless Servo Controller (`web_servo_control/`)
* **Status**: 🚀 **In Progress**
* **Goal**: Merge Wi-Fi HTTP Web Server, Servo Motor Control, and LED Matrix Visual Gauge into a single responsive IoT controller.
* **Key Features**:
  * Web interface with dynamic slider controls for angle positioning.
  * Mode selectors (`MANUAL`, `SWEEP`, `STEP`, `EASE`, `BOUNCE`) triggered over HTTP API.
  * Live status streaming to web client and R4 12x8 LED Matrix gauge.
  * Detach/Attach power saver commands via Web UI.
  * Native C++ unit tests for HTTP request parsing and servo state transitions.

---

## 🔮 Future Project Options

### 📡 Module 5: Ultrasonic Radar & Distance Scanner (`ultrasonic_radar/`)
* **Hardware Needed**: HC-SR04 Ultrasonic Sensor + Servo Motor + LED Matrix.
* **Goal**: Mount HC-SR04 onto servo motor, sweep 0°–180°, measure distance, and render a radar grid on the LED Matrix grid and web dashboard.
* **Key Concepts**: `pulseIn()` timing, distance calculation ($cm = \mu s / 58$), polar-to-2D grid projection, obstacle detection.

### ⚙️ Module 6: Stepper Motor & Precision Motion (`stepper_control/`)
* **Hardware Needed**: 28BYJ-48 Stepper Motor + ULN2003 Driver (or NEMA 17 + A4988 / TMC2209 driver).
* **Goal**: Implement non-blocking step sequencing (`FULL_STEP`, `HALF_STEP`, `MICROSTEPPING`) with acceleration/deceleration profiling.

### 📲 Module 7: Bluetooth LE (BLE) & MQTT Telemetry (`ble_mqtt_iot/`)
* **Hardware Needed**: ESP32-S3 Co-processor (Built into R4 WiFi).
* **Goal**: Real-time wireless telemetry using BLE Nordic UART services and MQTT protocol over Wi-Fi.

---

## 🛒 Procurement Recommendations

See [`catalog.json`](file:///home/surya/development/arduino/arduino-playground/catalog.json) for current inventory. Recommended components to procure over time:

1. **Phase 1 (Sensors & Feedback)**:
   * **MPU6050 6-Axis IMU (Gyro + Accelerometer)**: Great for self-balancing robots and gesture control.
   * **BME280 or DHT22 Temperature/Humidity Sensor**: Great for IoT environmental monitoring.
   * **VL53L0X Time-of-Flight Laser Distance Sensor**: Faster & more precise distance sensing than ultrasonic.

2. **Phase 2 (Motors & Drivers)**:
   * **28BYJ-48 Stepper Motor with ULN2003 Driver Board**: Inexpensive motor to learn stepper sequencing.
   * **L298N or TB6612FNG Dual H-Bridge Motor Driver**: Required for driving DC motors for 2WD/4WD mobile robot chassis.

3. **Phase 3 (Displays & Controls)**:
   * **0.96" I2C OLED Display (SSD1306, 128x64)**: High-density visual output beyond the built-in 12x8 LED matrix.
   * **Rotary Encoder (EC11 with push button)**: Tactile dial control for menus and positioning.
