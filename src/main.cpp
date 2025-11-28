#include <Arduino.h>
#include <WiFi.h>
#include <FT6X36.h>
#include "DisplayConfig.h"
#include "screens/ScreenManager.h"
#include "Config.h"

LGFX lcd;
FT6X36 touch(&Wire, 7);  // INT pin = GPIO 7
ScreenManager* screenManager;

// Screenshot buffer size (480x320 RGB565 = 307,200 bytes)
#define SCREENSHOT_BUFFER_SIZE (480 * 320 * 2)

// Screenshot function
void sendScreenshot() {
    Serial.println("\nSCREENSHOT_START");

    // Read pixel by pixel (LGFX doesn't always have direct buffer access)
    for (int y = 0; y < 320; y++) {
        for (int x = 0; x < 480; x++) {
            uint16_t pixel = lcd.readPixel(x, y);
            Serial.write((uint8_t*)&pixel, 2);
        }
        // Brief delay every 10 lines to avoid buffer overflow
        if (y % 10 == 0) {
            delay(1);
        }
    }

    Serial.println("\nSCREENSHOT_END");
    Serial.flush();
}

// Process serial commands
void processSerialCommand() {
    if (Serial.available() > 0) {
        String command = Serial.readStringUntil('\n');
        command.trim();

        if (command == "SCREENSHOT") {
            Serial.println("Screenshot command received!");
            sendScreenshot();
        } else if (command == "STATUS") {
            Serial.printf("WiFi: %s\n", WiFi.status() == WL_CONNECTED ? "Connected" : "Disconnected");
            Serial.printf("Free Heap: %d bytes\n", ESP.getFreeHeap());
            Serial.printf("Uptime: %lu seconds\n", millis() / 1000);
            globalConfig.printConfig();
        } else if (command == "HELP") {
            Serial.println("\nAvailable commands:");
            Serial.println("  SCREENSHOT         - Capture display buffer");
            Serial.println("  STATUS             - Show device status");
            Serial.println("  SET_GEMINI_KEY=xxx - Set Gemini API key");
            Serial.println("  RESET_CONFIG       - Reset all configuration");
            Serial.println("  HELP               - Show this help");
        } else if (command.startsWith("SET_GEMINI_KEY=")) {
            String key = command.substring(15);
            key.trim();
            if (key.length() > 0) {
                globalConfig.setGeminiApiKey(key);
                globalConfig.setFirstRun(false);
                if (globalConfig.save()) {
                    Serial.println("✓ Gemini API key saved successfully!");
                } else {
                    Serial.println("✗ Failed to save API key");
                }
            } else {
                Serial.println("✗ Invalid API key (empty)");
            }
        } else if (command == "RESET_CONFIG") {
            Serial.println("Resetting configuration...");
            globalConfig.reset();
            globalConfig.save();
            Serial.println("✓ Configuration reset complete!");
        }
    }
}

// ==================== Setup ====================
void setup() {
    Serial.begin(115200);
    Serial.println("\n\nBitcoin Dashboard - Bootstrap Mode");

    // Load configuration from NVRAM
    Serial.println("\n=== Initializing Configuration ===");
    globalConfig.load();

    // Check if this is first run
    if (globalConfig.isFirstRun()) {
        Serial.println("⚠️  FIRST RUN DETECTED");
        Serial.println("Configure via Serial Monitor:");
        Serial.println("  SET_GEMINI_KEY=your-api-key-here");
    }

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

    // Check if WiFi credentials are stored in configuration
    bool hasStoredWiFi = globalConfig.hasWiFiCredentials();

    if (hasStoredWiFi) {
        Serial.println("Found stored WiFi credentials, connecting...");
        String ssid = globalConfig.getWiFiSSID();
        String pass = globalConfig.getWiFiPassword();

        WiFi.begin(ssid.c_str(), pass.c_str());

        int attempts = 0;
        while (WiFi.status() != WL_CONNECTED && attempts < 20) {
            delay(500);
            Serial.print(".");
            attempts++;
        }

        if (WiFi.status() == WL_CONNECTED) {
            Serial.println("\n✓ WiFi connected!");
            Serial.printf("IP: %s\n", WiFi.localIP().toString().c_str());
            screenManager->switchScreen(SCREEN_DASHBOARD);
        } else {
            Serial.println("\n✗ WiFi connection failed, showing scan screen");
            screenManager->switchScreen(SCREEN_WIFI_SCAN);
        }
    } else {
        // No stored WiFi, show scan screen
        Serial.println("No stored WiFi credentials, showing scan screen...");
        screenManager->switchScreen(SCREEN_WIFI_SCAN);
    }
}

// ==================== Main Loop ====================
unsigned long lastTouchDebug = 0;

void loop() {
    // Process serial commands (SCREENSHOT, STATUS, HELP)
    processSerialCommand();

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
