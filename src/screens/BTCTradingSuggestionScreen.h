#ifndef BTC_TRADING_SUGGESTION_SCREEN_H
#define BTC_TRADING_SUGGESTION_SCREEN_H

#include "ScreenManager.h"
#include "BTCDashboardScreen.h"
#include "../api/OpenAIClient.h"
#include "../ui/TouchFeedbackManager.h"

// UI Configuration
#define TRADING_SCROLL_SPEED 3
#define TRADING_MAX_SCROLL_LINES 100
#define TRADING_LINE_HEIGHT 20
#define TRADING_PADDING 10

class BTCTradingSuggestionScreen : public BaseScreen {
private:
    ScreenManager* manager;
    OpenAIClient* openaiClient;
    BTCData currentBTCData;
    TradingSuggestion suggestion;
    TouchFeedbackManager feedback;

    // State
    bool hasSuggestion;
    bool isLoading;
    String errorMessage;
    unsigned long lastUpdate;

    // Scrolling
    int scrollOffset;
    int maxScrollOffset;
    int touchStartY;
    bool isDragging;

    // Update interval (5 minutes)
    static const unsigned long UPDATE_INTERVAL = 300000;

    // Colors
    static const uint32_t COLOR_BG = 0x000000;
    static const uint32_t COLOR_HEADER_BG = 0x1A1A1A;
    static const uint32_t COLOR_TEXT_WHITE = 0xFFFFFF;
    static const uint32_t COLOR_TEXT_LIGHT = 0xCCCCCC;
    static const uint32_t COLOR_TEXT_DIM = 0x999999;
    static const uint32_t COLOR_BTC_ORANGE = 0xFF9500;
    static const uint32_t COLOR_LOADING = 0xFFBF00;
    static const uint32_t COLOR_ERROR = 0xFF3333;

    // Signal colors
    static const uint32_t COLOR_STRONG_BUY = 0x00FF00;
    static const uint32_t COLOR_BUY = 0x88FF88;
    static const uint32_t COLOR_HOLD = 0xFFFF00;
    static const uint32_t COLOR_SELL = 0xFF8888;
    static const uint32_t COLOR_STRONG_SELL = 0xFF0000;
    static const uint32_t COLOR_UNCERTAIN = 0xCCCCCC;

    // UI Layout
    static const int HEADER_HEIGHT = 40;
    static const int REFRESH_BTN_X = 420;
    static const int REFRESH_BTN_Y = 5;
    static const int REFRESH_BTN_SIZE = 30;
    static const int BACK_BTN_X = 10;
    static const int BACK_BTN_Y = 5;
    static const int BACK_BTN_SIZE = 30;

    // Touch feedback IDs
    int backButtonFeedbackId;
    int refreshButtonFeedbackId;

    // UI Methods
    void drawHeader();
    void drawRefreshButton();
    void drawBackButton();
    void drawMarketSummary();
    void drawSignalBadge();
    void drawRecommendation();
    void drawLoadingAnimation();
    void drawEmptyState();
    void drawErrorState();
    void drawDisclaimer();
    void calculateMaxScroll();

    // Data methods
    void refreshSuggestion();
    uint32_t getSignalColor(TradingSignal signal);
    const char* getSignalText(TradingSignal signal);
    const char* getSignalIcon(TradingSignal signal);

    // Animation frame counter
    int animationFrame;

public:
    void init(ScreenManager* mgr) override;
    void update() override;
    void handleTouch(int16_t x, int16_t y) override;

    // Set Bitcoin data from dashboard
    void setBTCData(const BTCData& data);
};

#endif
