#include "files.h"
#include <Arduino.h>
#include <SD.h>
#include <vector>
#include <string>
#include "menu.h"

extern bool capsuleStates[];
extern std::string configureMenuValues[];

// Load initial values for capsules and configurable items
void loadInitialValues() {
    // Load capsule states
    capsuleStates[0] = readFileAsBool("/private.txt");
    capsuleStates[1] = readFileAsBool("/mqtt.txt");
    capsuleStates[2] = readFileAsBool("/encryption.txt");
    capsuleStates[3] = readFileAsBool("/update.txt");
    capsuleStates[4] = readFileAsBool("/bluetooth.txt");

    // Load configurable item values
    configureMenuValues[0] = std::string(loadFileAsString("/country.txt").c_str());
    configureMenuValues[1] = std::string(loadFileAsString("/state.txt").c_str());
    configureMenuValues[2] = std::string(loadFileAsString("/city.txt").c_str());
    configureMenuValues[3] = std::string(loadFileAsString("/callsign.txt").c_str());
    configureMenuValues[4] = std::string(loadFileAsString("/frequency.txt").c_str());

    Serial.println("Initial values loaded:");
    for (int i = 0; i < 5; i++) { // Assuming 5 capsule buttons
        Serial.printf("Capsule %d: %s\n", i, capsuleStates[i] ? "ON" : "OFF");
    }
    for (int i = 0; i < 5; i++) { // Assuming 5 configurable items
        Serial.printf("Configurable Item %d: %s\n", i, configureMenuValues[i].c_str());
    }
}

// Read a file as a boolean
bool readFileAsBool(const char* path) {
    String value = loadFileAsString(path);
    return value == "1";
}

// Read a file as a String
String loadFileAsString(const char* path) {
    File file = SD.open(path, FILE_READ);
    if (!file) {
        Serial.printf("Failed to open file: %s\n", path);
        return "Not Set";
    }
    String value = file.readStringUntil('\n');
    file.close();
    return value;
}

std::vector<std::string> readLinesFromFile(const char* path) {
    std::vector<std::string> lines;

    File file = SD.open(path, FILE_READ);
    if (!file) {
        Serial.printf("Failed to open file: %s\n", path);
        return lines; // Return empty vector
    }

    while (file.available()) {
        String line = file.readStringUntil('\n');
        line.trim(); // Remove any trailing newline or whitespace
        if (line.length() > 0) {
            lines.push_back(std::string(line.c_str())); // Convert to std::string
        }
    }

    file.close();
    return lines;
}


// Write a string to a file
void writeFile(const char* path, const char* value) {
    File file = SD.open(path, FILE_WRITE);
    if (!file) {
        Serial.printf("Failed to open file for writing: %s\n", path);
        return;
    }
    file.println(value);
    file.close();
    Serial.printf("Updated file: %s with value: %s\n", path, value);
}
