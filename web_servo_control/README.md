# 🌐 Module 4: IoT Wireless Servo Motor Controller

An interactive, non-blocking **IoT Web Application** running directly on the **Arduino UNO R4 WiFi**. It merges high-speed web server capabilities with object-oriented servo motion control and the onboard 12x8 LED Matrix visual gauge.

---

## ✨ Features

- **🎨 Modern Dark Glassmorphic Web Interface**: Served directly from the R4 WiFi over HTTP.
- **🎛️ Real-time Interactive Control**:
  - Drag-and-drop smooth angle slider ($0^\circ - 180^\circ$).
  - Instant angle preset buttons ($0^\circ$, $45^\circ$, $90^\circ$, $135^\circ$, $180^\circ$).
- **🔄 5 Motion Operating Modes**:
  - `MANUAL`: Direct angular positioning.
  - `SWEEP`: Non-blocking $0^\circ \leftrightarrow 180^\circ$ continuous oscillation.
  - `STEP`: Sequenced discrete step angles.
  - `EASE`: Sinusoidal ease-in / ease-out smooth organic movement.
  - `BOUNCE`: Expressive double-twitch animation sequence.
- **⚡ Power Saver Control**: Wirelessly toggle PWM signal attachment (`ATTACH` / `DETACH`) to save power and eliminate idle chatter.
- **📊 Dynamic LED Matrix Gauge**: Hardware 12x8 LED grid tracks physical servo position in real-time.
- **🔌 REST API Endpoints**:
  - `GET /api/status` $\rightarrow$ Returns live JSON state (`{"angle": 90, "mode": "MANUAL", "attached": true}`).
  - `GET /api/set?angle=135` $\rightarrow$ Sets target servo angle.
  - `GET /api/mode?name=sweep` $\rightarrow$ Switch motion operating mode.
  - `GET /api/power?action=detach` $\rightarrow$ Attach or detach servo pin.

---

## 🛠️ Hardware Setup

| Component | Pin / Connection | Description |
| :--- | :--- | :--- |
| **Servo Signal (PWM)** | `Digital Pin 9` | Control wire (Orange / Yellow) |
| **Servo VCC** | `5V` Pin | Power wire (Red) |
| **Servo GND** | `GND` Pin | Ground wire (Black / Brown) |
| **Wi-Fi Module** | Onboard ESP32-S3 | Connects to local Wi-Fi |
| **Visual Display** | Onboard 12x8 LED Matrix | Visual needle & level gauge |

---

## 🚀 How to Compile & Flash

1. Configure Wi-Fi credentials in `web_servo_control/secrets.h`:
   ```cpp
   #define SECRET_SSID "Your_WiFi_Name"
   #define SECRET_PASS "Your_WiFi_Password"
   ```

2. Compile using `arduino-cli`:
   ```bash
   arduino-cli compile --fqbn arduino:renesas_uno:unor4wifi web_servo_control
   ```

3. Upload to board:
   ```bash
   arduino-cli upload -p /dev/ttyACM0 --fqbn arduino:renesas_uno:unor4wifi web_servo_control
   ```

4. Open Serial Monitor at 115200 baud to retrieve the board's Wi-Fi IP address:
   ```bash
   arduino-cli monitor -p /dev/ttyACM0 -c baudrate=115200
   ```

5. Open your browser or smartphone and navigate to `http://<IP_ADDRESS>`.
