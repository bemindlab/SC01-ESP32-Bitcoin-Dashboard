#ifndef BTC_DASHBOARD_SCREEN_H
#define BTC_DASHBOARD_SCREEN_H

#include "ScreenManager.h"
#include "../ui/TouchFeedbackManager.h"
#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>

// API Endpoints
#define API_PRICE "https://mempool.space/api/v1/prices"
#define API_BLOCKS "https://mempool.space/api/v1/blocks"
#define API_MEMPOOL "https://mempool.space/api/mempool"
#define API_FEES "https://mempool.space/api/v1/fees/recommended"

// Update intervals (ms)
#define PRICE_INTERVAL 30000      // 30 sec
#define BLOCK_INTERVAL 60000      // 60 sec
#define MEMPOOL_INTERVAL 30000    // 30 sec

// Bitcoin data structure
struct BTCData {
    float priceUSD = 0;
    float priceEUR = 0;
    int blockHeight = 0;
    String blockHash = "";
    int blockTxCount = 0;
    uint32_t blockTime = 0;
    int mempoolCount = 0;
    float mempoolSize = 0;
    int feeFast = 0;
    int feeMedium = 0;
    int feeSlow = 0;
};

class BTCDashboardScreen : public BaseScreen {
private:
    ScreenManager* manager;
    BTCData btcData;
    TouchFeedbackManager feedback;

    unsigned long lastPriceUpdate;
    unsigned long lastBlockUpdate;
    unsigned long lastMempoolUpdate;

    int wifiIconFeedbackId;
    int settingsIconFeedbackId;

    // Colors
    static const uint32_t COLOR_BG = 0x000000;
    static const uint32_t COLOR_CARD_BG = 0x0C0C0C;
    static const uint32_t COLOR_BTC_ORANGE = 0xFF9500;
    static const uint32_t COLOR_BTC_GOLD = 0xFFBF00;
    static const uint32_t COLOR_TEXT_WHITE = 0xFFFFFF;
    static const uint32_t COLOR_TEXT_LIGHT = 0xCCCCCC;
    static const uint32_t COLOR_TEXT_DIM = 0x999999;
    static const uint32_t COLOR_PRICE_GREEN = 0x00FF00;
    static const uint32_t COLOR_BLOCK_CYAN = 0x00FFFF;
    static const uint32_t COLOR_MEMPOOL_YELLOW = 0xFFFF00;
    static const uint32_t COLOR_FEE_ORANGE = 0xFF6600;
    static const uint32_t COLOR_BORDER_ORANGE = 0xFF9500;
    static const uint32_t COLOR_BORDER_GREEN = 0x00FF00;
    static const uint32_t COLOR_BORDER_CYAN = 0x00FFFF;
    static const uint32_t COLOR_BORDER_YELLOW = 0xFFFF00;

    // UI layout
    static const int CARD_WIDTH = 220;
    static const int CARD_HEIGHT = 100;
    static const int CARD_MARGIN = 10;

    // WiFi icon button position (top-right corner)
    static const int WIFI_ICON_X = 440;
    static const int WIFI_ICON_Y = 5;
    static const int WIFI_ICON_SIZE = 30;

    // Settings icon button position (next to WiFi icon)
    static const int SETTINGS_ICON_X = 400;
    static const int SETTINGS_ICON_Y = 5;
    static const int SETTINGS_ICON_SIZE = 30;

    // API methods
    bool fetchPrice();
    bool fetchBlocks();
    bool fetchMempool();
    bool fetchFees();

    // UI methods
    void drawCard(int x, int y, int w, int h, uint32_t borderColor,
                  const char* title, const char* content, uint32_t contentColor);
    void drawWiFiIcon();
    void drawSettingsIcon();
    void setupUI();
    void updateUI();

public:
    void init(ScreenManager* mgr) override;
    void update() override;
    void handleTouch(int16_t x, int16_t y) override;

    // Getter for BTC data (used when navigating to News screen)
    BTCData getBTCData() const { return btcData; }
};

#endif
