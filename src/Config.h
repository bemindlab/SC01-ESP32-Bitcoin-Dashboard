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

// Telegram configuration keys
#define CONFIG_KEY_TELEGRAM_TOKEN "tg_token"
#define CONFIG_KEY_TELEGRAM_CHAT_ID "tg_chat_id"
#define CONFIG_KEY_TELEGRAM_ENABLED "tg_enabled"
#define CONFIG_KEY_PRICE_ALERT_HIGH "price_hi"
#define CONFIG_KEY_PRICE_ALERT_LOW "price_lo"
#define CONFIG_KEY_ALERT_5PCT "alert_5pct"
#define CONFIG_KEY_ALERT_10PCT "alert_10pct"
#define CONFIG_KEY_ALERT_20PCT "alert_20pct"
#define CONFIG_KEY_DAILY_REPORT_EN "daily_rpt_en"
#define CONFIG_KEY_DAILY_REPORT_HR "daily_rpt_hr"
#define CONFIG_KEY_DAILY_REPORT_MIN "daily_rpt_min"
#define CONFIG_KEY_ALERT_COOLDOWN "alert_cool"

// Default values
#define DEFAULT_PRICE_INTERVAL 30000    // 30 seconds
#define DEFAULT_BLOCK_INTERVAL 60000    // 60 seconds
#define DEFAULT_MEMPOOL_INTERVAL 30000  // 30 seconds

// Telegram default values
#define DEFAULT_ALERT_COOLDOWN 300000   // 5 minutes (300000 ms)
#define DEFAULT_DAILY_REPORT_HOUR 8     // 8 AM
#define DEFAULT_DAILY_REPORT_MINUTE 0   // Top of hour

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

    // Telegram Configuration
    String telegramToken;
    String telegramChatId;
    bool telegramEnabled;
    float priceAlertHigh;       // Upper price threshold (0 = disabled)
    float priceAlertLow;        // Lower price threshold (0 = disabled)
    bool alert5Percent;         // Alert on 5% price change
    bool alert10Percent;        // Alert on 10% price change
    bool alert20Percent;        // Alert on 20% price change
    bool dailyReportEnabled;    // Enable daily report
    int dailyReportHour;        // Hour for daily report (0-23)
    int dailyReportMinute;      // Minute for daily report (0-59)
    unsigned long alertCooldown; // Cooldown between alerts (ms)

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

        // Telegram defaults
        telegramToken = "";
        telegramChatId = "";
        telegramEnabled = false;
        priceAlertHigh = 0.0f;
        priceAlertLow = 0.0f;
        alert5Percent = false;
        alert10Percent = false;
        alert20Percent = false;
        dailyReportEnabled = false;
        dailyReportHour = DEFAULT_DAILY_REPORT_HOUR;
        dailyReportMinute = DEFAULT_DAILY_REPORT_MINUTE;
        alertCooldown = DEFAULT_ALERT_COOLDOWN;
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

    // Telegram Getters
    String getTelegramToken() const { return config.telegramToken; }
    String getTelegramChatId() const { return config.telegramChatId; }
    bool isTelegramEnabled() const { return config.telegramEnabled; }
    float getPriceAlertHigh() const { return config.priceAlertHigh; }
    float getPriceAlertLow() const { return config.priceAlertLow; }
    bool isAlert5Percent() const { return config.alert5Percent; }
    bool isAlert10Percent() const { return config.alert10Percent; }
    bool isAlert20Percent() const { return config.alert20Percent; }
    bool isDailyReportEnabled() const { return config.dailyReportEnabled; }
    int getDailyReportHour() const { return config.dailyReportHour; }
    int getDailyReportMinute() const { return config.dailyReportMinute; }
    unsigned long getAlertCooldown() const { return config.alertCooldown; }
    bool hasTelegramConfig() const { return config.telegramToken.length() > 0 && config.telegramChatId.length() > 0; }

    // Setters
    void setGeminiApiKey(const String& key);
    void setOpenAIApiKey(const String& key);
    void setWiFiCredentials(const String& ssid, const String& password);
    void setPriceInterval(unsigned long interval);
    void setBlockInterval(unsigned long interval);
    void setMempoolInterval(unsigned long interval);
    void setFirstRun(bool firstRun);

    // Telegram Setters
    void setTelegramToken(const String& token);
    void setTelegramChatId(const String& chatId);
    void setTelegramEnabled(bool enabled);
    void setPriceAlertHigh(float threshold);
    void setPriceAlertLow(float threshold);
    void setAlert5Percent(bool enabled);
    void setAlert10Percent(bool enabled);
    void setAlert20Percent(bool enabled);
    void setDailyReportEnabled(bool enabled);
    void setDailyReportTime(int hour, int minute);
    void setAlertCooldown(unsigned long cooldown);

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
