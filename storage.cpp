#include <SD.h>
#include <SPI.h>
#include "PinDefs.h"

// Initialize the SD card
bool initializeSD() {
    Serial.println("Initializing SD card...");
    
    if (!SD.begin(SD_CS_PIN)) {
        Serial.println("SD card initialization failed! Check wiring and format.");
        return false;
    }

    if (!SD.exists("/")) {
        Serial.println("SD card initialized but no filesystem found.");
        return false;
    }

    Serial.println("SD card initialized successfully.");
    return true;
}

// Load a raw RGB565 image from the SD card into a buffer
bool loadImageFromSD(const char *path, uint16_t *buffer, size_t bufferSize) {
    Serial.printf("Loading image from SD card: %s\n", path);

    File file = SD.open(path, FILE_READ);
    if (!file) {
        Serial.println("Failed to open file on SD card.");
        return false;
    }

    size_t bytesRead = file.read(reinterpret_cast<uint8_t *>(buffer), bufferSize * 2); // Multiply buffer size by 2 (16 bits per pixel)
    file.close();

    if (bytesRead != bufferSize * 2) {
        Serial.println("Error reading image file or buffer size mismatch.");
        return false;
    }

    Serial.println("Image loaded successfully from SD card.");
    return true;
}
