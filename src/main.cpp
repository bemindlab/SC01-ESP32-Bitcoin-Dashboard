#include <Arduino.h>
#include <WiFi.h>
#include <FT6X36.h>
#include "DisplayConfig.h"
#include "screens/ScreenManager.h"

LGFX lcd;
FT6X36 touch(&Wire, 7);  // INT pin = GPIO 7
ScreenManager* screenManager;

//  WiFi credentials (if stored)
String storedSSID = "";
String storedPassword = "";

// ==================== Setup ====================
void setup() {
    Serial.begin(115200);
    Serial.println("\n\nBitcoin Dashboard - Bootstrap Mode");

    // Initialize display
    lcd.init();
    lcd.setRotation(1);  // Landscape
    lcd.setBrightness(255);
    Serial.println("Display initialized!");

    // Initialize touch (SDA=6, SCL=5)
    Wire.begin(6, 5);
    if (touch.begin(40)) {
        Serial.println("Touch initialized successfully!");
    } else {
        Serial.println("ERROR: Touch initialization failed!");
    }

    // Test touch
    Serial.println("Testing touch detection...");
    for (int i = 0; i < 5; i++) {
        uint8_t touches = touch.touched();
        Serial.printf("Touch poll %d: %d touches\n", i, touches);
        delay(100);
    }

    // Create screen manager
    screenManager = new ScreenManager(&lcd, &touch);

    // Check if WiFi credentials are stored (TODO: EEPROM/Preferences)
    // For now, always show WiFi scan
    bool hasStoredWiFi = false;

    if (hasStoredWiFi && WiFi.status() == WL_CONNECTED) {
        // Go directly to dashboard
        Serial.println("Using stored WiFi, going to dashboard...");
        screenManager->switchScreen(SCREEN_DASHBOARD);
    } else {
        // Show WiFi scan screen
        Serial.println("Starting WiFi scan screen...");
        screenManager->switchScreen(SCREEN_WIFI_SCAN);
    }
}

// ==================== Main Loop ====================
unsigned long lastTouchDebug = 0;

void loop() {
    // Debug: Print touch status every 2 seconds
    if (millis() - lastTouchDebug >= 2000) {
        uint8_t touches = touch.touched();
        if (touches > 0) {
            Serial.printf("[DEBUG] Touch detected: %d\n", touches);
        }
        lastTouchDebug = millis();
    }

    // Update current screen (includes touch processing)
    screenManager->update();

    delay(10);
}
