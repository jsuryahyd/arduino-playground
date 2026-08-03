#ifndef MOCK_ARDUINO_LED_MATRIX_H
#define MOCK_ARDUINO_LED_MATRIX_H

#include "Arduino.h"

const uint32_t LEDMATRIX_CLOUD_WIFI[3] = { 0x18464841, 0x26549558, 0x54652188 };
const uint32_t LEDMATRIX_UNO[3]        = { 0x7fc404e0, 0x4e024024, 0xf24027fc };
const uint32_t LEDMATRIX_LIKE[3]       = { 0x1003005, 0x39c2842, 0x842843f8 };

class ArduinoLEDMatrix {
public:
    bool is_initialized = false;
    uint32_t last_frame[3] = {0};

    int begin() {
        is_initialized = true;
        return 1;
    }

    void loadFrame(const uint32_t frame[3]) {
        last_frame[0] = frame[0];
        last_frame[1] = frame[1];
        last_frame[2] = frame[2];
    }

    void loadPixels(const uint8_t* pixels, size_t count) {
        (void)pixels;
        (void)count;
    }

    void renderBitmap(const uint8_t bitmap[8][12], uint8_t rows, uint8_t cols) {
        (void)bitmap;
        (void)rows;
        (void)cols;
    }
};

#endif // MOCK_ARDUINO_LED_MATRIX_H
