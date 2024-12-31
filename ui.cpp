#include "ui.h"
#include "TDeckConfig.h"
#include <LovyanGFX.hpp>
#include "storage.h"
// Extern the display object
extern LGFX_S3TDeck display;

// Callback for drawing pixels
bool jpgDrawCallback(int16_t x, int16_t y, uint16_t w, uint16_t h, uint16_t *bitmap) {
  extern LGFX_S3TDeck display;  // Ensure this matches your display object name
  display.pushImage(x, y, w, h, bitmap);
  return true;
}


// Initialize the JPEG decoder
void initJpegDecoder() {
  TJpgDec.setCallback(jpgDrawCallback);  // Use the corrected callback
  TJpgDec.setSwapBytes(true);            // Correct byte order for RGB565
  TJpgDec.setJpgScale(1);                // Use full resolution
}

// Function to draw a JPEG image from the SD card
void drawJpgImage(const char *path) {
  if (!TJpgDec.drawFsJpg(0, 0, path, SD)) {  // Draw the image at (0, 0) from the SD card
    Serial.println("Failed to draw JPEG image.");
  }
}
