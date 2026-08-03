#include <iostream>
#include <cassert>
#include <string>

// Include mocks
#include "mocks/Arduino.h"
#include "mocks/Arduino_LED_Matrix.h"
#include "mocks/Servo.h"

// Define global mock instance
HardwareSerial Serial;

// Include sketch file directly
#include "../servo_control/servo_control.ino"

void test_servo_initialization() {
    std::cout << "\n===========================================\n";
    std::cout << "TEST 1: Testing Servo & Matrix Setup...\n";
    std::cout << "===========================================\n";

    setup();

    assert(myServo.attached() == true);
    assert(myServo.getPin() == SERVO_PIN);
    assert(myServo.read() == 90);
    assert(matrix.is_initialized == true);

    std::cout << "✅ TEST 1 PASSED: Servo attached to Pin " << SERVO_PIN << " at initial angle 90°!\n";
}

void test_angle_mapping_and_matrix_rendering() {
    std::cout << "\n===========================================\n";
    std::cout << "TEST 2: Testing Angle Settings & Matrix Canvas...\n";
    std::cout << "===========================================\n";

    // Set angle to 0°
    setServoAngle(0);
    assert(currentAngle == 0);
    assert(myServo.read() == 0);
    // Needle & track should be mapped to col 0
    assert(matrixCanvas[7][0] == 1);

    // Set angle to 180°
    setServoAngle(180);
    assert(currentAngle == 180);
    assert(myServo.read() == 180);
    // Needle & track should be mapped to col 11
    assert(matrixCanvas[7][11] == 1);

    // Bounds checking (clamp < 0 or > 180)
    setServoAngle(-10);
    assert(currentAngle == 0);

    setServoAngle(200);
    assert(currentAngle == 180);

    std::cout << "✅ TEST 2 PASSED: Angle bounds clamping & LED Matrix canvas calculations verified!\n";
}

void test_serial_commands() {
    std::cout << "\n===========================================\n";
    std::cout << "TEST 3: Testing Serial Command Processing...\n";
    std::cout << "===========================================\n";

    // Test setting angle via Serial
    Serial.injectInput("45\n");
    processSerialInput();
    assert(currentAngle == 45);
    assert(currentMode == MODE_MANUAL);

    // Test setting sweep mode
    Serial.injectInput("sweep\n");
    processSerialInput();
    assert(currentMode == MODE_SWEEP);

    // Test setting step mode
    Serial.injectInput("step\n");
    processSerialInput();
    assert(currentMode == MODE_STEP);

    // Test setting ease mode
    Serial.injectInput("ease\n");
    processSerialInput();
    assert(currentMode == MODE_EASE);

    // Test detach / attach commands
    Serial.injectInput("detach\n");
    processSerialInput();
    assert(myServo.attached() == false);

    Serial.injectInput("attach\n");
    processSerialInput();
    assert(myServo.attached() == true);

    // Test speed command
    Serial.injectInput("speed 15\n");
    processSerialInput();
    assert(updateIntervalMs == 15);

    std::cout << "✅ TEST 3 PASSED: Serial command parser handled all modes, attach/detach, & speed adjustments!\n";
}

void test_servo_sweep_mode() {
    std::cout << "\n===========================================\n";
    std::cout << "TEST 4: Testing Autonomous Sweep Mode...\n";
    std::cout << "===========================================\n";

    currentMode = MODE_SWEEP;
    currentAngle = 0;
    sweepDirection = 1;
    updateIntervalMs = 0; // Immediate step for test

    for (int i = 0; i < 200; i++) {
        updateServoLogic();
    }

    // Sweep should have reached 180° and reversed direction
    assert(sweepDirection == -1);

    std::cout << "✅ TEST 4 PASSED: Sweep logic oscillated smoothly and bounced at upper limit!\n";
}

int main() {
    std::cout << "Starting Local Native C++ Tests for Servo Control Sketch...\n";

    test_servo_initialization();
    test_angle_mapping_and_matrix_rendering();
    test_serial_commands();
    test_servo_sweep_mode();

    std::cout << "\n🎉 ALL SERVO TESTS PASSED SUCCESSFULLY! 🎉\n\n";
    return 0;
}
