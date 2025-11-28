#ifndef BTC_NEWS_SCREEN_H
#define BTC_NEWS_SCREEN_H

#include "ScreenManager.h"
#include "BTCDashboardScreen.h"
#include "../api/GeminiClient.h"

// UI Configuration
#define NEWS_SCROLL_SPEED 3           // Pixels per scroll step
#define NEWS_MAX_SCROLL_LINES 100     // Maximum lines to render
#define NEWS_LINE_HEIGHT 20           // Pixels per line
#define NEWS_PADDING 10               // Screen padding

class BTCNewsScreen : public BaseScreen {
private:
    ScreenManager* manager;
    GeminiClient* geminiClient;
    BTCData currentBTCData;

    // News content
    String newsText;
    bool hasNews;
    bool isLoading;
    String errorMessage;

    // Scrolling
    int scrollOffset;
    int maxScrollOffset;
    int touchStartY;
    bool isDragging;

    // Colors
    static const uint32_t COLOR_BG = 0x000000;
    static const uint32_t COLOR_HEADER_BG = 0x1A1A1A;
    static const uint32_t COLOR_TEXT_WHITE = 0xFFFFFF;
    static const uint32_t COLOR_TEXT_LIGHT = 0xCCCCCC;
    static const uint32_t COLOR_TEXT_DIM = 0x999999;
    static const uint32_t COLOR_BTC_ORANGE = 0xFF9500;
    static const uint32_t COLOR_LOADING = 0xFFBF00;
    static const uint32_t COLOR_ERROR = 0xFF3333;
    static const uint32_t COLOR_SUCCESS = 0x00FF00;

    // UI Layout
    static const int HEADER_HEIGHT = 40;
    static const int REFRESH_BTN_X = 420;
    static const int REFRESH_BTN_Y = 5;
    static const int REFRESH_BTN_SIZE = 30;
    static const int BACK_BTN_X = 10;
    static const int BACK_BTN_Y = 5;
    static const int BACK_BTN_SIZE = 30;

    // UI Methods
    void drawHeader();
    void drawRefreshButton();
    void drawBackButton();
    void drawLoadingAnimation();
    void drawNewsContent();
    void drawEmptyState();
    void drawErrorState();
    void calculateMaxScroll();

    // Word wrapping helper
    void wrapText(const String& text, int maxWidth, int fontSize);

    // Refresh functionality
    void refreshNews();

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
