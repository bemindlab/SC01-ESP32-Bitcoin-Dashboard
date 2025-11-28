#include "SettingsScreen.h"

void SettingsScreen::init(ScreenManager* mgr) {
    manager = mgr;
    selectedOption = 0;
    editingKey = false;
    tempApiKey = globalConfig.getGeminiApiKey();

    // Initialize touch feedback
    feedback.init(manager->getLCD());

    // Clear screen
    LGFX* lcd = manager->getLCD();
    lcd->fillScreen(COLOR_BG);

    // Draw initial UI
    drawHeader();
    drawOptions();

    // Register back button feedback
    backButtonFeedbackId = feedback.registerIcon(
        BACK_BTN_X, BACK_BTN_Y, 30,
        COLOR_HEADER_BG, COLOR_BTC_ORANGE, 200
    );

    // Register option list items
    int startY = HEADER_HEIGHT + OPTION_PADDING;
    for (int i = 0; i < OPTION_COUNT; i++) {
        int y = startY + (i * (OPTION_HEIGHT + OPTION_PADDING));
        optionFeedbackIds[i] = feedback.registerListItem(
            OPTION_PADDING, y,
            480 - (OPTION_PADDING * 2), OPTION_HEIGHT,
            0x1A1A1A, 0x0066FF, COLOR_BTC_ORANGE
        );
    }

    Serial.println("Settings Screen initialized");
}

void SettingsScreen::update() {
    // Update touch feedback animations (non-blocking)
    feedback.update();

    // Settings screen is mostly static, updated on touch
}

void SettingsScreen::handleTouch(int16_t x, int16_t y) {
    Serial.printf("SettingsScreen touch: (%d, %d)\n", x, y);

    // Back button (top-left)
    if (x >= BACK_BTN_X && x <= BACK_BTN_X + 30 &&
        y >= BACK_BTN_Y && y <= BACK_BTN_Y + 30) {
        Serial.println("Back button tapped - returning to dashboard");

        // Visual feedback - non-blocking flash
        feedback.flash(backButtonFeedbackId);

        manager->switchScreen(SCREEN_DASHBOARD);
        return;
    }

    // If editing key, handle keyboard input (not implemented yet)
    if (editingKey) {
        // TODO: Implement on-screen keyboard
        Serial.println("Key editing - keyboard input needed");
        return;
    }

    // Check if touch is on an option
    int startY = HEADER_HEIGHT + OPTION_PADDING;
    for (int i = 0; i < OPTION_COUNT; i++) {
        int optionY = startY + (i * (OPTION_HEIGHT + OPTION_PADDING));

        if (y >= optionY && y <= optionY + OPTION_HEIGHT) {
            selectedOption = i;

            // Visual feedback - highlight selected option
            feedback.onTouchDown(optionFeedbackIds[i]);
            delay(100);  // Brief delay to show selection
            feedback.onTouchUp(optionFeedbackIds[i]);

            handleOptionSelect(i);
            break;
        }
    }
}

void SettingsScreen::handleOptionSelect(int option) {
    LGFX* lcd = manager->getLCD();

    switch (option) {
        case OPTION_GEMINI_KEY:
            Serial.println("Edit Gemini API Key selected");
            editGeminiKey();
            break;

        case OPTION_WIFI_SETTINGS:
            Serial.println("WiFi Settings selected");
#ifdef SINGLE_SCREEN_MODE
            Serial.println("SINGLE_SCREEN_MODE: WiFi screen not available");
            Serial.println("Use serial command: SET_WIFI=SSID,Password");
#else
            // Navigate to WiFi scan screen
            manager->switchScreen(SCREEN_WIFI_SCAN);
#endif
            break;

        case OPTION_INTERVALS:
            Serial.println("Update Intervals selected");
            // TODO: Implement interval editor
            lcd->fillScreen(COLOR_BG);
            drawHeader();
            lcd->setTextColor(COLOR_TEXT_LIGHT);
            lcd->setFont(&fonts::FreeSans12pt7b);
            lcd->setCursor(100, 150);
            lcd->print("Coming soon!");
            delay(1000);
            lcd->fillScreen(COLOR_BG);
            drawHeader();
            drawOptions();
            break;

        case OPTION_RESET_CONFIG:
            Serial.println("Reset Configuration selected");
            resetConfiguration();
            break;

        case OPTION_BACK:
            Serial.println("Back selected");
            manager->switchScreen(SCREEN_DASHBOARD);
            break;
    }
}

void SettingsScreen::editGeminiKey() {
    LGFX* lcd = manager->getLCD();

    // Clear screen
    lcd->fillScreen(COLOR_BG);
    drawHeader();

    // Show instructions
    lcd->setTextColor(COLOR_TEXT_LIGHT);
    lcd->setFont(&fonts::FreeSans9pt7b);

    lcd->setCursor(20, 80);
    lcd->print("Gemini API Key Configuration");

    lcd->setTextColor(COLOR_TEXT_DIM);
    lcd->setFont(&fonts::FreeSans9pt7b);
    lcd->setCursor(20, 120);
    lcd->print("Current key:");

    // Show masked key or "Not Set"
    String currentKey = globalConfig.getGeminiApiKey();
    if (currentKey.length() > 8) {
        String masked = currentKey.substring(0, 4) + "..." +
                       currentKey.substring(currentKey.length() - 4);
        lcd->setTextColor(COLOR_SUCCESS);
        lcd->setCursor(20, 145);
        lcd->print(masked);
    } else {
        lcd->setTextColor(COLOR_ERROR);
        lcd->setCursor(20, 145);
        lcd->print("NOT SET");
    }

    // Instructions
    lcd->setTextColor(COLOR_TEXT_LIGHT);
    lcd->setFont(&fonts::FreeSans9pt7b);
    lcd->setCursor(20, 190);
    lcd->print("To set your API key:");

    lcd->setTextColor(COLOR_TEXT_DIM);
    lcd->setFont(&fonts::FreeSans9pt7b);
    lcd->setCursor(20, 215);
    lcd->print("1. Edit src/Config.cpp");
    lcd->setCursor(20, 235);
    lcd->print("2. Or use Serial command:");
    lcd->setCursor(20, 255);
    lcd->print("   SET_GEMINI_KEY=your-key");

    // Alternative: Show QR code for API key URL
    lcd->setTextColor(COLOR_BTC_ORANGE);
    lcd->setFont(&fonts::FreeSansBold9pt7b);
    lcd->setCursor(20, 285);
    lcd->print("Get key at:");
    lcd->setTextColor(COLOR_TEXT_LIGHT);
    lcd->setFont(&fonts::FreeSans9pt7b);
    lcd->setCursor(20, 305);
    lcd->print("makersuite.google.com");

    Serial.println("\n=== Gemini API Key Setup ===");
    Serial.print("Current key: ");
    Serial.println(currentKey.length() > 0 ? "SET" : "NOT SET");
    Serial.println("\nTo configure via Serial Monitor, send:");
    Serial.println("SET_GEMINI_KEY=your-actual-api-key-here");
    Serial.println("\nPress any key to return...");

    delay(5000);

    // Return to settings menu
    lcd->fillScreen(COLOR_BG);
    drawHeader();
    drawOptions();
}

void SettingsScreen::resetConfiguration() {
    LGFX* lcd = manager->getLCD();

    // Show confirmation
    lcd->fillScreen(COLOR_BG);
    drawHeader();

    lcd->setTextColor(COLOR_ERROR);
    lcd->setFont(&fonts::FreeSansBold12pt7b);
    lcd->setCursor(80, 120);
    lcd->print("Reset Config?");

    lcd->setTextColor(COLOR_TEXT_LIGHT);
    lcd->setFont(&fonts::FreeSans9pt7b);
    lcd->setCursor(60, 160);
    lcd->print("This will erase all");
    lcd->setCursor(60, 180);
    lcd->print("saved settings!");

    lcd->setTextColor(COLOR_TEXT_DIM);
    lcd->setCursor(80, 220);
    lcd->print("Resetting in 3...");

    Serial.println("\n=== RESETTING CONFIGURATION ===");
    delay(1000);

    lcd->fillRect(0, 210, 480, 30, COLOR_BG);
    lcd->setTextColor(COLOR_TEXT_DIM);
    lcd->setCursor(80, 220);
    lcd->print("Resetting in 2...");
    delay(1000);

    lcd->fillRect(0, 210, 480, 30, COLOR_BG);
    lcd->setTextColor(COLOR_TEXT_DIM);
    lcd->setCursor(80, 220);
    lcd->print("Resetting in 1...");
    delay(1000);

    // Perform reset
    globalConfig.reset();
    globalConfig.save();

    lcd->fillRect(0, 210, 480, 30, COLOR_BG);
    lcd->setTextColor(COLOR_SUCCESS);
    lcd->setCursor(60, 220);
    lcd->print("Configuration Reset!");

    Serial.println("Configuration reset complete");
    delay(2000);

    // Return to settings
    lcd->fillScreen(COLOR_BG);
    drawHeader();
    drawOptions();
}

void SettingsScreen::drawHeader() {
    LGFX* lcd = manager->getLCD();

    // Header background
    lcd->fillRect(0, 0, 480, HEADER_HEIGHT, COLOR_HEADER_BG);

    // Title
    lcd->setTextColor(COLOR_BTC_ORANGE);
    lcd->setFont(&fonts::FreeSansBold12pt7b);
    lcd->setCursor(50, 25);
    lcd->print("Settings");

    // Draw back button
    drawBackButton();

    // Bottom border
    lcd->drawLine(0, HEADER_HEIGHT, 480, HEADER_HEIGHT, COLOR_BTC_ORANGE);
}

void SettingsScreen::drawBackButton() {
    LGFX* lcd = manager->getLCD();

    // Draw back arrow "<"
    lcd->setTextColor(COLOR_TEXT_WHITE);
    lcd->setFont(&fonts::FreeSansBold12pt7b);
    lcd->setCursor(BACK_BTN_X, BACK_BTN_Y + 20);
    lcd->print("<");
}

void SettingsScreen::drawOptions() {
    int startY = HEADER_HEIGHT + OPTION_PADDING;

    // Option labels
    const char* labels[] = {
        "Gemini API Key",
        "WiFi Settings",
        "Update Intervals",
        "Reset Configuration",
        "Back to Dashboard"
    };

    // Option values (status)
    String values[OPTION_COUNT];
    values[OPTION_GEMINI_KEY] = globalConfig.hasGeminiKey() ? "SET" : "NOT SET";
    values[OPTION_WIFI_SETTINGS] = globalConfig.hasWiFiCredentials() ? "CONFIGURED" : "SETUP";
    values[OPTION_INTERVALS] = "EDIT";
    values[OPTION_RESET_CONFIG] = "DANGER";
    values[OPTION_BACK] = "";

    for (int i = 0; i < OPTION_COUNT; i++) {
        drawOption(i, labels[i], values[i].c_str(), i == selectedOption);
    }

    // Footer hint
    LGFX* lcd = manager->getLCD();
    lcd->setTextColor(COLOR_TEXT_DIM);
    lcd->setFont(&fonts::FreeSans9pt7b);
    lcd->setCursor(150, 310);
    lcd->print("Tap option to select");
}

void SettingsScreen::drawOption(int index, const char* label, const char* value, bool isSelected) {
    LGFX* lcd = manager->getLCD();

    int y = HEADER_HEIGHT + OPTION_PADDING + (index * (OPTION_HEIGHT + OPTION_PADDING));

    // Background
    uint32_t bgColor = isSelected ? COLOR_SELECTED : 0x1A1A1A;
    lcd->fillRoundRect(OPTION_PADDING, y, 480 - (OPTION_PADDING * 2), OPTION_HEIGHT, 5, bgColor);

    // Label
    lcd->setTextColor(COLOR_TEXT_WHITE);
    lcd->setFont(&fonts::FreeSansBold9pt7b);
    lcd->setCursor(OPTION_PADDING + 10, y + 25);
    lcd->print(label);

    // Value/Status
    if (strlen(value) > 0) {
        uint32_t valueColor = COLOR_TEXT_DIM;
        if (strcmp(value, "SET") == 0 || strcmp(value, "CONFIGURED") == 0) {
            valueColor = COLOR_SUCCESS;
        } else if (strcmp(value, "NOT SET") == 0 || strcmp(value, "SETUP") == 0) {
            valueColor = COLOR_ERROR;
        } else if (strcmp(value, "DANGER") == 0) {
            valueColor = COLOR_ERROR;
        }

        lcd->setTextColor(valueColor);
        lcd->setFont(&fonts::FreeSans9pt7b);
        lcd->setCursor(OPTION_PADDING + 10, y + 48);
        lcd->print(value);
    }
}
