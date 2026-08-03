/*
  ==============================================================================
  Arduino UNO R4 WiFi - Servo Motor Explorer & Controller
  ==============================================================================
  Features:
    1. Direct Angle Control: Type any angle (0 - 180°) in Serial Monitor.
    2. Continuous Sweep Mode ("sweep"): Non-blocking smooth oscillation.
    3. Step Position Mode ("step"): Discrete position increments (0°, 45°, 90°, 135°, 180°).
    4. Smooth Easing Mode ("ease"): Sinusoidal ease-in / ease-out robotic movement.
    5. Expressive Twitch ("bounce"): Quick double-twitch / bounce demo.
    6. Power Management ("attach" / "detach"): Turn servo PWM on/off to reduce chatter.
    7. LED Matrix Gauge: Real-time visual angle needle & position indicator on UNO R4 LED Matrix!

  Wiring Guide (Standard Micro Servo SG90 / MG990):
    - Red wire    --> Arduino 5V
    - Brown/Black --> Arduino GND
    - Yellow/Orange--> Arduino PWM Pin 9 (configurable SERVO_PIN)
  ==============================================================================
*/

#include <Servo.h>
#include "Arduino_LED_Matrix.h"

// Hardware Configuration
const int SERVO_PIN = 9;

// Global Objects
Servo myServo;
ArduinoLEDMatrix matrix;

// Operating Modes
enum ServoMode {
  MODE_MANUAL,
  MODE_SWEEP,
  MODE_STEP,
  MODE_EASE,
  MODE_BOUNCE
};

ServoMode currentMode = MODE_MANUAL;

// State Variables
int currentAngle = 90;
int sweepDirection = 1;      // 1 = moving towards 180, -1 = moving towards 0
int stepIndex = 0;
const int stepAngles[] = { 0, 45, 90, 135, 180, 135, 90, 45 };
const int numSteps = sizeof(stepAngles) / sizeof(stepAngles[0]);

// Timing variables for non-blocking loop
unsigned long lastUpdateMs = 0;
int updateIntervalMs = 20;   // Movement delay per degree (controls speed)

// Easing state
float easeProgress = 0.0;    // 0.0 to 1.0
int easeStartAngle = 0;
int easeTargetAngle = 180;
bool easeForward = true;

// LED Matrix Angle Indicator Canvas (8 rows x 12 columns)
uint8_t matrixCanvas[8][12];

// Function Prototypes
void printMenu();
void processSerialInput();
void updateServoLogic();
void setServoAngle(int angle);
void renderMatrixGauge(int angle);
float calculateEaseInOut(float t);

void setup() {
  Serial.begin(115200);
  
  // Initialize Servo
  myServo.attach(SERVO_PIN);
  myServo.write(currentAngle);

  // Initialize UNO R4 LED Matrix
  matrix.begin();
  renderMatrixGauge(currentAngle);

  delay(500);
  printMenu();
}

void loop() {
  processSerialInput();
  updateServoLogic();
}

// -----------------------------------------------------------------------------
// Print Interactive Menu & Instructions
// -----------------------------------------------------------------------------
void printMenu() {
  Serial.println(F("\n=================================================="));
  Serial.println(F("🤖 Arduino UNO R4 Servo Explorer"));
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

// -----------------------------------------------------------------------------
// Process Serial Input Commands
// -----------------------------------------------------------------------------
void processSerialInput() {
  if (!Serial.available()) return;

  String input = Serial.readStringUntil('\n');
  input.trim();
  input.toLowerCase();

  if (input.length() == 0) return;

  if (input == "sweep") {
    currentMode = MODE_SWEEP;
    Serial.println(F("▶️ Mode set to: SWEEP"));
  } else if (input == "step") {
    currentMode = MODE_STEP;
    stepIndex = 0;
    Serial.println(F("▶️ Mode set to: STEP"));
  } else if (input == "ease") {
    currentMode = MODE_EASE;
    easeProgress = 0.0;
    easeStartAngle = currentAngle;
    easeTargetAngle = (currentAngle > 90) ? 0 : 180;
    Serial.println(F("▶️ Mode set to: EASE (Sinusoidal Motion)"));
  } else if (input == "bounce") {
    currentMode = MODE_BOUNCE;
    Serial.println(F("▶️ Triggering BOUNCE twitch sequence..."));
  } else if (input == "attach") {
    if (!myServo.attached()) {
      myServo.attach(SERVO_PIN);
      Serial.println(F("✅ Servo ATTACHED to Pin 9"));
    } else {
      Serial.println(F("ℹ️ Servo already attached"));
    }
  } else if (input == "detach") {
    if (myServo.attached()) {
      myServo.detach();
      Serial.println(F("⏹️ Servo DETACHED (Power saved / No chatter)"));
    } else {
      Serial.println(F("ℹ️ Servo already detached"));
    }
  } else if (input == "status") {
    Serial.print(F("📊 Current Angle: "));
    Serial.print(currentAngle);
    Serial.print(F("° | Pin Attached: "));
    Serial.print(myServo.attached() ? "YES" : "NO");
    Serial.print(F(" | Active Mode: "));
    switch (currentMode) {
      case MODE_MANUAL: Serial.println(F("MANUAL")); break;
      case MODE_SWEEP:  Serial.println(F("SWEEP")); break;
      case MODE_STEP:   Serial.println(F("STEP")); break;
      case MODE_EASE:   Serial.println(F("EASE")); break;
      case MODE_BOUNCE: Serial.println(F("BOUNCE")); break;
    }
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
    // Check if user entered a numerical angle
    int angle = input.toInt();
    if (angle >= 0 && angle <= 180 && (input == "0" || angle > 0)) {
      currentMode = MODE_MANUAL;
      setServoAngle(angle);
      Serial.print(F("🎯 Manual Position set to: "));
      Serial.print(angle);
      Serial.println(F("°"));
    } else {
      Serial.println(F("❓ Unknown command. Type 'help' for instructions."));
    }
  }
}

// -----------------------------------------------------------------------------
// Update Servo Movement Logic (Non-blocking using millis())
// -----------------------------------------------------------------------------
void updateServoLogic() {
  unsigned long now = millis();
  if (now - lastUpdateMs < (unsigned long)updateIntervalMs) return;
  lastUpdateMs = now;

  switch (currentMode) {
    case MODE_SWEEP:
      currentAngle += sweepDirection;
      if (currentAngle >= 180) {
        currentAngle = 180;
        sweepDirection = -1;
      } else if (currentAngle <= 0) {
        currentAngle = 0;
        sweepDirection = 1;
      }
      setServoAngle(currentAngle);
      break;

    case MODE_STEP:
      if (now - lastUpdateMs >= (unsigned long)updateIntervalMs * 20) { // Slower pause between steps
        stepIndex = (stepIndex + 1) % numSteps;
        setServoAngle(stepAngles[stepIndex]);
      }
      break;

    case MODE_EASE:
      easeProgress += 0.02;
      if (easeProgress >= 1.0) {
        easeProgress = 0.0;
        // Swap target & start
        int temp = easeStartAngle;
        easeStartAngle = easeTargetAngle;
        easeTargetAngle = temp;
      }
      {
        float easedT = calculateEaseInOut(easeProgress);
        int newAngle = (int)(easeStartAngle + (easeTargetAngle - easeStartAngle) * easedT);
        setServoAngle(newAngle);
      }
      break;

    case MODE_BOUNCE:
      // Perform a rapid double twitch pattern then return to MANUAL mode
      setServoAngle(0);
      delay(200);
      setServoAngle(180);
      delay(200);
      setServoAngle(45);
      delay(150);
      setServoAngle(135);
      delay(150);
      setServoAngle(90);
      Serial.println(F("✅ Bounce animation complete. Switched to MANUAL mode."));
      currentMode = MODE_MANUAL;
      break;

    case MODE_MANUAL:
    default:
      // No autonomous motion in manual mode
      break;
  }
}

// -----------------------------------------------------------------------------
// Move Servo & Refresh R4 LED Matrix Display
// -----------------------------------------------------------------------------
void setServoAngle(int angle) {
  if (angle < 0) angle = 0;
  if (angle > 180) angle = 180;
  currentAngle = angle;

  if (myServo.attached()) {
    myServo.write(currentAngle);
  }
  
  renderMatrixGauge(currentAngle);
}

// -----------------------------------------------------------------------------
// Calculate Sinusoidal Ease-In / Ease-Out (0.0 to 1.0)
// -----------------------------------------------------------------------------
float calculateEaseInOut(float t) {
  // Cosine-based ease in out: (1 - cos(t * PI)) / 2
  return 0.5 * (1.0 - cos(t * 3.14159265));
}

// -----------------------------------------------------------------------------
// Render Servo Position Visual Gauge on R4 12x8 LED Matrix
// -----------------------------------------------------------------------------
void renderMatrixGauge(int angle) {
  // Clear canvas
  memset(matrixCanvas, 0, sizeof(matrixCanvas));

  // Map 0-180 degrees to column index 0 to 11
  int targetCol = map(angle, 0, 180, 0, 11);
  if (targetCol < 0) targetCol = 0;
  if (targetCol > 11) targetCol = 11;

  // Row 0: Top Header / Corner Markers (0°, 90°, 180° markers)
  matrixCanvas[0][0]  = 1; // Left bound
  matrixCanvas[0][5]  = 1; // Center bound
  matrixCanvas[0][6]  = 1; // Center bound
  matrixCanvas[0][11] = 1; // Right bound

  // Rows 1-5: Vertical Servo Needle Bar
  for (int r = 1; r <= 5; r++) {
    matrixCanvas[r][targetCol] = 1;
    // Highlight needle tip wider for better visibility
    if (r == 1 || r == 2) {
      if (targetCol > 0)  matrixCanvas[r][targetCol - 1] = 1;
      if (targetCol < 11) matrixCanvas[r][targetCol + 1] = 1;
    }
  }

  // Rows 6-7: Horizontal Progress Track (LEDs lit up to targetCol)
  for (int c = 0; c <= targetCol; c++) {
    matrixCanvas[7][c] = 1;
  }

  // Update LED matrix
  matrix.loadPixels((uint8_t*)matrixCanvas, 96);
}
