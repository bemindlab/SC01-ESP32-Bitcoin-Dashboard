#include "Config.h"
#include "utils/SDLogger.h"

// Global instance
ConfigManager globalConfig;

ConfigManager::ConfigManager() {
    isLoaded = false;
}

ConfigManager::~ConfigManager() {
    preferences.end();
}

bool ConfigManager::load() {
    Serial.println("\n=== Loading Configuration ===");

    // Open preferences in read-only mode
    if (!preferences.begin(CONFIG_NAMESPACE, true)) {
        Serial.println("Failed to open preferences for reading");
        return false;
    }

    // Load API keys
    config.geminiApiKey = preferences.getString(CONFIG_KEY_GEMINI_API, "");
    config.openaiApiKey = preferences.getString(CONFIG_KEY_OPENAI_API, "");

    // Load WiFi credentials
    config.wifiSSID = preferences.getString(CONFIG_KEY_WIFI_SSID, "");
    config.wifiPassword = preferences.getString(CONFIG_KEY_WIFI_PASS, "");

    // Load intervals
    config.priceInterval = preferences.getULong(CONFIG_KEY_PRICE_INTERVAL, DEFAULT_PRICE_INTERVAL);
    config.blockInterval = preferences.getULong(CONFIG_KEY_BLOCK_INTERVAL, DEFAULT_BLOCK_INTERVAL);
    config.mempoolInterval = preferences.getULong(CONFIG_KEY_MEMPOOL_INTERVAL, DEFAULT_MEMPOOL_INTERVAL);

    // Load system flags
    config.firstRun = preferences.getBool(CONFIG_KEY_FIRST_RUN, true);

    // Load Telegram configuration
    config.telegramToken = preferences.getString(CONFIG_KEY_TELEGRAM_TOKEN, "");
    config.telegramChatId = preferences.getString(CONFIG_KEY_TELEGRAM_CHAT_ID, "");
    config.telegramEnabled = preferences.getBool(CONFIG_KEY_TELEGRAM_ENABLED, false);
    config.priceAlertHigh = preferences.getFloat(CONFIG_KEY_PRICE_ALERT_HIGH, 0.0f);
    config.priceAlertLow = preferences.getFloat(CONFIG_KEY_PRICE_ALERT_LOW, 0.0f);
    config.alert5Percent = preferences.getBool(CONFIG_KEY_ALERT_5PCT, false);
    config.alert10Percent = preferences.getBool(CONFIG_KEY_ALERT_10PCT, false);
    config.alert20Percent = preferences.getBool(CONFIG_KEY_ALERT_20PCT, false);
    config.dailyReportEnabled = preferences.getBool(CONFIG_KEY_DAILY_REPORT_EN, false);
    config.dailyReportHour = preferences.getInt(CONFIG_KEY_DAILY_REPORT_HR, DEFAULT_DAILY_REPORT_HOUR);
    config.dailyReportMinute = preferences.getInt(CONFIG_KEY_DAILY_REPORT_MIN, DEFAULT_DAILY_REPORT_MINUTE);
    config.alertCooldown = preferences.getULong(CONFIG_KEY_ALERT_COOLDOWN, DEFAULT_ALERT_COOLDOWN);

    preferences.end();

    isLoaded = true;

    Serial.println("Configuration loaded successfully!");
    printConfig();

    return true;
}

bool ConfigManager::save() {
    Serial.println("\n=== Saving Configuration ===");

    // Open preferences in read-write mode
    if (!preferences.begin(CONFIG_NAMESPACE, false)) {
        Serial.println("Failed to open preferences for writing");
        sdLogger.log(LOG_ERROR, "Failed to save configuration: NVRAM write error");
        return false;
    }

    // Save API keys
    preferences.putString(CONFIG_KEY_GEMINI_API, config.geminiApiKey);
    preferences.putString(CONFIG_KEY_OPENAI_API, config.openaiApiKey);

    // Save WiFi credentials
    preferences.putString(CONFIG_KEY_WIFI_SSID, config.wifiSSID);
    preferences.putString(CONFIG_KEY_WIFI_PASS, config.wifiPassword);

    // Save intervals
    preferences.putULong(CONFIG_KEY_PRICE_INTERVAL, config.priceInterval);
    preferences.putULong(CONFIG_KEY_BLOCK_INTERVAL, config.blockInterval);
    preferences.putULong(CONFIG_KEY_MEMPOOL_INTERVAL, config.mempoolInterval);

    // Save system flags
    preferences.putBool(CONFIG_KEY_FIRST_RUN, config.firstRun);

    // Save Telegram configuration
    preferences.putString(CONFIG_KEY_TELEGRAM_TOKEN, config.telegramToken);
    preferences.putString(CONFIG_KEY_TELEGRAM_CHAT_ID, config.telegramChatId);
    preferences.putBool(CONFIG_KEY_TELEGRAM_ENABLED, config.telegramEnabled);
    preferences.putFloat(CONFIG_KEY_PRICE_ALERT_HIGH, config.priceAlertHigh);
    preferences.putFloat(CONFIG_KEY_PRICE_ALERT_LOW, config.priceAlertLow);
    preferences.putBool(CONFIG_KEY_ALERT_5PCT, config.alert5Percent);
    preferences.putBool(CONFIG_KEY_ALERT_10PCT, config.alert10Percent);
    preferences.putBool(CONFIG_KEY_ALERT_20PCT, config.alert20Percent);
    preferences.putBool(CONFIG_KEY_DAILY_REPORT_EN, config.dailyReportEnabled);
    preferences.putInt(CONFIG_KEY_DAILY_REPORT_HR, config.dailyReportHour);
    preferences.putInt(CONFIG_KEY_DAILY_REPORT_MIN, config.dailyReportMinute);
    preferences.putULong(CONFIG_KEY_ALERT_COOLDOWN, config.alertCooldown);

    preferences.end();

    Serial.println("Configuration saved successfully!");
    sdLogger.log(LOG_INFO, "Configuration saved to NVRAM");
    printConfig();

    return true;
}

void ConfigManager::reset() {
    Serial.println("\n=== Resetting Configuration ===");
    sdLogger.log(LOG_WARN, "Configuration reset requested");

    // Open preferences in read-write mode
    if (!preferences.begin(CONFIG_NAMESPACE, false)) {
        Serial.println("Failed to open preferences for reset");
        sdLogger.log(LOG_ERROR, "Failed to reset configuration: NVRAM access error");
        return;
    }

    // Clear all stored data
    preferences.clear();
    preferences.end();

    // Reset to defaults
    config = AppConfig();

    Serial.println("Configuration reset to defaults");
    sdLogger.log(LOG_INFO, "Configuration reset to factory defaults");
}

void ConfigManager::setGeminiApiKey(const String& key) {
    config.geminiApiKey = key;
    Serial.println("Gemini API key updated");

    // Log with masked key (show only first 4 chars)
    if (key.length() > 4) {
        String masked = key.substring(0, 4) + "..." + String((int)(key.length() - 4)) + " chars";
        sdLogger.logf(LOG_INFO, "Gemini API key updated: %s", masked.c_str());
    } else {
        sdLogger.log(LOG_INFO, "Gemini API key updated: ****");
    }
}

void ConfigManager::setOpenAIApiKey(const String& key) {
    config.openaiApiKey = key;
    Serial.println("OpenAI API key updated");

    // Log with masked key (show only first 4 chars)
    if (key.length() > 4) {
        String masked = key.substring(0, 4) + "..." + String((int)(key.length() - 4)) + " chars";
        sdLogger.logf(LOG_INFO, "OpenAI API key updated: %s", masked.c_str());
    } else {
        sdLogger.log(LOG_INFO, "OpenAI API key updated: ****");
    }
}

void ConfigManager::setWiFiCredentials(const String& ssid, const String& password) {
    config.wifiSSID = ssid;
    config.wifiPassword = password;
    Serial.printf("WiFi credentials updated: SSID=%s\n", ssid.c_str());

    // Log WiFi change with masked password
    sdLogger.logf(LOG_INFO, "WiFi credentials updated: SSID=%s, Password=**** (%d chars)",
                 ssid.c_str(), password.length());
}

void ConfigManager::setPriceInterval(unsigned long interval) {
    unsigned long oldInterval = config.priceInterval;
    config.priceInterval = interval;
    Serial.printf("Price interval updated: %lu ms\n", interval);

    sdLogger.logf(LOG_INFO, "Price update interval changed: %lu ms -> %lu ms (%lu sec)",
                 oldInterval, interval, interval / 1000);
}

void ConfigManager::setBlockInterval(unsigned long interval) {
    unsigned long oldInterval = config.blockInterval;
    config.blockInterval = interval;
    Serial.printf("Block interval updated: %lu ms\n", interval);

    sdLogger.logf(LOG_INFO, "Block update interval changed: %lu ms -> %lu ms (%lu sec)",
                 oldInterval, interval, interval / 1000);
}

void ConfigManager::setMempoolInterval(unsigned long interval) {
    unsigned long oldInterval = config.mempoolInterval;
    config.mempoolInterval = interval;
    Serial.printf("Mempool interval updated: %lu ms\n", interval);

    sdLogger.logf(LOG_INFO, "Mempool update interval changed: %lu ms -> %lu ms (%lu sec)",
                 oldInterval, interval, interval / 1000);
}

void ConfigManager::setFirstRun(bool firstRun) {
    config.firstRun = firstRun;
    Serial.printf("First run flag: %s\n", firstRun ? "true" : "false");
    sdLogger.logf(LOG_INFO, "First run flag set to: %s", firstRun ? "true" : "false");
}

bool ConfigManager::isValid() const {
    // Basic validation
    bool valid = true;

    // Check if critical settings are present
    if (config.geminiApiKey.length() == 0) {
        Serial.println("WARNING: Gemini API key not configured");
        valid = false;
    }

    // Validate intervals
    if (config.priceInterval < 5000) {
        Serial.println("WARNING: Price interval too low (minimum 5s)");
        valid = false;
    }

    if (config.blockInterval < 10000) {
        Serial.println("WARNING: Block interval too low (minimum 10s)");
        valid = false;
    }

    return valid;
}

void ConfigManager::setTelegramToken(const String& token) {
    config.telegramToken = token;
    Serial.println("Telegram bot token updated");

    // Log with masked token (show only last 4 chars)
    if (token.length() > 4) {
        String masked = "..." + token.substring(token.length() - 4);
        sdLogger.logf(LOG_INFO, "Telegram bot token updated: %s (%d chars)",
                     masked.c_str(), token.length());
    } else {
        sdLogger.log(LOG_INFO, "Telegram bot token updated: ****");
    }
}

void ConfigManager::setTelegramChatId(const String& chatId) {
    config.telegramChatId = chatId;
    Serial.printf("Telegram chat ID updated: %s\n", chatId.c_str());
    sdLogger.logf(LOG_INFO, "Telegram chat ID updated: %s", chatId.c_str());
}

void ConfigManager::setTelegramEnabled(bool enabled) {
    config.telegramEnabled = enabled;
    Serial.printf("Telegram notifications: %s\n", enabled ? "ENABLED" : "DISABLED");
    sdLogger.logf(LOG_INFO, "Telegram notifications %s", enabled ? "enabled" : "disabled");
}

void ConfigManager::setPriceAlertHigh(float threshold) {
    config.priceAlertHigh = threshold;
    if (threshold > 0) {
        Serial.printf("Price alert (high) set to: $%.2f\n", threshold);
        sdLogger.logf(LOG_INFO, "Price alert high threshold set: $%.2f", threshold);
    } else {
        Serial.println("Price alert (high) disabled");
        sdLogger.log(LOG_INFO, "Price alert high threshold disabled");
    }
}

void ConfigManager::setPriceAlertLow(float threshold) {
    config.priceAlertLow = threshold;
    if (threshold > 0) {
        Serial.printf("Price alert (low) set to: $%.2f\n", threshold);
        sdLogger.logf(LOG_INFO, "Price alert low threshold set: $%.2f", threshold);
    } else {
        Serial.println("Price alert (low) disabled");
        sdLogger.log(LOG_INFO, "Price alert low threshold disabled");
    }
}

void ConfigManager::setAlert5Percent(bool enabled) {
    config.alert5Percent = enabled;
    Serial.printf("5%% price change alert: %s\n", enabled ? "ENABLED" : "DISABLED");
    sdLogger.logf(LOG_INFO, "5%% price change alert %s", enabled ? "enabled" : "disabled");
}

void ConfigManager::setAlert10Percent(bool enabled) {
    config.alert10Percent = enabled;
    Serial.printf("10%% price change alert: %s\n", enabled ? "ENABLED" : "DISABLED");
    sdLogger.logf(LOG_INFO, "10%% price change alert %s", enabled ? "enabled" : "disabled");
}

void ConfigManager::setAlert20Percent(bool enabled) {
    config.alert20Percent = enabled;
    Serial.printf("20%% price change alert: %s\n", enabled ? "ENABLED" : "DISABLED");
    sdLogger.logf(LOG_INFO, "20%% price change alert %s", enabled ? "enabled" : "disabled");
}

void ConfigManager::setDailyReportEnabled(bool enabled) {
    config.dailyReportEnabled = enabled;
    Serial.printf("Daily report: %s\n", enabled ? "ENABLED" : "DISABLED");
    sdLogger.logf(LOG_INFO, "Daily report %s", enabled ? "enabled" : "disabled");
}

void ConfigManager::setDailyReportTime(int hour, int minute) {
    // Validate hour and minute
    if (hour < 0 || hour > 23) {
        Serial.printf("ERROR: Invalid hour %d (must be 0-23)\n", hour);
        sdLogger.logf(LOG_ERROR, "Invalid daily report hour: %d", hour);
        return;
    }
    if (minute < 0 || minute > 59) {
        Serial.printf("ERROR: Invalid minute %d (must be 0-59)\n", minute);
        sdLogger.logf(LOG_ERROR, "Invalid daily report minute: %d", minute);
        return;
    }

    config.dailyReportHour = hour;
    config.dailyReportMinute = minute;
    Serial.printf("Daily report time set to: %02d:%02d\n", hour, minute);
    sdLogger.logf(LOG_INFO, "Daily report time set: %02d:%02d", hour, minute);
}

void ConfigManager::setAlertCooldown(unsigned long cooldown) {
    config.alertCooldown = cooldown;
    Serial.printf("Alert cooldown set to: %lu ms (%lu min)\n", cooldown, cooldown / 60000);
    sdLogger.logf(LOG_INFO, "Alert cooldown set: %lu ms (%lu min)", cooldown, cooldown / 60000);
}

void ConfigManager::printConfig() const {
    Serial.println("\n--- Current Configuration ---");

    // API Keys (show only first/last 4 chars for security)
    if (config.geminiApiKey.length() > 8) {
        String masked = config.geminiApiKey.substring(0, 4) + "..." +
                       config.geminiApiKey.substring(config.geminiApiKey.length() - 4);
        Serial.printf("Gemini API Key: %s\n", masked.c_str());
    } else if (config.geminiApiKey.length() > 0) {
        Serial.println("Gemini API Key: ****");
    } else {
        Serial.println("Gemini API Key: NOT SET");
    }

    if (config.openaiApiKey.length() > 8) {
        String masked = config.openaiApiKey.substring(0, 4) + "..." +
                       config.openaiApiKey.substring(config.openaiApiKey.length() - 4);
        Serial.printf("OpenAI API Key: %s\n", masked.c_str());
    } else if (config.openaiApiKey.length() > 0) {
        Serial.println("OpenAI API Key: ****");
    } else {
        Serial.println("OpenAI API Key: NOT SET");
    }

    // WiFi credentials (hide password)
    if (config.wifiSSID.length() > 0) {
        Serial.printf("WiFi SSID: %s\n", config.wifiSSID.c_str());
        Serial.println("WiFi Password: ****");
    } else {
        Serial.println("WiFi SSID: NOT SET");
    }

    // Intervals
    Serial.printf("Price Interval: %lu ms (%lu sec)\n",
                 config.priceInterval, config.priceInterval / 1000);
    Serial.printf("Block Interval: %lu ms (%lu sec)\n",
                 config.blockInterval, config.blockInterval / 1000);
    Serial.printf("Mempool Interval: %lu ms (%lu sec)\n",
                 config.mempoolInterval, config.mempoolInterval / 1000);

    // System flags
    Serial.printf("First Run: %s\n", config.firstRun ? "YES" : "NO");

    // Telegram configuration
    Serial.println("\n[Telegram Configuration]");
    if (config.telegramToken.length() > 4) {
        String masked = "..." + config.telegramToken.substring(config.telegramToken.length() - 4);
        Serial.printf("Bot Token: %s (%d chars)\n", masked.c_str(), config.telegramToken.length());
    } else if (config.telegramToken.length() > 0) {
        Serial.println("Bot Token: ****");
    } else {
        Serial.println("Bot Token: NOT SET");
    }

    if (config.telegramChatId.length() > 0) {
        Serial.printf("Chat ID: %s\n", config.telegramChatId.c_str());
    } else {
        Serial.println("Chat ID: NOT SET");
    }

    Serial.printf("Notifications: %s\n", config.telegramEnabled ? "ENABLED" : "DISABLED");

    // Price alerts
    if (config.priceAlertHigh > 0) {
        Serial.printf("Price Alert (High): $%.2f\n", config.priceAlertHigh);
    } else {
        Serial.println("Price Alert (High): DISABLED");
    }

    if (config.priceAlertLow > 0) {
        Serial.printf("Price Alert (Low): $%.2f\n", config.priceAlertLow);
    } else {
        Serial.println("Price Alert (Low): DISABLED");
    }

    // Percentage alerts
    Serial.printf("5%% Change Alert: %s\n", config.alert5Percent ? "ENABLED" : "DISABLED");
    Serial.printf("10%% Change Alert: %s\n", config.alert10Percent ? "ENABLED" : "DISABLED");
    Serial.printf("20%% Change Alert: %s\n", config.alert20Percent ? "ENABLED" : "DISABLED");

    // Daily report
    Serial.printf("Daily Report: %s", config.dailyReportEnabled ? "ENABLED" : "DISABLED");
    if (config.dailyReportEnabled) {
        Serial.printf(" at %02d:%02d", config.dailyReportHour, config.dailyReportMinute);
    }
    Serial.println();

    Serial.printf("Alert Cooldown: %lu ms (%lu min)\n",
                 config.alertCooldown, config.alertCooldown / 60000);

    Serial.println("-----------------------------\n");
}
