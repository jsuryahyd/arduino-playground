#ifndef MOCK_SERVO_H
#define MOCK_SERVO_H

#include "Arduino.h"

class Servo {
private:
    int _pin;
    int _angle;
    bool _attached;

public:
    Servo() : _pin(-1), _angle(90), _attached(false) {}

    uint8_t attach(int pin) {
        _pin = pin;
        _attached = true;
        return 1;
    }

    uint8_t attach(int pin, int minMicros, int maxMicros) {
        (void)minMicros;
        (void)maxMicros;
        _pin = pin;
        _attached = true;
        return 1;
    }

    void detach() {
        _attached = false;
    }

    void write(int angle) {
        if (angle < 0) angle = 0;
        if (angle > 180) angle = 180;
        _angle = angle;
    }

    void writeMicroseconds(int us) {
        // Map 1000-2000us to 0-180deg approximately
        int deg = (us - 1000) * 180 / 1000;
        if (deg < 0) deg = 0;
        if (deg > 180) deg = 180;
        _angle = deg;
    }

    int read() const {
        return _angle;
    }

    bool attached() const {
        return _attached;
    }

    int getPin() const {
        return _pin;
    }
};

#endif // MOCK_SERVO_H
