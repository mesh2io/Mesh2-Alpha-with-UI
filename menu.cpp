#include <LovyanGFX.hpp>
#include "TDeckConfig.h"
#include "PinDefs.h"
#include "files.h"
#include "ui.h"
#include "TouchDrvGT911.hpp"

#include "menu.h"

// Define capsuleStates and configureMenuValues
bool capsuleStates[] = {false, false, false, false, false};
std::string configureMenuValues[] = {"Not Set", "Not Set", "Not Set", "Not Set", "Not Set"};

const char* configureMenuItems[] = {"Country", "State", "Callsign", "Frequency"};


// Declare touch object as extern
extern TouchDrvGT911 touch;

// Main menu state
static bool menuVisible = false;
static bool configureMenuVisible = false;

// Capsule buttons
static const char* capsuleButtons[] = {"Private", "MQTT", "Encryption", "Auto Update", "Bluetooth"};
static int capsuleButtonCount = sizeof(capsuleButtons) / sizeof(capsuleButtons[0]);

// Configurable items
static int configureMenuItemCount = sizeof(configureMenuItems) / sizeof(configureMenuItems[0]);
static int configureMenuStartIndex = 0;

// Button positions
#define BACK_BUTTON_X 10
#define BACK_BUTTON_Y 220
#define NEXT_BUTTON_X 170
#define NEXT_BUTTON_Y 220
#define BUTTON_WIDTH 50
#define BUTTON_HEIGHT 30

void initMenu() {
    loadInitialValues(); // Load capsule and configuration values
}

void drawTransparentMenu() {
    // Draw background and main menu options
    drawJpgImage("/assets/background.jpg");

    // Main menu options
    const char* mainMenuItems[] = {"Configure", "Send", "Read"};
    int itemCount = sizeof(mainMenuItems) / sizeof(mainMenuItems[0]);
    for (int i = 0; i < itemCount; i++) {
        int x = 10 + (i * 100);
        display.drawRoundRect(x, 100, 90, 40, 5, TFT_WHITE);
        display.setCursor(x + 5, 115);
        display.print(mainMenuItems[i]);
    }
}
void drawCapsuleButton(int x, int y, const char* text, bool state) {
    int capsuleWidth = 90;  // Capsule width
    int capsuleHeight = 30; // Capsule height

    // Draw capsule background based on state
    if (state) {
        display.fillRoundRect(x, y, capsuleWidth, capsuleHeight, 15, TFT_GREEN);
    } else {
        display.fillRoundRect(x, y, capsuleWidth, capsuleHeight, 15, TFT_RED);
    }

    // Draw capsule border
    display.drawRoundRect(x, y, capsuleWidth, capsuleHeight, 15, TFT_WHITE);

    // Draw centered text
    int textX = x + (capsuleWidth / 2) - (strlen(text) * 3); // Adjust based on text length
    int textY = y + (capsuleHeight / 2) - 4;                // Adjust for vertical alignment
    display.setCursor(textX, textY);
    display.setTextColor(TFT_WHITE);
    display.print(text);
}

void drawConfigureMenu() {
    drawJpgImage("/assets/background.jpg"); // Draw background

    // Draw Exit button
    display.fillRoundRect(10, 10, 60, 25, 5, 0xFF0000); // Red for exit
    display.setCursor(15, 15);
    display.setTextColor(0xFFFFFF); // White text
    display.print("Exit");

    // Configurable items
    for (int i = configureMenuStartIndex; i < configureMenuStartIndex + 4; i++) {
        if (i >= configureMenuItemCount) break;

        int yPosition = 40 + ((i - configureMenuStartIndex) * 30);
        display.drawRoundRect(10, yPosition, 200, 25, 5, 0xFFFFFF);
        display.setCursor(15, yPosition + 5);
        display.printf("%s: %s", configureMenuItems[i], configureMenuValues[i].c_str());
    }

    // Capsule buttons
    for (int i = 0; i < capsuleButtonCount; i++) {
        int yPosition = 40 + (i * 53); // 50 px height + 3 px spacing
        drawCapsuleButton(210, yPosition, capsuleButtons[i], capsuleStates[i]);
    }

    // Back and Next buttons
    display.fillRoundRect(BACK_BUTTON_X, BACK_BUTTON_Y, BUTTON_WIDTH, BUTTON_HEIGHT, 5, 0x808080); // Gray
    display.setCursor(BACK_BUTTON_X + 5, BACK_BUTTON_Y + 5);
    display.print("Back");

    display.fillRoundRect(NEXT_BUTTON_X, NEXT_BUTTON_Y, BUTTON_WIDTH, BUTTON_HEIGHT, 5, 0x808080); // Gray
    display.setCursor(NEXT_BUTTON_X + 5, NEXT_BUTTON_Y + 5);
    display.print("Next");
}


void handleMenuInput() {
    if (menuVisible) {
        handleMainMenuInput();
    } else if (configureMenuVisible) {
        handleConfigureMenuInput();
    } else if (digitalRead(TRACKBALL_BUTTON_PIN) == LOW) {
        delay(200); // Debounce delay
        menuVisible = true;
        drawTransparentMenu();
    }
}

void handleMainMenuInput() {
    if (touch.isPressed()) {
        int16_t x[5], y[5];
        uint8_t touched = touch.getPoint(x, y, touch.getSupportTouchPoint());

        if (touched > 0) {
            for (int i = 0; i < touched; ++i) {
                if (x[i] >= 10 && x[i] <= 100 && y[i] >= 100 && y[i] <= 140) {
                    menuVisible = false;
                    configureMenuVisible = true;
                    drawConfigureMenu();
                    return;
                }
            }
        }
    }
}

void handleConfigureMenuInput() {
    if (touch.isPressed()) {
        int16_t x[5], y[5];
        uint8_t touched = touch.getPoint(x, y, touch.getSupportTouchPoint());

        if (touched > 0) {
            for (int i = 0; i < touched; ++i) {
                // Exit button
                if (x[i] >= 10 && x[i] <= 70 && y[i] >= 10 && y[i] <= 35) {
                    returnToMainMenu();
                    return;
                }

                // Capsule buttons
                handleCapsuleTouch(x[i], y[i]);

                // Configurable items
                for (int j = configureMenuStartIndex; j < configureMenuStartIndex + 5; j++) {
                    if (j >= configureMenuItemCount) break;

                    int optionY = 40 + ((j - configureMenuStartIndex) * 30);
                    if (x[i] >= 10 && x[i] <= 210 && y[i] >= optionY && y[i] <= optionY + 25) {
                        openSelectionMenu(j); // Open the selection menu
                        return;
                    }
                }
            }
        }
    }
}


void handleCapsuleTouch(int x, int y) {
    for (int i = 0; i < capsuleButtonCount; i++) {
        int capsuleX = 210;
        int capsuleY = 40 + (i * 50);
        if (x >= capsuleX && x <= capsuleX + 90 && y >= capsuleY && y <= capsuleY + 30) {
            capsuleStates[i] = !capsuleStates[i];
            writeFile((std::string("/") + capsuleButtons[i] + ".txt").c_str(), capsuleStates[i] ? "1" : "0");
            drawConfigureMenu();
            return;
        }
    }
}

void returnToMainMenu() {
    configureMenuVisible = false;
    menuVisible = true;
    drawTransparentMenu();
}

#include <algorithm> // For std::transform
void openSelectionMenu(int index) {
    // Validate the index
    if (index < 0 || index >= configureMenuItemCount) {
        Serial.println("Invalid index for selection menu.");
        return;
    }

    // Check if the current option is user-enterable (Callsign or Frequency)
    bool isUserInput = (strcmp(configureMenuItems[index], "Callsign") == 0 ||
                        strcmp(configureMenuItems[index], "Frequency") == 0);

    if (isUserInput) {
        // Handle user input for Callsign or Frequency
        handleTextInput(index);
        return;
    }

    // For non-user-enterable options, proceed with normal selection logic
    std::string fileName = configureMenuItems[index];
    std::transform(fileName.begin(), fileName.end(), fileName.begin(), ::tolower);
    std::string filePath = "/" + fileName + ".txt";

    std::vector<std::string> options = readLinesFromFile(filePath.c_str());
    if (options.empty()) {
        Serial.printf("No options available in file: %s\n", filePath.c_str());
        return;
    }

    int selectedOption = 0;
    int startIndex = 0;
    bool needsRedraw = true;

    while (true) {
        if (needsRedraw) {
            display.fillScreen(0x000000);
            display.setTextColor(0xFFFFFF);

            for (int i = startIndex; i < startIndex + 5 && i < options.size(); i++) {
                int yPosition = 40 + ((i - startIndex) * 30);
                display.drawRoundRect(10, yPosition, 200, 25, 5, 0xFFFFFF);
                display.setCursor(15, yPosition + 5);
                display.print(options[i].c_str());
            }

            display.fillRoundRect(BACK_BUTTON_X, BACK_BUTTON_Y, BUTTON_WIDTH, BUTTON_HEIGHT, 5, 0x808080);
            display.setCursor(BACK_BUTTON_X + 5, BACK_BUTTON_Y + 5);
            display.print("Back");

            display.fillRoundRect(NEXT_BUTTON_X, NEXT_BUTTON_Y, BUTTON_WIDTH, BUTTON_HEIGHT, 5, 0x808080);
            display.setCursor(NEXT_BUTTON_X + 5, NEXT_BUTTON_Y + 5);
            display.print("Next");

            needsRedraw = false;
        }

        if (touch.isPressed()) {
            int16_t x[5], y[5];
            uint8_t touched = touch.getPoint(x, y, touch.getSupportTouchPoint());

            for (int i = 0; i < touched; ++i) {
                if (x[i] >= BACK_BUTTON_X && x[i] <= BACK_BUTTON_X + BUTTON_WIDTH &&
                    y[i] >= BACK_BUTTON_Y && y[i] <= BACK_BUTTON_Y + BUTTON_HEIGHT && startIndex > 0) {
                    startIndex -= 5;
                    needsRedraw = true;
                    delay(200);
                    break;
                }

                if (x[i] >= NEXT_BUTTON_X && x[i] <= NEXT_BUTTON_X + BUTTON_WIDTH &&
                    y[i] >= NEXT_BUTTON_Y && y[i] <= NEXT_BUTTON_Y + BUTTON_HEIGHT && startIndex + 5 < options.size()) {
                    startIndex += 5;
                    needsRedraw = true;
                    delay(200);
                    break;
                }

                for (int j = startIndex; j < startIndex + 5 && j < options.size(); j++) {
                    int yPosition = 40 + ((j - startIndex) * 30);
                    if (x[i] >= 10 && x[i] <= 210 && y[i] >= yPosition && y[i] <= yPosition + 25) {
                        selectedOption = j;
                        configureMenuValues[index] = options[selectedOption];
                        std::string userFilePath = "/user_" + fileName + ".txt";
                        writeFile(userFilePath.c_str(), options[selectedOption].c_str());
                        drawConfigureMenu();
                        delay(200);
                        return;
                    }
                }
            }
        }
    }
}
void handleTextInput(int index) {
    std::string currentInput = configureMenuValues[index];
    bool needsRedraw = true;

    while (true) {
        if (needsRedraw) {
            display.fillScreen(0x000000);
            display.setTextColor(0xFFFFFF);

            // Display title and current input
            display.setCursor(10, 40);
            display.printf("Enter %s:", configureMenuItems[index]);

            display.drawRoundRect(10, 80, 220, 25, 5, 0xFFFFFF);
            display.setCursor(15, 85);
            display.print(currentInput.c_str());

            // Done button
            display.fillRoundRect(80, 120, 80, 30, 5, 0x00FF00);
            display.setCursor(100, 130);
            display.print("Done");

            needsRedraw = false;
        }

        if (touch.isPressed()) {
            int16_t x[5], y[5];
            uint8_t touched = touch.getPoint(x, y, touch.getSupportTouchPoint());

            for (int i = 0; i < touched; ++i) {
                // Handle Done button
                if (x[i] >= 80 && x[i] <= 160 && y[i] >= 120 && y[i] <= 150) {
                    std::string userFilePath = "/user_" + std::string(configureMenuItems[index]) + ".txt";
                    writeFile(userFilePath.c_str(), currentInput.c_str());
                    configureMenuValues[index] = currentInput;
                    drawConfigureMenu();
                    delay(200);
                    return;
                }
            }

            // Capture keyboard input here (mocked for simplicity)
            // Update `currentInput` based on user entry
        }
    }
}


