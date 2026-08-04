#ifndef MOCK_ARDUINO_H
#define MOCK_ARDUINO_H

#include <iostream>
#include <string>
#include <sstream>
#include <chrono>
#include <thread>
#include <cstdint>
#include <cmath>
#include <cstring>
#include <algorithm>
#include <cctype>

typedef uint8_t byte;

// Flash memory helper macro mock
#ifndef F
#define F(string_literal) string_literal
#endif

#ifndef PROGMEM
#define PROGMEM
#endif

// Arduino map function mock
inline long map(long x, long in_min, long in_max, long out_min, long out_max) {
    if (in_max == in_min) return out_min;
    return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

// Mock millis timer
inline unsigned long millis() {
    static auto start_time = std::chrono::steady_clock::now();
    auto now = std::chrono::steady_clock::now();
    return std::chrono::duration_cast<std::chrono::milliseconds>(now - start_time).count();
}

inline void delay(unsigned long ms) {
    std::this_thread::sleep_for(std::chrono::milliseconds(ms > 50 ? 5 : ms));
}

// Mock Arduino String class
class String : public std::string {
public:
    String() : std::string() {}
    String(const char* s) : std::string(s ? s : "") {}
    String(const std::string& s) : std::string(s) {}
    String(long val) : std::string(std::to_string(val)) {}
    String(int val) : std::string(std::to_string(val)) {}
    String(unsigned int val) : std::string(std::to_string(val)) {}
    String(unsigned long val) : std::string(std::to_string(val)) {}

    void trim() {
        size_t first = find_first_not_of(" \t\r\n");
        if (first == std::string::npos) {
            clear();
            return;
        }
        size_t last = find_last_not_of(" \t\r\n");
        *this = substr(first, (last - first + 1));
    }

    void toLowerCase() {
        for (auto& c : *this) {
            c = std::tolower(static_cast<unsigned char>(c));
        }
    }

    bool startsWith(const String& prefix) const {
        return find(prefix, 0) == 0;
    }

    bool endsWith(const String& suffix) const {
        if (suffix.length() > length()) return false;
        return compare(length() - suffix.length(), suffix.length(), suffix) == 0;
    }

    int indexOf(const String& str, size_t from = 0) const {
        size_t pos = find(str, from);
        return (pos == std::string::npos) ? -1 : (int)pos;
    }

    int indexOf(const char* str, size_t from = 0) const {
        size_t pos = find(str ? str : "", from);
        return (pos == std::string::npos) ? -1 : (int)pos;
    }

    String substring(size_t from) const {
        if (from >= length()) return String("");
        return String(substr(from));
    }

    String substring(size_t from, size_t to) const {
        if (from >= length()) return String("");
        return String(substr(from, to - from));
    }

    int toInt() const {
        try {
            return std::stoi(*this);
        } catch (...) {
            return 0;
        }
    }

    String operator+(const String& other) const {
        return String(std::string(*this) + std::string(other));
    }
    String operator+(const char* other) const {
        return String(std::string(*this) + std::string(other ? other : ""));
    }
};

inline String operator+(const char* lhs, const String& rhs) {
    return String(std::string(lhs ? lhs : "") + std::string(rhs));
}

// Mock Serial Interface
class HardwareSerial {
private:
    std::string inputBuffer;

public:
    std::stringstream log_stream;

    void begin(unsigned long baud) {
        log_stream << "[Serial Begin @ " << baud << " baud]\n";
    }

    int available() {
        return !inputBuffer.empty() ? 1 : 0;
    }

    void injectInput(const std::string& input) {
        inputBuffer += input;
    }

    String readStringUntil(char terminator) {
        size_t pos = inputBuffer.find(terminator);
        if (pos != std::string::npos) {
            std::string res = inputBuffer.substr(0, pos);
            inputBuffer.erase(0, pos + 1);
            return String(res);
        } else {
            std::string res = inputBuffer;
            inputBuffer.clear();
            return String(res);
        }
    }

    template <typename T>
    void print(const T& val) {
        log_stream << val;
        std::cout << val;
    }

    template <typename T>
    void println(const T& val) {
        log_stream << val << "\n";
        std::cout << val << "\n";
    }

    void println() {
        log_stream << "\n";
        std::cout << "\n";
    }
};

extern HardwareSerial Serial;

#endif // MOCK_ARDUINO_H
