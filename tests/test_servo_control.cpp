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
    std::cout << "TEST 1: Testing OOP ServoController Setup...\n";
    std::cout << "===========================================\n";

    setup();

    assert(controller.isAttached() == true);
    assert(controller.getPin() == 9);
    assert(controller.getAngle() == 90);

    std::cout << "✅ TEST 1 PASSED: Controller attached to Pin " << controller.getPin() << " at initial angle 90°!\n";
}

void test_angle_mapping_and_matrix_rendering() {
    std::cout << "\n===========================================\n";
    std::cout << "TEST 2: Testing Angle Settings & Matrix Canvas...\n";
    std::cout << "===========================================\n";

    // Set angle to 0°
    controller.setAngle(0);
    assert(controller.getAngle() == 0);
    assert(controller.getCanvasPixel(7, 0) == 1);

    // Set angle to 180°
    controller.setAngle(180);
    assert(controller.getAngle() == 180);
    assert(controller.getCanvasPixel(7, 11) == 1);

    // Bounds checking (clamp < 0 or > 180)
    controller.setAngle(-10);
    assert(controller.getAngle() == 0);

    controller.setAngle(200);
    assert(controller.getAngle() == 180);

    std::cout << "✅ TEST 2 PASSED: Angle bounds clamping & LED Matrix canvas calculations verified!\n";
}

void test_serial_commands() {
    std::cout << "\n===========================================\n";
    std::cout << "TEST 3: Testing Serial Command Processing...\n";
    std::cout << "===========================================\n";

    // Test setting angle via Serial
    Serial.injectInput("45\n");
    controller.processSerialInput();
    assert(controller.getAngle() == 45);
    assert(controller.getMode() == MODE_MANUAL);

    // Test setting sweep mode
    Serial.injectInput("sweep\n");
    controller.processSerialInput();
    assert(controller.getMode() == MODE_SWEEP);

    // Test setting step mode
    Serial.injectInput("step\n");
    controller.processSerialInput();
    assert(controller.getMode() == MODE_STEP);

    // Test setting ease mode
    Serial.injectInput("ease\n");
    controller.processSerialInput();
    assert(controller.getMode() == MODE_EASE);

    // Test detach / attach commands
    Serial.injectInput("detach\n");
    controller.processSerialInput();
    assert(controller.isAttached() == false);

    Serial.injectInput("attach\n");
    controller.processSerialInput();
    assert(controller.isAttached() == true);

    // Test speed command
    Serial.injectInput("speed 15\n");
    controller.processSerialInput();
    assert(controller.getInterval() == 15);

    std::cout << "✅ TEST 3 PASSED: Serial command parser handled all modes, attach/detach, & speed adjustments!\n";
}

void test_servo_sweep_mode() {
    std::cout << "\n===========================================\n";
    std::cout << "TEST 4: Testing Autonomous Sweep Mode...\n";
    std::cout << "===========================================\n";

    controller.setAngle(0);
    Serial.injectInput("speed 1\n");
    controller.processSerialInput();
    Serial.injectInput("sweep\n");
    controller.processSerialInput();

    for (int i = 0; i < 185; i++) {
        delay(2);
        controller.update();
    }

    assert(controller.getSweepDirection() == -1);

    std::cout << "✅ TEST 4 PASSED: OOP Sweep logic oscillated smoothly and bounced at upper limit!\n";
}

int main() {
    std::cout << "Starting Local Native C++ Tests for Servo Control Sketch...\n";

    test_servo_initialization();
    test_angle_mapping_and_matrix_rendering();
    test_serial_commands();
    test_servo_sweep_mode();

    std::cout << "\n🎉 ALL OOP SERVO TESTS PASSED SUCCESSFULLY! 🎉\n\n";
    return 0;
}
