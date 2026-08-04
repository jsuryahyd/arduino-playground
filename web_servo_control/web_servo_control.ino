/*
  ==============================================================================
  Arduino UNO R4 WiFi - IoT Wireless Servo Controller & Web Dashboard
  ==============================================================================
  Combines non-blocking ServoController (OOP), Built-in 12x8 LED Matrix Gauge,
  and Wi-Fi Web Server into an interactive IoT Web Application.

  Features:
  - Responsive Glassmorphism Dark-Mode Web Dashboard.
  - Interactive Angle Slider (0°-180°) & Quick Preset Buttons.
  - 5 Motion Modes: MANUAL, SWEEP, STEP, EASE, BOUNCE.
  - Real-time JSON API (/api/status, /api/set, /api/mode, /api/power).
  - Power Saver attach/detach controls over Wi-Fi.
  - Simultaneous Serial Monitor CLI support.
*/

#include <Servo.h>
#include "WiFiS3.h"
#include "Arduino_LED_Matrix.h"
#include "secrets.h"
#include "index_html.h"

// Wi-Fi Configuration
const char ssid[] = SECRET_SSID;
const char pass[] = SECRET_PASS;

enum ServoMode {
  MODE_MANUAL,
  MODE_SWEEP,
  MODE_STEP,
  MODE_EASE,
  MODE_BOUNCE
};

class WebServoController {
private:
  Servo servo;
  ArduinoLEDMatrix matrix;
  int pin;

  ServoMode mode;
  int angle;
  int targetAngle;
  float currentAngleDeg;
  int motorSpeedDegSec; // Rotational velocity speed (15 - 360 deg/sec)

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
  WebServoController(int servoPin = 9)
    : pin(servoPin), mode(MODE_MANUAL), angle(90), targetAngle(90),
      currentAngleDeg(90.0f), motorSpeedDegSec(180),
      sweepDirection(1), stepIndex(0), lastUpdateMs(0),
      lastStepMs(0), updateIntervalMs(20), easeProgress(0.0f),
      easeStartAngle(0), easeTargetAngle(180) {}

  void begin() {
    servo.attach(pin, 500, 2500);
    servo.write(angle);
    matrix.begin();
    renderMatrixGauge(angle);
  }

  void update() {
    unsigned long now = millis();
    if (now - lastUpdateMs < (unsigned long)updateIntervalMs) return;
    float deltaSec = (now - lastUpdateMs) / 1000.0f;
    lastUpdateMs = now;

    float maxStep = (float)motorSpeedDegSec * deltaSec;

    switch (mode) {
      case MODE_SWEEP:
        currentAngleDeg += (float)sweepDirection * maxStep;
        if (currentAngleDeg >= 180.0f) {
          currentAngleDeg = 180.0f;
          sweepDirection = -1;
        } else if (currentAngleDeg <= 0.0f) {
          currentAngleDeg = 0.0f;
          sweepDirection = 1;
        }
        setAngleInternal((int)round(currentAngleDeg));
        break;

      case MODE_STEP:
        if (now - lastStepMs >= 1000) { // Step every second
          lastStepMs = now;
          stepIndex = (stepIndex + 1) % NUM_STEPS;
          targetAngle = stepAngles[stepIndex];
        }
        // Smoothly move towards step target
        if ((int)round(currentAngleDeg) != targetAngle) {
          if (fabsf((float)targetAngle - currentAngleDeg) <= maxStep) {
            currentAngleDeg = (float)targetAngle;
          } else if ((float)targetAngle > currentAngleDeg) {
            currentAngleDeg += maxStep;
          } else {
            currentAngleDeg -= maxStep;
          }
          setAngleInternal((int)round(currentAngleDeg));
        }
        break;

      case MODE_EASE:
        easeProgress += 0.02f * ((float)motorSpeedDegSec / 180.0f);
        if (easeProgress >= 1.0f) {
          easeProgress = 0.0f;
          int temp = easeStartAngle;
          easeStartAngle = easeTargetAngle;
          easeTargetAngle = temp;
        }
        {
          float easedT = calculateEaseInOut(easeProgress);
          currentAngleDeg = easeStartAngle + (easeTargetAngle - easeStartAngle) * easedT;
          setAngleInternal((int)round(currentAngleDeg));
        }
        break;

      case MODE_BOUNCE:
        setAngleInternal(0);
        delay(150);
        setAngleInternal(180);
        delay(150);
        setAngleInternal(45);
        delay(150);
        setAngleInternal(135);
        delay(150);
        setAngleInternal(90);
        mode = MODE_MANUAL;
        break;

      case MODE_MANUAL:
      default:
        if ((int)round(currentAngleDeg) != targetAngle) {
          if (fabsf((float)targetAngle - currentAngleDeg) <= maxStep) {
            currentAngleDeg = (float)targetAngle;
          } else if ((float)targetAngle > currentAngleDeg) {
            currentAngleDeg += maxStep;
          } else {
            currentAngleDeg -= maxStep;
          }
          setAngleInternal((int)round(currentAngleDeg));
        }
        break;
    }
  }

  void setAngle(int target) {
    mode = MODE_MANUAL;
    if (target < 0) target = 0;
    if (target > 180) target = 180;
    targetAngle = target;
    
    // At maximum speed (360 deg/sec), jump instantly
    if (motorSpeedDegSec >= 360) {
      currentAngleDeg = (float)targetAngle;
      setAngleInternal(targetAngle);
    }
  }

  void setAngleInternal(int target) {
    if (target < 0) target = 0;
    if (target > 180) target = 180;
    angle = target;

    if (servo.attached()) {
      servo.write(angle);
    }
    renderMatrixGauge(angle);
  }

  void setMode(ServoMode newMode) {
    mode = newMode;
    if (mode == MODE_STEP) {
      stepIndex = 0;
      lastStepMs = millis();
    } else if (mode == MODE_EASE) {
      easeProgress = 0.0f;
      easeStartAngle = angle;
      easeTargetAngle = (angle > 90) ? 0 : 180;
    }
  }

  void attachServo() {
    if (!servo.attached()) {
      servo.attach(pin, 500, 2500);
    }
  }

  void detachServo() {
    if (servo.attached()) {
      servo.detach();
    }
  }

  bool isAttached() { return servo.attached(); }
  int getAngle() const { return angle; }
  ServoMode getMode() const { return mode; }
  int getSpeed() const { return motorSpeedDegSec; }

  void setSpeed(int degSec) {
    if (degSec < 15) degSec = 15;
    if (degSec > 360) degSec = 360;
    motorSpeedDegSec = degSec;
  }

  const char* getModeString() const {
    switch (mode) {
      case MODE_MANUAL: return "MANUAL";
      case MODE_SWEEP:  return "SWEEP";
      case MODE_STEP:   return "STEP";
      case MODE_EASE:   return "EASE";
      case MODE_BOUNCE: return "BOUNCE";
      default:          return "UNKNOWN";
    }
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

  void processSerialInput() {
    if (!Serial.available()) return;
    String input = Serial.readStringUntil('\n');
    input.trim();
    input.toLowerCase();

    if (input == "sweep") setMode(MODE_SWEEP);
    else if (input == "step") setMode(MODE_STEP);
    else if (input == "ease") setMode(MODE_EASE);
    else if (input == "bounce") setMode(MODE_BOUNCE);
    else if (input == "attach") attachServo();
    else if (input == "detach") detachServo();
    else if (input.startsWith("speed ")) {
      int speedVal = input.substring(6).toInt();
      setSpeed(speedVal);
    }
    else {
      int val = input.toInt();
      if (val >= 0 && val <= 180 && (input == "0" || val > 0)) {
        setAngle(val);
      }
    }
  }
};

// Debug Logging Configuration (Set to 1 to enable Serial logs, 0 for zero-overhead no-op)
#ifndef DEBUG_LOGGING
#define DEBUG_LOGGING 0
#endif

#if DEBUG_LOGGING
  #define LOG_DEBUG(x) Serial.println(x)
  #define LOG_DEBUG_VAL(label, val) Serial.print(label); Serial.println(val)
#else
  #define LOG_DEBUG(x) ((void)0)
  #define LOG_DEBUG_VAL(label, val) ((void)0)
#endif

// Global Server, Controller, & Persistent SSE Stream Instances
WebServoController controller(9);
WiFiServer server(80);
WiFiClient sseClient;

int wifiStatus = WL_IDLE_STATUS;
int lastStreamedAngle = -1;
String lastStreamedMode = "";
bool lastStreamedAttached = false;
int lastStreamedSpeed = -1;

// Broadcast telemetry over persistent SSE stream (State-change only, zero idle overhead)
void broadcastSseTelemetry(bool force = false) {
  if (!sseClient || !sseClient.connected()) return;

  int currentAngle = controller.getAngle();
  String currentMode = controller.getModeString();
  bool currentAttached = controller.isAttached();
  int currentSpeed = controller.getSpeed();

  bool stateChanged = (currentAngle != lastStreamedAngle ||
                       currentMode != lastStreamedMode ||
                       currentAttached != lastStreamedAttached ||
                       currentSpeed != lastStreamedSpeed);

  // Stream event ONLY when position/mode/power/speed state actually changes or when forced
  if (force || stateChanged) {
    lastStreamedAngle = currentAngle;
    lastStreamedMode = currentMode;
    lastStreamedAttached = currentAttached;
    lastStreamedSpeed = currentSpeed;

    sseClient.print(F("data: {\"angle\":"));
    sseClient.print(currentAngle);
    sseClient.print(F(",\"mode\":\""));
    sseClient.print(currentMode);
    sseClient.print(F("\",\"attached\":"));
    sseClient.print(currentAttached ? "true" : "false");
    sseClient.print(F(",\"speed\":"));
    sseClient.print(currentSpeed);
    sseClient.println(F("}\n"));
  }
}

void setup() {
  Serial.begin(115200);
  controller.begin();

  Serial.println("Connecting to Wi-Fi...");
  while (wifiStatus != WL_CONNECTED) {
    Serial.print("Attempting connection to SSID: ");
    Serial.println(ssid);
    wifiStatus = WiFi.begin(ssid, pass);
    delay(4000);
  }

  server.begin();
  Serial.println("✅ Wi-Fi Connected!");
  Serial.print("🌐 Dashboard URL: http://");
  Serial.println(WiFi.localIP());
}

void loop() {
  // Always update background servo motor movement & matrix gauge
  controller.update();
  controller.processSerialInput();

  // Push real-time telemetry over active SSE stream ONLY on state change
  broadcastSseTelemetry(false);

  // Handle incoming HTTP REST commands & SSE connections
  WiFiClient client = server.available();
  if (client) {
    String requestLine = "";
    unsigned long startMs = millis();
    
    // Non-blocking fast line reader (read line 1 only)
    while (client.connected() && (millis() - startMs < 200)) {
      if (client.available()) {
        char c = client.read();
        if (c == '\n') break;
        if (c != '\r') requestLine += c;
      }
    }

    if (requestLine.indexOf("GET /api/events") >= 0) {
      LOG_DEBUG(F("📡 [SSE] Client connected to /api/events stream"));
      if (sseClient && sseClient.connected()) {
        sseClient.stop();
      }
      sseClient = client;

      sseClient.println(F("HTTP/1.1 200 OK"));
      sseClient.println(F("Content-Type: text/event-stream"));
      sseClient.println(F("Cache-Control: no-cache"));
      sseClient.println(F("Access-Control-Allow-Origin: *"));
      sseClient.println(F("Connection: keep-alive"));
      sseClient.println();

      broadcastSseTelemetry(true); // Send initial state push
      return; // Keep sseClient connected! Do NOT stop()
    }
    else if (requestLine.indexOf("GET /api/status") >= 0) {
      client.println(F("HTTP/1.1 200 OK"));
      client.println(F("Content-Type: application/json"));
      client.println(F("Access-Control-Allow-Origin: *"));
      client.println(F("Connection: close"));
      client.println();
      client.print(F("{\"angle\":"));
      client.print(controller.getAngle());
      client.print(F(",\"mode\":\""));
      client.print(controller.getModeString());
      client.print(F("\",\"attached\":"));
      client.print(controller.isAttached() ? "true" : "false");
      client.print(F(",\"speed\":"));
      client.print(controller.getSpeed());
      client.println(F("}"));
      client.stop();
    } 
    else if (requestLine.indexOf("GET /api/set?angle=") >= 0) {
      int idx = requestLine.indexOf("angle=") + 6;
      int angleVal = requestLine.substring(idx).toInt();
      LOG_DEBUG_VAL(F("🌐 [HTTP] Set Angle: "), angleVal);
      controller.setAngle(angleVal);

      client.println(F("HTTP/1.1 200 OK"));
      client.println(F("Content-Type: text/plain"));
      client.println(F("Access-Control-Allow-Origin: *"));
      client.println(F("Connection: close"));
      client.println();
      client.println(F("OK"));
      client.stop();

      broadcastSseTelemetry(true); // Trigger instant SSE update
    }
    else if (requestLine.indexOf("GET /api/speed?") >= 0) {
      int idx = requestLine.indexOf("val=");
      if (idx < 0) idx = requestLine.indexOf("ms=");
      if (idx >= 0) {
        idx = requestLine.indexOf("=", idx) + 1;
        int speedVal = requestLine.substring(idx).toInt();
        LOG_DEBUG_VAL(F("🌐 [HTTP] Set Speed (°/s): "), speedVal);
        controller.setSpeed(speedVal);
      }

      client.println(F("HTTP/1.1 200 OK"));
      client.println(F("Content-Type: text/plain"));
      client.println(F("Access-Control-Allow-Origin: *"));
      client.println(F("Connection: close"));
      client.println();
      client.println(F("OK"));
      client.stop();

      broadcastSseTelemetry(true); // Trigger instant SSE update
    }
    else if (requestLine.indexOf("GET /api/mode?name=") >= 0) {
      if (requestLine.indexOf("name=sweep") >= 0) { controller.setMode(MODE_SWEEP); LOG_DEBUG(F("🌐 [HTTP] Set Mode: SWEEP")); }
      else if (requestLine.indexOf("name=step") >= 0) { controller.setMode(MODE_STEP); LOG_DEBUG(F("🌐 [HTTP] Set Mode: STEP")); }
      else if (requestLine.indexOf("name=ease") >= 0) { controller.setMode(MODE_EASE); LOG_DEBUG(F("🌐 [HTTP] Set Mode: EASE")); }
      else if (requestLine.indexOf("name=bounce") >= 0) { controller.setMode(MODE_BOUNCE); LOG_DEBUG(F("🌐 [HTTP] Set Mode: BOUNCE")); }
      else { controller.setMode(MODE_MANUAL); LOG_DEBUG(F("🌐 [HTTP] Set Mode: MANUAL")); }

      client.println(F("HTTP/1.1 200 OK"));
      client.println(F("Content-Type: text/plain"));
      client.println(F("Access-Control-Allow-Origin: *"));
      client.println(F("Connection: close"));
      client.println();
      client.println(F("OK"));
      client.stop();

      broadcastSseTelemetry(true); // Trigger instant SSE update
    }
    else if (requestLine.indexOf("GET /api/power?action=") >= 0) {
      if (requestLine.indexOf("action=attach") >= 0) {
        controller.attachServo();
        LOG_DEBUG(F("🌐 [HTTP] Power: ATTACH"));
      } else {
        controller.detachServo();
        LOG_DEBUG(F("🌐 [HTTP] Power: DETACH"));
      }
      client.println(F("HTTP/1.1 200 OK"));
      client.println(F("Content-Type: text/plain"));
      client.println(F("Access-Control-Allow-Origin: *"));
      client.println(F("Connection: close"));
      client.println();
      client.println(F("OK"));
      client.stop();

      broadcastSseTelemetry(true); // Trigger instant SSE update
    }
    else if (requestLine.indexOf("GET /") >= 0) {
      LOG_DEBUG(F("🌐 [HTTP] Serving Web Dashboard HTML"));
      client.println(F("HTTP/1.1 200 OK"));
      client.println(F("Content-Type: text/html"));
      client.println(F("Connection: close"));
      client.println();
      client.println(INDEX_HTML);
      client.stop();
    }
    else {
      client.stop();
    }
  }
}
