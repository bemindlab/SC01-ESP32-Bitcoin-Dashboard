#ifndef CONFIG_H
#define CONFIG_H

#include <Arduino.h>
#include <Preferences.h>

// Configuration namespace for NVRAM storage
#define CONFIG_NAMESPACE "btc_dash"

// Configuration keys
#define CONFIG_KEY_GEMINI_API "gemini_key"
#define CONFIG_KEY_OPENAI_API "openai_key"
#define CONFIG_KEY_WIFI_SSID "wifi_ssid"
#define CONFIG_KEY_WIFI_PASS "wifi_pass"
#define CONFIG_KEY_PRICE_INTERVAL "price_int"
#define CONFIG_KEY_BLOCK_INTERVAL "block_int"
#define CONFIG_KEY_MEMPOOL_INTERVAL "mempool_int"
#define CONFIG_KEY_FIRST_RUN "first_run"

// Default values
#define DEFAULT_PRICE_INTERVAL 30000    // 30 seconds
#define DEFAULT_BLOCK_INTERVAL 60000    // 60 seconds
#define DEFAULT_MEMPOOL_INTERVAL 30000  // 30 seconds

// Configuration structure
struct AppConfig {
    // API Keys
    String geminiApiKey;
    String openaiApiKey;

    // WiFi Credentials
    String wifiSSID;
    String wifiPassword;

    // Update Intervals (milliseconds)
    unsigned long priceInterval;
    unsigned long blockInterval;
    unsigned long mempoolInterval;

    // System
    bool firstRun;

    // Constructor with defaults
    AppConfig() {
        geminiApiKey = "";
        openaiApiKey = "";
        wifiSSID = "";
        wifiPassword = "";
        priceInterval = DEFAULT_PRICE_INTERVAL;
        blockInterval = DEFAULT_BLOCK_INTERVAL;
        mempoolInterval = DEFAULT_MEMPOOL_INTERVAL;
        firstRun = true;
    }
};

// Configuration Manager Class
class ConfigManager {
private:
    Preferences preferences;
    AppConfig config;
    bool isLoaded;

public:
    ConfigManager();
    ~ConfigManager();

    // Load configuration from NVRAM
    bool load();

    // Save configuration to NVRAM
    bool save();

    // Reset to defaults
    void reset();

    // Getters
    String getGeminiApiKey() const { return config.geminiApiKey; }
    String getOpenAIApiKey() const { return config.openaiApiKey; }
    String getWiFiSSID() const { return config.wifiSSID; }
    String getWiFiPassword() const { return config.wifiPassword; }
    unsigned long getPriceInterval() const { return config.priceInterval; }
    unsigned long getBlockInterval() const { return config.blockInterval; }
    unsigned long getMempoolInterval() const { return config.mempoolInterval; }
    bool isFirstRun() const { return config.firstRun; }
    bool hasGeminiKey() const { return config.geminiApiKey.length() > 0; }
    bool hasOpenAIKey() const { return config.openaiApiKey.length() > 0; }
    bool hasWiFiCredentials() const { return config.wifiSSID.length() > 0; }

    // Setters
    void setGeminiApiKey(const String& key);
    void setOpenAIApiKey(const String& key);
    void setWiFiCredentials(const String& ssid, const String& password);
    void setPriceInterval(unsigned long interval);
    void setBlockInterval(unsigned long interval);
    void setMempoolInterval(unsigned long interval);
    void setFirstRun(bool firstRun);

    // Get full config
    AppConfig getConfig() const { return config; }

    // Validate configuration
    bool isValid() const;

    // Print configuration (for debugging, hides sensitive data)
    void printConfig() const;
};

// Global configuration instance
extern ConfigManager globalConfig;

#endif
