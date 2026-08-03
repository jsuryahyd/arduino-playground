/*
  Arduino UNO R4 WiFi - Official LED Matrix Icons & Animations
  
  Your Uno R4 WiFi has a 12x8 LED matrix built-in.
  This sketch uses official gallery icons, built-in animations, and
  a 2D pixel array so you can draw your own shapes with 1s and 0s!
*/

#include "Arduino_LED_Matrix.h"

// Create matrix instance
ArduinoLEDMatrix matrix;

// -------------------------------------------------------------
// 1. CUSTOM 2D BITMAP MATRIX (8 Rows x 12 Columns)
//    1 = LED ON, 0 = LED OFF
// -------------------------------------------------------------
uint8_t custom_smiley[8][12] = {
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 1, 1, 0, 0, 0, 0, 1, 1, 0, 0 }, // Eyes
  { 0, 0, 1, 1, 0, 0, 0, 0, 1, 1, 0, 0 }, // Eyes
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0 }, // Mouth corners
  { 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0 }, // Smile curve
  { 0, 0, 0, 1, 1, 1, 1, 1, 1, 0, 0, 0 }, // Smile bottom
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }
};

uint8_t custom_heart[8][12] = {
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 1, 1, 0, 0, 0, 1, 1, 0, 0, 0 },
  { 0, 1, 1, 1, 1, 0, 1, 1, 1, 1, 0, 0 },
  { 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0 },
  { 0, 0, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0 },
  { 0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0 }
};

void setup() {
  Serial.begin(115200);
  matrix.begin();
  
  Serial.println("=========================================");
  Serial.println("🚀 Arduino UNO R4 WiFi LED Matrix Running");
  Serial.println("=========================================");
}

void loop() {
  // --- 1. Custom 8x12 Grid Smiley ---
  Serial.println("--> Displaying Custom 8x12 Smiley");
  matrix.loadPixels((uint8_t*)custom_smiley, 96);
  delay(1500);

  // --- 2. Custom 8x12 Grid Heart ---
  Serial.println("--> Displaying Custom 8x12 Heart");
  matrix.loadPixels((uint8_t*)custom_heart, 96);
  delay(1500);

  // --- 3. Official Gallery Icons ---
  Serial.println("--> Displaying Official Arduino Logo");
  matrix.loadFrame(LEDMATRIX_UNO);
  delay(1500);

  Serial.println("--> Displaying Official Wi-Fi Icon");
  matrix.loadFrame(LEDMATRIX_CLOUD_WIFI);
  delay(1500);

  Serial.println("--> Displaying Official Thumbs Up / Like");
  matrix.loadFrame(LEDMATRIX_LIKE);
  delay(1500);

  // --- 4. Official Built-in Bouncing Ball Animation ---
  Serial.println("--> Playing Bouncing Ball Animation");
  matrix.loadSequence(LEDMATRIX_ANIMATION_BOUNCING_BALL);
  matrix.play(false); // Play animation once
  delay(2500);
}
