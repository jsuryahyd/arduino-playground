# 🤖 Arduino UNO R4 Servo Explorer & Controller

An interactive, multi-mode project for driving standard Servo Motors (e.g., SG90 micro servos, MG995, MG996R) with the **Arduino UNO R4 WiFi / Minima**, complete with **UNO R4 LED Matrix live visual gauge feedback**!

---

## 📌 Hardware Wiring Guide

| Servo Wire Color | Arduino Pin | Description |
| :--- | :--- | :--- |
| **Red** | `5V` | Positive Power Supply (5V) |
| **Brown / Black** | `GND` | Common Ground |
| **Orange / Yellow** | `Pin 9` (PWM) | Pulse-Width Modulation (PWM) Signal |

> ⚠️ **Power Tip:** Small micro servos like the **SG90** can be powered directly from the Arduino UNO R4 5V pin over USB. For larger servos (like MG995/MG996R) or heavy mechanical loads, use an external 5V power supply (share GND with the Arduino) to prevent resetting the board due to voltage drops.

---

## ⚡ Key Features & Modes

This project provides 5 distinct servo operating modes:

1. 🎯 **Manual Angle Control (`0` – `180`)**: Directly position the servo arm to any exact degree.
2. 🔄 **Smooth Sweep Mode (`sweep`)**: Continuously oscillates back and forth between 0° and 180° using non-blocking timing.
3. 📐 **Step Increments (`step`)**: Pauses at key robotic angular positions (0°, 45°, 90°, 135°, 180°).
4. 🌊 **Sinusoidal Easing (`ease`)**: Smooth ease-in / ease-out motion curve (natural, organic robotic arm motion).
5. ⚡ **Expressive Bounce (`bounce`)**: Rapid double-twitch animation sequence to test responsiveness.
6. 🔋 **Power Management (`detach` / `attach`)**: Dynamically detach the PWM signal pin to stop motor chatter, save power, and avoid servo heat when idle.
7. 📊 **R4 LED Matrix Visual Gauge**: Live 12x8 LED matrix display showing a position needle and progress track corresponding to the physical servo angle in real time!

---

## 💻 Serial Monitor Commands

Open the **Serial Monitor** at **115200 Baud** (`Newline` mode) to issue interactive commands:

| Command | Action |
| :--- | :--- |
| `0` to `180` | Set exact angle (e.g., `45`, `90`, `180`) |
| `sweep` | Toggle continuous smooth oscillation mode |
| `step` | Cycle through discrete angle steps |
| `ease` | Enable sinusoidal ease-in/ease-out motion |
| `bounce` | Perform quick double-twitch demo |
| `speed <ms>` | Adjust step delay in ms (e.g. `speed 15` fast, `speed 40` slow) |
| `detach` | Turn off PWM pulse (stop jitter & save power) |
| `attach` | Re-enable PWM signal pin |
| `status` | Print active mode, attached pin status, and angle |
| `help` | Print interactive menu |

---

## 🧪 Local Unit Testing

You can verify the logic offline without hardware by executing our C++ test runner:

```bash
./tests/run_tests.sh
```

---

## 🚀 How to Compile, Upload & Interact in Antigravity IDE

In **Antigravity IDE**, you can compile, upload, and interact with the UNO R4 board directly from the terminal or using integrated tools:

1. **Compile & Upload via CLI in Antigravity Terminal:**
   ```bash
   arduino-cli compile --fqbn arduino:renesas_uno:uno_r4_wifi servo_control
   arduino-cli upload -p /dev/ttyACM0 --fqbn arduino:renesas_uno:uno_r4_wifi servo_control
   ```

2. **Open Serial Monitor in Antigravity IDE:**
   ```bash
   arduino-cli monitor -p /dev/ttyACM0 -c baudrate=115200
   ```

3. **Offline Unit Test Runner:**
   ```bash
   ./tests/run_tests.sh
   ```

