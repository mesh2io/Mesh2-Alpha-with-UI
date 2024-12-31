#include <LovyanGFX.hpp>
#include "storage.h"
#include "TDeckConfig.h"
#include <TJpg_Decoder.h>
#include "ui.h"
#include "PinDefs.h"
#include <TouchDrvGT911.hpp>
#include "files.h"


#include "menu.h" // New menu system

LGFX_S3TDeck display;
TouchDrvGT911 touch;
int16_t x[5], y[5]; // To store touch coordinates

void setup() {
    delay(3000);
    Serial.begin(115200);

    // Initialize the display
    display.init();
    display.setRotation(1);
    display.setColorDepth(24);

    // Initialize the SD card
    if (!initializeSD()) {
        Serial.println("SD Card Initialization Failed!");
        return;
    }

    // Initialize the touch INT pin
    pinMode(TOUCH_INT_PIN, INPUT);

    // Trackball Button
    pinMode(TRACKBALL_BUTTON_PIN, INPUT);

    // Enable board peripherals
    pinMode(BOARD_POWERON, OUTPUT);
    digitalWrite(BOARD_POWERON, HIGH);

    // Initialize I2C for touch
    Wire.begin(TOUCH_SDA_PIN, TOUCH_SCL_PIN);

    // Set the INT pin for touch
    touch.setPins(-1, TOUCH_INT_PIN);

    // Initialize the GT911 sensor
    if (!touch.begin(Wire, GT911_SLAVE_ADDRESS_L)) {
        Serial.println("Failed to find GT911 - check your wiring!");
        while (true) {
            delay(1000);
        }
    }

    Serial.println("GT911 Initialized!");

    loadInitialValues();

    // Set maximum screen dimensions
    touch.setMaxCoordinates(320, 240);

    // Configure touch orientation
    touch.setSwapXY(true);
    touch.setMirrorXY(false, true);

    // Initialize JPEG decoder
    initJpegDecoder();

    // Draw the background image
    drawJpgImage("/assets/background.jpg");

    // Initialize the menu system
    initMenu();
}

void loop() {
    // Check if the menu is triggered by the trackball button
    handleMenuInput();

    if (touch.isPressed()) {
        Serial.println("Screen Pressed!");
        uint8_t touched = touch.getPoint(x, y, touch.getSupportTouchPoint());

        if (touched > 0) {
            for (int i = 0; i < touched; ++i) {
                Serial.printf("Touch Point %d: X=%d, Y=%d\n", i, x[i], y[i]);
            }
        }
    }

    delay(100); // Poll touch at intervals
}
