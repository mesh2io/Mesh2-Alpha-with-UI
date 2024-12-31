#ifndef STORAGE_H
#define STORAGE_H

#include <Arduino.h>

// Initialize the SD card
bool initializeSD();

// Load a raw RGB565 image from the SD card into a buffer
bool loadImageFromSD(const char *path, uint16_t *buffer, size_t bufferSize);

#endif // STORAGE_H
