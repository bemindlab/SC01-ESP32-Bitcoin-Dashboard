#include <Arduino.h>
#include <WiFi.h>
#include <FT6X36.h>
#include "DisplayConfig.h"
#include "screens/ScreenManager.h"
#include "Config.h"
#include "utils/SDLogger.h"
#include "utils/CrashHandler.h"

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
        } else if (command == "DEBUG_SCREENS") {
            Serial.println("\n=== Debug: Capturing Main Screen ===");
            Serial.println("Capturing main screen...");
            sendScreenshot();
            Serial.println("✓ Debug screen capture complete!");

        } else if (command == "STATUS") {
            Serial.printf("WiFi: %s\n", WiFi.status() == WL_CONNECTED ? "Connected" : "Disconnected");
            Serial.printf("Free Heap: %d bytes\n", ESP.getFreeHeap());
            Serial.printf("Uptime: %lu seconds\n", millis() / 1000);
            globalConfig.printConfig();
        } else if (command == "CHECK_SD_CARD") {
            Serial.println("\n=== SD Card Status ===");
            Serial.printf("Logger Ready: %s\n", sdLogger.isReady() ? "Yes" : "No");
            Serial.printf("Logger Enabled: %s\n", sdLogger.isEnabled() ? "Yes" : "No");

            if (sdLogger.isReady()) {
                Serial.printf("Card Present: %s\n", sdLogger.isCardPresent() ? "Yes" : "No");
                Serial.printf("Status: %s\n", sdLogger.getStatusString());
                Serial.printf("Free Space: %.2f GB\n", sdLogger.getFreeSpace() / (1024.0 * 1024.0 * 1024.0));
                Serial.printf("Total Space: %.2f GB\n", sdLogger.getTotalSpace() / (1024.0 * 1024.0 * 1024.0));
                Serial.printf("Log Files: %d\n", sdLogger.getLogFileCount());
            } else {
                Serial.println("\n⚠️  SD Card Not Available");
                Serial.println("\nSD Card Pin Configuration:");
                Serial.printf("  CS (Chip Select): GPIO %d\n", SD_CS_PIN);
                Serial.printf("  MOSI (Data Out):  GPIO %d\n", SD_MOSI_PIN);
                Serial.printf("  MISO (Data In):   GPIO %d\n", SD_MISO_PIN);
                Serial.printf("  CLK (Clock):      GPIO %d\n", SD_CLK_PIN);
                Serial.println("\nPossible causes:");
                Serial.println("  1. No SD card inserted in slot");
                Serial.println("  2. SD card not formatted (use FAT32)");
                Serial.println("  3. SD card damaged or incompatible");
                Serial.println("  4. Hardware connection issue");
                Serial.println("\nTo retry: Restart device or use REINIT_SD command");
            }
        } else if (command == "REINIT_SD") {
            Serial.println("\n=== Reinitializing SD Card ===");
            if (sdLogger.begin()) {
                Serial.println("✓ SD card reinitialized successfully");
            } else {
                Serial.println("✗ SD card reinitialization failed");
            }
        } else if (command == "FORMAT_SD_CARD") {
            if (sdLogger.formatCard()) {
                Serial.println("✓ SD card formatted successfully");
            } else {
                Serial.println("✗ SD card format failed");
            }
        } else if (command == "LOG_ENABLE") {
            sdLogger.enable();
            Serial.println("✓ SD card logging enabled");
        } else if (command == "LOG_DISABLE") {
            sdLogger.disable();
            Serial.println("✓ SD card logging disabled");
        } else if (command == "LOG_FLUSH") {
            sdLogger.flush();
            Serial.println("✓ Log buffer flushed to SD card");
        } else if (command.startsWith("LOG_LEVEL=")) {
            String level = command.substring(10);
            level.trim();
            level.toUpperCase();

            if (level == "DEBUG") {
                sdLogger.setLogLevel(LOG_DEBUG);
                Serial.println("✓ Log level set to DEBUG");
            } else if (level == "INFO") {
                sdLogger.setLogLevel(LOG_INFO);
                Serial.println("✓ Log level set to INFO");
            } else if (level == "WARN") {
                sdLogger.setLogLevel(LOG_WARN);
                Serial.println("✓ Log level set to WARN");
            } else if (level == "ERROR") {
                sdLogger.setLogLevel(LOG_ERROR);
                Serial.println("✓ Log level set to ERROR");
            } else if (level == "FATAL") {
                sdLogger.setLogLevel(LOG_FATAL);
                Serial.println("✓ Log level set to FATAL");
            } else {
                Serial.println("✗ Invalid log level. Use: DEBUG, INFO, WARN, ERROR, or FATAL");
            }
        } else if (command == "LOG_MEMORY") {
            sdLogger.logMemoryUsage();
            Serial.println("✓ Memory usage logged to SD card");
        } else if (command.startsWith("EXPORT_DATA")) {
            // Parse data type (e.g., EXPORT_DATA=PRICE, EXPORT_DATA=ALL, or just EXPORT_DATA for all)
            String dataType = "ALL";  // Default to all
            if (command.indexOf('=') > 0) {
                dataType = command.substring(command.indexOf('=') + 1);
                dataType.trim();
                dataType.toUpperCase();
            }
            Serial.printf("Exporting CSV data: %s\n", dataType.c_str());
            sdLogger.exportData(dataType.c_str());
        } else if (command == "CLEANUP_CSV") {
            Serial.println("Running CSV cleanup (retention policy)...");
            sdLogger.cleanup();
        } else if (command == "LAST_CRASH") {
            Serial.println("\n=== Last Crash Information ===");
            CrashInfo info = crashHandler.getCrashInfo();
            if (info.hasCrashed || info.crashCount > 0) {
                Serial.printf("Crash Count: %u\n", info.crashCount);
                Serial.printf("Last Crash Reason: %s\n", info.lastCrashReason);
                Serial.printf("Last Screen: %s\n", info.lastScreen);
                Serial.printf("Last API Call: %s\n", info.lastAPICall);
                Serial.printf("Uptime at crash: %u seconds\n", info.lastCrashUptime / 1000);
                Serial.printf("Watchdog timeout: %s\n", info.watchdogTimeout ? "YES" : "NO");
                Serial.println("\nCheck /logs/errors/ directory for detailed crash dumps");
            } else {
                Serial.println("No crash detected since last power-on reset");
            }
        } else if (command == "HELP") {
            Serial.println("\n=== Available Commands ===");
            Serial.println("\n[Display]");
            Serial.println("  SCREENSHOT         - Capture display buffer");
            Serial.println("  DEBUG_SCREENS      - Capture all screens for layout debugging");
            Serial.println("\n[Device Status]");
            Serial.println("  STATUS             - Show device status");
            Serial.println("  LAST_CRASH         - Show last crash information");
            Serial.println("\n[Configuration]");
            Serial.println("  SET_WIFI=SSID,Pass - Set WiFi credentials (requires restart)");
            Serial.println("  SET_GEMINI_KEY=xxx - Set Gemini API key");
            Serial.println("  SET_OPENAI_KEY=xxx - Set OpenAI API key");
            Serial.println("  RESET_CONFIG       - Reset all configuration");
            Serial.println("\n[Telegram Bot Configuration]");
            Serial.println("  SET_TELEGRAM_TOKEN=xxx       - Set Telegram bot token from @BotFather");
            Serial.println("  SET_TELEGRAM_CHAT_ID=xxx     - Set your Telegram chat ID");
            Serial.println("  TELEGRAM_ENABLE              - Enable Telegram notifications");
            Serial.println("  TELEGRAM_DISABLE             - Disable Telegram notifications");
            Serial.println("  SET_PRICE_ALERT_HIGH=xxx     - Set upper price alert threshold (0=disable)");
            Serial.println("  SET_PRICE_ALERT_LOW=xxx      - Set lower price alert threshold (0=disable)");
            Serial.println("  SET_PRICE_CHANGE_ALERT=5,10  - Enable percentage alerts (5,10,20)");
            Serial.println("  SET_DAILY_REPORT=HH:MM       - Enable daily report at specified time");
            Serial.println("  DISABLE_DAILY_REPORT         - Disable daily reports");
            Serial.println("  TEST_TELEGRAM                - Test Telegram configuration");
            Serial.println("  TELEGRAM_STATUS              - Show Telegram configuration status");
            Serial.println("\n[SD Card]");
            Serial.println("  CHECK_SD_CARD      - Check SD card status (detailed diagnostics)");
            Serial.println("  REINIT_SD          - Reinitialize SD card (hot-swap recovery)");
            Serial.println("  FORMAT_SD_CARD     - Format SD card (WARNING: Deletes all data!)");
            Serial.println("  LOG_ENABLE         - Enable SD card logging");
            Serial.println("  LOG_DISABLE        - Disable SD card logging");
            Serial.println("  LOG_FLUSH          - Force flush log buffer");
            Serial.println("  LOG_LEVEL=LEVEL    - Set log level (DEBUG/INFO/WARN/ERROR/FATAL)");
            Serial.println("  LOG_MEMORY         - Log current memory usage");
            Serial.println("\n[CSV Data Export]");
            Serial.println("  EXPORT_DATA        - Export all CSV data to serial console");
            Serial.println("  EXPORT_DATA=PRICE  - Export only price data");
            Serial.println("  EXPORT_DATA=BLOCKS - Export only block data");
            Serial.println("  EXPORT_DATA=MEMPOOL- Export only mempool data");
            Serial.println("  CLEANUP_CSV        - Run CSV retention policy (delete old files)");
            Serial.println("\n[Help]");
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
        } else if (command.startsWith("SET_OPENAI_KEY=")) {
            String key = command.substring(15);
            key.trim();
            if (key.length() > 0) {
                globalConfig.setOpenAIApiKey(key);
                if (globalConfig.save()) {
                    Serial.println("✓ OpenAI API key saved successfully!");
                } else {
                    Serial.println("✗ Failed to save API key");
                }
            } else {
                Serial.println("✗ Invalid API key (empty)");
            }
        } else if (command.startsWith("SET_WIFI=")) {
            String params = command.substring(9);
            params.trim();

            // Parse SSID and password (format: SET_WIFI=SSID,Password)
            int commaIndex = params.indexOf(',');
            if (commaIndex > 0) {
                String ssid = params.substring(0, commaIndex);
                String password = params.substring(commaIndex + 1);

                ssid.trim();
                password.trim();

                if (ssid.length() > 0) {
                    globalConfig.setWiFiCredentials(ssid, password);
                    if (globalConfig.save()) {
                        Serial.println("✓ WiFi credentials saved successfully!");
                        Serial.println("Restart device to connect to new network");
                    } else {
                        Serial.println("✗ Failed to save WiFi credentials");
                    }
                } else {
                    Serial.println("✗ Invalid SSID (empty)");
                }
            } else {
                Serial.println("✗ Invalid format. Use: SET_WIFI=SSID,Password");
            }
        } else if (command == "RESET_CONFIG") {
            Serial.println("Resetting configuration...");
            globalConfig.reset();
            globalConfig.save();
            Serial.println("✓ Configuration reset complete!");
        } else if (command.startsWith("SET_TELEGRAM_TOKEN=")) {
            String token = command.substring(19);
            token.trim();
            if (token.length() > 0) {
                globalConfig.setTelegramToken(token);
                if (globalConfig.save()) {
                    Serial.println("✓ Telegram bot token saved successfully!");
                } else {
                    Serial.println("✗ Failed to save Telegram token");
                }
            } else {
                Serial.println("✗ Invalid token (empty)");
            }
        } else if (command.startsWith("SET_TELEGRAM_CHAT_ID=")) {
            String chatId = command.substring(21);
            chatId.trim();
            if (chatId.length() > 0) {
                globalConfig.setTelegramChatId(chatId);
                if (globalConfig.save()) {
                    Serial.println("✓ Telegram chat ID saved successfully!");
                } else {
                    Serial.println("✗ Failed to save chat ID");
                }
            } else {
                Serial.println("✗ Invalid chat ID (empty)");
            }
        } else if (command == "TELEGRAM_ENABLE") {
            if (globalConfig.hasTelegramConfig()) {
                globalConfig.setTelegramEnabled(true);
                if (globalConfig.save()) {
                    Serial.println("✓ Telegram notifications enabled!");
                } else {
                    Serial.println("✗ Failed to save configuration");
                }
            } else {
                Serial.println("✗ Cannot enable: Telegram not configured");
                Serial.println("  Use SET_TELEGRAM_TOKEN and SET_TELEGRAM_CHAT_ID first");
            }
        } else if (command == "TELEGRAM_DISABLE") {
            globalConfig.setTelegramEnabled(false);
            if (globalConfig.save()) {
                Serial.println("✓ Telegram notifications disabled!");
            } else {
                Serial.println("✗ Failed to save configuration");
            }
        } else if (command.startsWith("SET_PRICE_ALERT_HIGH=")) {
            String value = command.substring(21);
            value.trim();
            float threshold = value.toFloat();
            if (threshold > 0 || value == "0") {
                globalConfig.setPriceAlertHigh(threshold);
                if (globalConfig.save()) {
                    if (threshold > 0) {
                        Serial.printf("✓ High price alert set to: $%.2f\n", threshold);
                    } else {
                        Serial.println("✓ High price alert disabled");
                    }
                } else {
                    Serial.println("✗ Failed to save configuration");
                }
            } else {
                Serial.println("✗ Invalid threshold value");
            }
        } else if (command.startsWith("SET_PRICE_ALERT_LOW=")) {
            String value = command.substring(20);
            value.trim();
            float threshold = value.toFloat();
            if (threshold > 0 || value == "0") {
                globalConfig.setPriceAlertLow(threshold);
                if (globalConfig.save()) {
                    if (threshold > 0) {
                        Serial.printf("✓ Low price alert set to: $%.2f\n", threshold);
                    } else {
                        Serial.println("✓ Low price alert disabled");
                    }
                } else {
                    Serial.println("✗ Failed to save configuration");
                }
            } else {
                Serial.println("✗ Invalid threshold value");
            }
        } else if (command.startsWith("SET_PRICE_CHANGE_ALERT=")) {
            String alerts = command.substring(23);
            alerts.trim();

            // Parse comma-separated percentages (e.g., "5,10,20")
            bool alert5 = false, alert10 = false, alert20 = false;
            int commaPos;

            while (alerts.length() > 0) {
                commaPos = alerts.indexOf(',');
                String pct = (commaPos > 0) ? alerts.substring(0, commaPos) : alerts;
                pct.trim();

                if (pct == "5") alert5 = true;
                else if (pct == "10") alert10 = true;
                else if (pct == "20") alert20 = true;

                if (commaPos > 0) {
                    alerts = alerts.substring(commaPos + 1);
                } else {
                    break;
                }
            }

            globalConfig.setAlert5Percent(alert5);
            globalConfig.setAlert10Percent(alert10);
            globalConfig.setAlert20Percent(alert20);

            if (globalConfig.save()) {
                Serial.println("✓ Price change alerts configured:");
                Serial.printf("  5%%: %s\n", alert5 ? "ENABLED" : "DISABLED");
                Serial.printf("  10%%: %s\n", alert10 ? "ENABLED" : "DISABLED");
                Serial.printf("  20%%: %s\n", alert20 ? "ENABLED" : "DISABLED");
            } else {
                Serial.println("✗ Failed to save configuration");
            }
        } else if (command.startsWith("SET_DAILY_REPORT=")) {
            String timeStr = command.substring(17);
            timeStr.trim();

            // Parse HH:MM format
            int colonPos = timeStr.indexOf(':');
            if (colonPos > 0) {
                String hourStr = timeStr.substring(0, colonPos);
                String minStr = timeStr.substring(colonPos + 1);

                int hour = hourStr.toInt();
                int minute = minStr.toInt();

                if (hour >= 0 && hour <= 23 && minute >= 0 && minute <= 59) {
                    globalConfig.setDailyReportTime(hour, minute);
                    globalConfig.setDailyReportEnabled(true);

                    if (globalConfig.save()) {
                        Serial.printf("✓ Daily report enabled at %02d:%02d\n", hour, minute);
                    } else {
                        Serial.println("✗ Failed to save configuration");
                    }
                } else {
                    Serial.println("✗ Invalid time (hour: 0-23, minute: 0-59)");
                }
            } else {
                Serial.println("✗ Invalid format. Use: SET_DAILY_REPORT=HH:MM");
                Serial.println("  Example: SET_DAILY_REPORT=08:00");
            }
        } else if (command == "DISABLE_DAILY_REPORT") {
            globalConfig.setDailyReportEnabled(false);
            if (globalConfig.save()) {
                Serial.println("✓ Daily report disabled");
            } else {
                Serial.println("✗ Failed to save configuration");
            }
        } else if (command == "TEST_TELEGRAM") {
            Serial.println("=== Telegram Test ===");
            if (globalConfig.hasTelegramConfig()) {
                Serial.println("Telegram configuration found:");
                String token = globalConfig.getTelegramToken();
                String chatId = globalConfig.getTelegramChatId();
                Serial.printf("  Token: ...%s (%d chars)\n",
                             token.substring(token.length() - 4).c_str(), token.length());
                Serial.printf("  Chat ID: %s\n", chatId.c_str());
                Serial.printf("  Enabled: %s\n", globalConfig.isTelegramEnabled() ? "YES" : "NO");
                Serial.println("\n⚠️  Note: Actual message sending requires TelegramClient implementation");
                Serial.println("  This test only validates configuration is stored correctly");
            } else {
                Serial.println("✗ Telegram not configured");
                Serial.println("  Use SET_TELEGRAM_TOKEN and SET_TELEGRAM_CHAT_ID first");
            }
        } else if (command == "TELEGRAM_STATUS") {
            Serial.println("\n=== Telegram Configuration Status ===");
            Serial.printf("Bot Token: %s\n",
                         globalConfig.getTelegramToken().length() > 0 ? "SET" : "NOT SET");
            Serial.printf("Chat ID: %s\n",
                         globalConfig.getTelegramChatId().length() > 0 ? "SET" : "NOT SET");
            Serial.printf("Notifications: %s\n",
                         globalConfig.isTelegramEnabled() ? "ENABLED" : "DISABLED");
            Serial.println("\n[Price Alerts]");
            if (globalConfig.getPriceAlertHigh() > 0) {
                Serial.printf("  High: $%.2f\n", globalConfig.getPriceAlertHigh());
            } else {
                Serial.println("  High: DISABLED");
            }
            if (globalConfig.getPriceAlertLow() > 0) {
                Serial.printf("  Low: $%.2f\n", globalConfig.getPriceAlertLow());
            } else {
                Serial.println("  Low: DISABLED");
            }
            Serial.println("\n[Price Change Alerts]");
            Serial.printf("  5%%: %s\n", globalConfig.isAlert5Percent() ? "ENABLED" : "DISABLED");
            Serial.printf("  10%%: %s\n", globalConfig.isAlert10Percent() ? "ENABLED" : "DISABLED");
            Serial.printf("  20%%: %s\n", globalConfig.isAlert20Percent() ? "ENABLED" : "DISABLED");
            Serial.println("\n[Daily Report]");
            if (globalConfig.isDailyReportEnabled()) {
                Serial.printf("  Time: %02d:%02d\n",
                             globalConfig.getDailyReportHour(),
                             globalConfig.getDailyReportMinute());
            } else {
                Serial.println("  DISABLED");
            }
            Serial.printf("\n[Alert Cooldown]\n  %lu ms (%lu minutes)\n",
                         globalConfig.getAlertCooldown(),
                         globalConfig.getAlertCooldown() / 60000);
            Serial.println("=====================================");
        }
    }
}

// ==================== Setup ====================
void setup() {
    Serial.begin(115200);
    Serial.println("\n\nBitcoin Dashboard - Bootstrap Mode");

    // Initialize crash handler (must be first to detect crashes)
    crashHandler.begin();

    // Load configuration from NVRAM
    Serial.println("\n=== Initializing Configuration ===");
    globalConfig.load();

    // Check if this is first run
    if (globalConfig.isFirstRun()) {
        Serial.println("⚠️  FIRST RUN DETECTED");
        Serial.println("Configure via Serial Monitor:");
        Serial.println("  SET_GEMINI_KEY=your-api-key-here");
    }

    // Initialize SD card logging
    Serial.println("\n=== Initializing SD Card ===");
    if (sdLogger.begin()) {
        Serial.println("✓ SD card logging initialized");
        sdLogger.logBoot("Bitcoin Dashboard started");
        sdLogger.logf(LOG_INFO, "Firmware version: 1.2.0");
        sdLogger.logf(LOG_INFO, "CPU: ESP32-S3 @ %u MHz", ESP.getCpuFreqMHz());
        sdLogger.logf(LOG_INFO, "Flash: %u MB", ESP.getFlashChipSize() / (1024 * 1024));
    } else {
        Serial.println("⚠️  SD card not available (logging disabled)");
        Serial.println("  Insert SD card and restart to enable logging");
    }

    // Initialize display
    lcd.init();
    lcd.setRotation(1);  // Landscape
    lcd.setBrightness(255);
    Serial.println("Display initialized!");
    sdLogger.log(LOG_INFO, "Display initialized: 480x320 landscape mode");

    // Initialize touch (SDA=6, SCL=5)
    Wire.begin(6, 5);
    if (touch.begin(40)) {
        Serial.println("Touch initialized successfully!");
        sdLogger.log(LOG_INFO, "Touch controller initialized: FT6X36");
    } else {
        Serial.println("ERROR: Touch initialization failed!");
        sdLogger.log(LOG_ERROR, "Touch controller initialization failed");
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
    sdLogger.log(LOG_INFO, "Screen manager initialized");

    // Check if WiFi credentials are stored in configuration
    bool hasStoredWiFi = globalConfig.hasWiFiCredentials();

    if (hasStoredWiFi) {
        Serial.println("Found stored WiFi credentials, connecting...");
        String ssid = globalConfig.getWiFiSSID();
        String pass = globalConfig.getWiFiPassword();

        sdLogger.logf(LOG_INFO, "Attempting WiFi connection to SSID: %s", ssid.c_str());
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
            sdLogger.logf(LOG_INFO, "WiFi connected successfully: IP=%s, RSSI=%d dBm",
                         WiFi.localIP().toString().c_str(), WiFi.RSSI());
            screenManager->switchScreen(SCREEN_MAIN);
        } else {
#ifdef SINGLE_SCREEN_MODE
            Serial.println("\n✗ WiFi connection failed!");
            Serial.println("SINGLE_SCREEN_MODE: Staying on Main screen (configure WiFi via serial)");
            sdLogger.logf(LOG_WARN, "WiFi connection failed after %d attempts (SSID: %s)",
                         attempts, ssid.c_str());
            screenManager->switchScreen(SCREEN_MAIN);
#else
            Serial.println("\n✗ WiFi connection failed, showing scan screen");
            sdLogger.logf(LOG_WARN, "WiFi connection failed after %d attempts, showing WiFi scan",
                         attempts);
            screenManager->switchScreen(SCREEN_WIFI_SCAN);
#endif
        }
    } else {
#ifdef SINGLE_SCREEN_MODE
        Serial.println("No stored WiFi credentials!");
        Serial.println("SINGLE_SCREEN_MODE: Use serial commands to configure WiFi");
        Serial.println("Example: SET_WIFI=YourSSID,YourPassword");
        sdLogger.log(LOG_INFO, "No WiFi credentials configured (SINGLE_SCREEN_MODE)");
        screenManager->switchScreen(SCREEN_MAIN);
#else
        // No stored WiFi, show scan screen
        Serial.println("No stored WiFi credentials, showing scan screen...");
        sdLogger.log(LOG_INFO, "No WiFi credentials, displaying WiFi scan screen");
        screenManager->switchScreen(SCREEN_WIFI_SCAN);
#endif
    }

    // Log initial memory usage
    sdLogger.logMemoryUsage();
    sdLogger.log(LOG_INFO, "Boot sequence completed");

    // Initialize watchdog timer after successful boot (30 second timeout)
    crashHandler.initWatchdog(30);
    crashHandler.setCurrentScreen("Main");
    Serial.println("\n✓ System initialization complete");
}

// ==================== Main Loop ====================
unsigned long lastTouchDebug = 0;
unsigned long lastMemoryLog = 0;
const unsigned long MEMORY_LOG_INTERVAL = 300000; // 5 minutes in milliseconds

void loop() {
    // Feed watchdog timer (must be called regularly)
    crashHandler.feedWatchdog();

    // Process serial commands (SCREENSHOT, STATUS, HELP, LAST_CRASH)
    processSerialCommand();

    // Debug: Print touch status every 2 seconds
    if (millis() - lastTouchDebug >= 2000) {
        uint8_t touches = touch.touched();
        if (touches > 0) {
            Serial.printf("[DEBUG] Touch detected: %d\n", touches);
        }
        lastTouchDebug = millis();
    }

    // Periodic memory logging (every 5 minutes)
    if (millis() - lastMemoryLog >= MEMORY_LOG_INTERVAL) {
        sdLogger.logMemoryUsage();
        sdLogger.logf(LOG_DEBUG, "Uptime: %lu seconds", millis() / 1000);
        lastMemoryLog = millis();
    }

    // Check for SD card hot-swap
    sdLogger.checkHotSwap();

    // Update current screen (includes touch processing)
    screenManager->update();

    delay(10);
}
