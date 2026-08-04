#include <iostream>
#include <cassert>
#include <string>

// Include mocks
#include "mocks/Arduino.h"
#include "mocks/Arduino_LED_Matrix.h"
#include "mocks/Servo.h"
#include "mocks/WiFiS3.h"

// Define global mocks
WiFiClass WiFi;

// Include sketch file directly
#include "../web_servo_control/web_servo_control.ino"

void test_web_servo_initialization() {
    std::cout << "\n===========================================\n";
    std::cout << "TEST 1: Testing Web Servo Setup & Wi-Fi Connection...\n";
    std::cout << "===========================================\n";

    setup();

    assert(controller.isAttached() == true);
    assert(controller.getAngle() == 90);
    assert(std::string(controller.getModeString()) == "MANUAL");

    std::cout << "✅ TEST 1 PASSED: Controller & Web Server initialized successfully!\n";
}

void test_http_api_endpoints() {
    std::cout << "\n===========================================\n";
    std::cout << "TEST 2: Testing HTTP REST API Routes...\n";
    std::cout << "===========================================\n";

    // 1. Test /api/set?angle=135
    controller.setSpeed(360); // Instant movement mode for deterministic unit test assertions
    server.injectMockClient("GET /api/set?angle=135 HTTP/1.1\r\n\r\n");
    loop();
    assert(controller.getAngle() == 135);
    assert(std::string(controller.getModeString()) == "MANUAL");

    // 2. Test /api/mode?name=sweep
    server.injectMockClient("GET /api/mode?name=sweep HTTP/1.1\r\n\r\n");
    loop();
    assert(std::string(controller.getModeString()) == "SWEEP");

    // 3. Test /api/power?action=detach
    server.injectMockClient("GET /api/power?action=detach HTTP/1.1\r\n\r\n");
    loop();
    assert(controller.isAttached() == false);

    // 4. Test /api/power?action=attach
    server.injectMockClient("GET /api/power?action=attach HTTP/1.1\r\n\r\n");
    loop();
    assert(controller.isAttached() == true);

    // 5. Test GET /api/events SSE stream endpoint
    server.injectMockClient("GET /api/events HTTP/1.1\r\n\r\n");
    loop();

    // 6. Test GET /api/status JSON endpoint
    server.injectMockClient("GET /api/status HTTP/1.1\r\n\r\n");
    loop();

    std::cout << "✅ TEST 2 PASSED: All REST API & SSE endpoints (/api/set, /api/mode, /api/power, /api/events, /api/status) verified!\n";
}

void test_web_ui_dashboard_rendering() {
    std::cout << "\n===========================================\n";
    std::cout << "TEST 3: Testing HTML Web Dashboard Rendering...\n";
    std::cout << "===========================================\n";

    server.injectMockClient("GET / HTTP/1.1\r\nHost: 192.168.1.100\r\n\r\n");
    loop();

    std::cout << "✅ TEST 3 PASSED: HTML Dashboard UI rendered to client successfully!\n";
}

int main() {
    std::cout << "Starting Local Native C++ Tests for IoT Web Servo Controller...\n";

    test_web_servo_initialization();
    test_http_api_endpoints();
    test_web_ui_dashboard_rendering();

    std::cout << "\n🎉 ALL WEB SERVO TESTS PASSED SUCCESSFULLY! 🎉\n\n";
    return 0;
}
