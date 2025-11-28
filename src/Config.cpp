#include "Config.h"

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

    preferences.end();

    Serial.println("Configuration saved successfully!");
    printConfig();

    return true;
}

void ConfigManager::reset() {
    Serial.println("\n=== Resetting Configuration ===");

    // Open preferences in read-write mode
    if (!preferences.begin(CONFIG_NAMESPACE, false)) {
        Serial.println("Failed to open preferences for reset");
        return;
    }

    // Clear all stored data
    preferences.clear();
    preferences.end();

    // Reset to defaults
    config = AppConfig();

    Serial.println("Configuration reset to defaults");
}

void ConfigManager::setGeminiApiKey(const String& key) {
    config.geminiApiKey = key;
    Serial.println("Gemini API key updated");
}

void ConfigManager::setOpenAIApiKey(const String& key) {
    config.openaiApiKey = key;
    Serial.println("OpenAI API key updated");
}

void ConfigManager::setWiFiCredentials(const String& ssid, const String& password) {
    config.wifiSSID = ssid;
    config.wifiPassword = password;
    Serial.printf("WiFi credentials updated: SSID=%s\n", ssid.c_str());
}

void ConfigManager::setPriceInterval(unsigned long interval) {
    config.priceInterval = interval;
    Serial.printf("Price interval updated: %lu ms\n", interval);
}

void ConfigManager::setBlockInterval(unsigned long interval) {
    config.blockInterval = interval;
    Serial.printf("Block interval updated: %lu ms\n", interval);
}

void ConfigManager::setMempoolInterval(unsigned long interval) {
    config.mempoolInterval = interval;
    Serial.printf("Mempool interval updated: %lu ms\n", interval);
}

void ConfigManager::setFirstRun(bool firstRun) {
    config.firstRun = firstRun;
    Serial.printf("First run flag: %s\n", firstRun ? "true" : "false");
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

    Serial.println("-----------------------------\n");
}
