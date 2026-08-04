#ifndef MOCK_WIFIS3_H
#define MOCK_WIFIS3_H

#include "Arduino.h"
#include <queue>
#include <sstream>
#include <memory>

#define WL_IDLE_STATUS 0
#define WL_CONNECTED 3
#define WL_NO_MODULE 255

class IPAddress {
public:
    std::string toString() const { return "192.168.1.100"; }
    operator std::string() const { return toString(); }
    operator String() const { return String(toString()); }
};

inline std::ostream& operator<<(std::ostream& os, const IPAddress& ip) {
    os << ip.toString();
    return os;
}

inline HardwareSerial Serial;

class WiFiClient {
private:
    bool _connected = false;
    std::queue<char> _rx_buffer;
public:
    std::shared_ptr<std::stringstream> tx_stream;

    WiFiClient() : _connected(false), tx_stream(std::make_shared<std::stringstream>()) {}
    WiFiClient(bool connected) : _connected(connected), tx_stream(std::make_shared<std::stringstream>()) {}

    operator bool() const { return _connected; }
    bool connected() const { return _connected; }
    
    void pushRequest(const std::string& req) {
        for (char c : req) {
            _rx_buffer.push(c);
        }
        _connected = true;
    }

    int available() const {
        return _rx_buffer.size();
    }

    char read() {
        if (_rx_buffer.empty()) return 0;
        char c = _rx_buffer.front();
        _rx_buffer.pop();
        return c;
    }

    template <typename T>
    void print(const T& val) {
        if (tx_stream) *tx_stream << val;
    }

    template <typename T>
    void println(const T& val) {
        if (tx_stream) *tx_stream << val << "\n";
    }

    void println() {
        if (tx_stream) *tx_stream << "\n";
    }

    void stop() {
        _connected = false;
    }
};

class WiFiServer {
private:
    uint16_t _port;
    bool _running;
    WiFiClient _pending_client;
    bool _has_client;

public:
    WiFiServer(uint16_t port) : _port(port), _running(false), _has_client(false) {}

    void begin() { _running = true; }

    void injectMockClient(const std::string& httpRequest) {
        _pending_client = WiFiClient();
        _pending_client.pushRequest(httpRequest);
        _has_client = true;
    }

    WiFiClient available() {
        if (_has_client) {
            _has_client = false;
            return _pending_client;
        }
        return WiFiClient(false);
    }

    WiFiClient& getPendingClient() {
        return _pending_client;
    }
};

class WiFiClass {
public:
    uint8_t status() { return WL_CONNECTED; }
    
    uint8_t begin(const char* ssid, const char* pass) {
        std::cout << "[Mock WiFi] Connected to SSID: " << ssid << "\n";
        return WL_CONNECTED;
    }

    IPAddress localIP() {
        return IPAddress();
    }
};

extern WiFiClass WiFi;

#endif // MOCK_WIFIS3_H
