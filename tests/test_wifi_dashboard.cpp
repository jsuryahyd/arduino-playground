#include <iostream>
#include <cassert>
#include <string>

// Include mocks
#include "mocks/Arduino.h"
#include "mocks/WiFiS3.h"
#include "mocks/Arduino_LED_Matrix.h"

// Define global mock instances
WiFiClass WiFi;

// Include sketch file directly
#include "../wifi_dashboard/wifi_dashboard.ino"

void test_setup_initialization() {
    std::cout << "\n===========================================\n";
    std::cout << "TEST 1: Testing setup() Initialization...\n";
    std::cout << "===========================================\n";

    setup();

    assert(matrix.is_initialized == true);
    assert(status == WL_CONNECTED);
    std::cout << "✅ TEST 1 PASSED: setup() initialized LED Matrix & connected to Wi-Fi SSID '" << ssid << "' successfully!\n";
}

void test_http_request_response() {
    std::cout << "\n===========================================\n";
    std::cout << "TEST 2: Testing HTTP Web Dashboard Response...\n";
    std::cout << "===========================================\n";

    // Simulate an incoming HTTP GET request from a browser
    std::string http_get_request = "GET / HTTP/1.1\r\nHost: 192.168.1.5\r\nUser-Agent: Mozilla/5.0\r\n\r\n";
    server.injectMockClient(http_get_request);

    // Execute loop() to process the request
    loop();

    // Capture the generated response from shared pointer tx_stream
    std::string html_output = server.getPendingClient().tx_stream->str();

    std::cout << "\n--- Generated HTTP Response Output ---\n";
    std::cout << html_output << "\n---------------------------------------\n";

    // Validate HTTP Response Headers & Body
    assert(html_output.find("HTTP/1.1 200 OK") != std::string::npos);
    assert(html_output.find("Content-type:text/html") != std::string::npos);
    assert(html_output.find("<h1>Arduino UNO R4 WiFi</h1>") != std::string::npos);
    assert(html_output.find("Board Status: <b>Online</b>") != std::string::npos);

    std::cout << "✅ TEST 2 PASSED: Web Dashboard HTTP 200 OK & HTML page generated correctly!\n";
}

int main() {
    std::cout << "Starting Local Native C++ Tests for Arduino Sketch...\n";

    test_setup_initialization();
    test_http_request_response();

    std::cout << "\n🎉 ALL TESTS PASSED SUCCESSFULLY 100% LOCALLY! 🎉\n\n";
    return 0;
}
