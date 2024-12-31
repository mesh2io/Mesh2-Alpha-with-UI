#ifndef MENU_H
#define MENU_H
#include <Arduino.h>

#include <string>
#include <vector>
// Global variable declarations
extern bool capsuleStates[];                 // Capsule states (ON/OFF)
extern std::string configureMenuValues[];    // Configurable item values

// Function declarations
void initMenu();                             // Initialize the menu system
void drawTransparentMenu();                  // Draw the main transparent menu
void drawConfigureMenu();                    // Draw the Configure menu
void handleMenuInput();                      // Handle input for the menu
void handleMainMenuInput();                  // Handle input for the main menu
void handleConfigureMenuInput();             // Handle input for the Configure menu
void handleCapsuleTouch(int x, int y);       // Handle touch events for capsule buttons
void returnToMainMenu();                     // Return to the main menu
void openSelectionMenu(int index);           // Open a selection menu for configurable items
void handleTextInput(int index);             // Handle user-enterable input (e.g., Callsign, Frequency)

// Function declarations
void loadInitialValues();
bool readFileAsBool(const char* path);
String loadFileAsString(const char* path);
std::vector<std::string> readLinesFromFile(const char* path);
void writeFile(const char* path, const char* value);

#endif // MENU_H
