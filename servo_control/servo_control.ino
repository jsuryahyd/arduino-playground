/*
  ==============================================================================
  Arduino UNO R4 WiFi - Object-Oriented Servo Motor Controller
  ==============================================================================
  Features clean C++ Object-Oriented encapsulation inside `ServoController` class.
  All state variables (angles, modes, timing, canvas) are safely encapsulated.
*/

#include <Servo.h>
#include "Arduino_LED_Matrix.h"

enum ServoMode {
  MODE_MANUAL,
  MODE_SWEEP,
  MODE_STEP,
  MODE_EASE,
  MODE_BOUNCE
};

class ServoController {
private:
  Servo servo;
  ArduinoLEDMatrix matrix;
  int pin;
  
  ServoMode mode;
  int angle;
  int sweepDirection;
  int stepIndex;
  
  unsigned long lastUpdateMs;
  unsigned long lastStepMs;
  int updateIntervalMs;
  
  float easeProgress;
  int easeStartAngle;
  int easeTargetAngle;
  
  uint8_t canvas[8][12];

  static const int NUM_STEPS = 8;
  const int stepAngles[NUM_STEPS] = { 0, 45, 90, 135, 180, 135, 90, 45 };

  float calculateEaseInOut(float t) {
    return 0.5f * (1.0f - cosf(t * 3.14159265f));
  }

public:
  ServoController(int servoPin = 9)
    : pin(servoPin), mode(MODE_MANUAL), angle(90),
      sweepDirection(1), stepIndex(0), lastUpdateMs(0),
      lastStepMs(0), updateIntervalMs(20), easeProgress(0.0f),
      easeStartAngle(0), easeTargetAngle(180) {}

  void begin() {
    // 500us to 2500us calibration for full SG90 micro-servo pulse range
    servo.attach(pin, 500, 2500);
    servo.write(angle);
    matrix.begin();
    renderMatrixGauge(angle);
  }

  void processSerialInput() {
    if (!Serial.available()) return;

    String input = Serial.readStringUntil('\n');
    input.trim();
    input.toLowerCase();

    if (input.length() == 0) return;

    if (input == "sweep") {
      mode = MODE_SWEEP;
      Serial.println(F("▶️ Mode set to: SWEEP"));
    } else if (input == "step") {
      mode = MODE_STEP;
      stepIndex = 0;
      lastStepMs = millis();
      Serial.println(F("▶️ Mode set to: STEP"));
    } else if (input == "ease") {
      mode = MODE_EASE;
      easeProgress = 0.0f;
      easeStartAngle = angle;
      easeTargetAngle = (angle > 90) ? 0 : 180;
      Serial.println(F("▶️ Mode set to: EASE (Sinusoidal Motion)"));
    } else if (input == "bounce") {
      mode = MODE_BOUNCE;
      Serial.println(F("▶️ Triggering BOUNCE twitch sequence..."));
    } else if (input == "attach") {
      if (!servo.attached()) {
        servo.attach(pin, 500, 2500);
        Serial.println(F("✅ Servo ATTACHED"));
      } else {
        Serial.println(F("ℹ️ Servo already attached"));
      }
    } else if (input == "detach") {
      if (servo.attached()) {
        servo.detach();
        Serial.println(F("⏹️ Servo DETACHED (Power saved / No chatter)"));
      } else {
        Serial.println(F("ℹ️ Servo already detached"));
      }
    } else if (input == "status") {
      printStatus();
    } else if (input == "help") {
      printMenu();
    } else if (input.startsWith("speed ")) {
      int val = input.substring(6).toInt();
      if (val > 0 && val <= 500) {
        updateIntervalMs = val;
        Serial.print(F("⚡ Animation step speed set to: "));
        Serial.print(updateIntervalMs);
        Serial.println(F(" ms"));
      } else {
        Serial.println(F("❌ Invalid speed! Enter a value between 1 and 500 ms."));
      }
    } else {
      int newAngle = input.toInt();
      if (newAngle >= 0 && newAngle <= 180 && (input == "0" || newAngle > 0)) {
        mode = MODE_MANUAL;
        setAngle(newAngle);
        Serial.print(F("🎯 Manual Position set to: "));
        Serial.print(newAngle);
        Serial.println(F("°"));
      } else {
        Serial.println(F("❓ Unknown command. Type 'help' for instructions."));
      }
    }
  }

  void update() {
    unsigned long now = millis();
    if (now - lastUpdateMs < (unsigned long)updateIntervalMs) return;
    lastUpdateMs = now;

    switch (mode) {
      case MODE_SWEEP:
        angle += sweepDirection;
        if (angle >= 180) {
          angle = 180;
          sweepDirection = -1;
        } else if (angle <= 0) {
          angle = 0;
          sweepDirection = 1;
        }
        setAngle(angle);
        break;

      case MODE_STEP:
        if (now - lastStepMs >= (unsigned long)updateIntervalMs * 25) {
          lastStepMs = now;
          stepIndex = (stepIndex + 1) % NUM_STEPS;
          setAngle(stepAngles[stepIndex]);
          Serial.print(F("📐 Step position: "));
          Serial.print(stepAngles[stepIndex]);
          Serial.println(F("°"));
        }
        break;

      case MODE_EASE:
        easeProgress += 0.02f;
        if (easeProgress >= 1.0f) {
          easeProgress = 0.0f;
          int temp = easeStartAngle;
          easeStartAngle = easeTargetAngle;
          easeTargetAngle = temp;
        }
        {
          float easedT = calculateEaseInOut(easeProgress);
          int newAngle = (int)(easeStartAngle + (easeTargetAngle - easeStartAngle) * easedT);
          setAngle(newAngle);
        }
        break;

      case MODE_BOUNCE:
        setAngle(0);
        delay(200);
        setAngle(180);
        delay(200);
        setAngle(45);
        delay(150);
        setAngle(135);
        delay(150);
        setAngle(90);
        Serial.println(F("✅ Bounce animation complete. Switched to MANUAL mode."));
        mode = MODE_MANUAL;
        break;

      case MODE_MANUAL:
      default:
        break;
    }
  }

  void setAngle(int targetAngle) {
    if (targetAngle < 0) targetAngle = 0;
    if (targetAngle > 180) targetAngle = 180;
    angle = targetAngle;

    if (servo.attached()) {
      servo.write(angle);
    }
    renderMatrixGauge(angle);
  }

  void renderMatrixGauge(int currentAngle) {
    memset(canvas, 0, sizeof(canvas));

    int targetCol = map(currentAngle, 0, 180, 0, 11);
    if (targetCol < 0) targetCol = 0;
    if (targetCol > 11) targetCol = 11;

    canvas[0][0]  = 1;
    canvas[0][5]  = 1;
    canvas[0][6]  = 1;
    canvas[0][11] = 1;

    for (int r = 1; r <= 5; r++) {
      canvas[r][targetCol] = 1;
      if (r == 1 || r == 2) {
        if (targetCol > 0)  canvas[r][targetCol - 1] = 1;
        if (targetCol < 11) canvas[r][targetCol + 1] = 1;
      }
    }

    for (int c = 0; c <= targetCol; c++) {
      canvas[7][c] = 1;
    }

    matrix.loadPixels((uint8_t*)canvas, 96);
  }

  void printMenu() {
    Serial.println(F("\n=================================================="));
    Serial.println(F("🤖 Arduino UNO R4 Servo Controller (OOP Edition)"));
    Serial.println(F("=================================================="));
    Serial.println(F(" Commands:"));
    Serial.println(F("  [0 - 180]  : Set servo angle immediately (e.g. 0, 90, 180)"));
    Serial.println(F("  sweep      : Toggle continuous smooth sweep (0° <-> 180°)"));
    Serial.println(F("  step       : Cycle through discrete angles (0, 45, 90, 135, 180)"));
    Serial.println(F("  ease       : Sinusoidal ease-in / ease-out natural motion"));
    Serial.println(F("  bounce     : Perform expressiveness test / quick double-twitch"));
    Serial.println(F("  speed <ms> : Set animation delay in ms (e.g. 'speed 15')"));
    Serial.println(F("  attach     : Attach servo pin"));
    Serial.println(F("  detach     : Detach servo pin (stop pulse / prevent jitter)"));
    Serial.println(F("  status     : Show current angle, pin status, and active mode"));
    Serial.println(F("  help       : Display this menu"));
    Serial.println(F("==================================================\n"));
  }

  void printStatus() {
    Serial.print(F("📊 Current Angle: "));
    Serial.print(angle);
    Serial.print(F("° | Pin Attached: "));
    Serial.print(servo.attached() ? "YES" : "NO");
    Serial.print(F(" | Active Mode: "));
    switch (mode) {
      case MODE_MANUAL: Serial.println(F("MANUAL")); break;
      case MODE_SWEEP:  Serial.println(F("SWEEP")); break;
      case MODE_STEP:   Serial.println(F("STEP")); break;
      case MODE_EASE:   Serial.println(F("EASE")); break;
      case MODE_BOUNCE: Serial.println(F("BOUNCE")); break;
    }
  }

  // Public Getters for Unit Testing & State Querying
  int getAngle() const { return angle; }
  ServoMode getMode() const { return mode; }
  bool isAttached() { return servo.attached(); }
  int getPin() const { return pin; }
  int getInterval() const { return updateIntervalMs; }
  int getSweepDirection() const { return sweepDirection; }
  uint8_t getCanvasPixel(int r, int c) const { return canvas[r][c]; }
};

// Global Instance
ServoController controller(9);

void setup() {
  Serial.begin(115200);
  controller.begin();
  delay(500);
  controller.printMenu();
}

void loop() {
  controller.processSerialInput();
  controller.update();
}
