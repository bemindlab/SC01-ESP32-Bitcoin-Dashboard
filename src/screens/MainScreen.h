#ifndef MAINSCREEN_H
#define MAINSCREEN_H

#include "ScreenManager.h"
#include "../Config.h"
#include "../api/BTCData.h"

class MainScreen : public BaseScreen {
private:
    ScreenManager* manager;

    // Data update intervals
    static const unsigned long PRICE_UPDATE = 30000;
    static const unsigned long STATS_UPDATE = 60000;
    static const unsigned long AI_UPDATE = 300000;  // 5 minutes for AI signals

    unsigned long lastPriceUpdate = 0;
    unsigned long lastStatsUpdate = 0;
    unsigned long lastAIUpdate = 0;

    // Vertical scrolling
    int scrollOffsetY = 0;
    int maxScrollY = 0;

    // Horizontal scrolling
    int scrollOffsetX = 0;
    int maxScrollX = 0;

    // Touch tracking
    int16_t lastTouchX = 0;
    int16_t lastTouchY = 0;
    int16_t initialTouchX = 0;
    int16_t initialTouchY = 0;
    bool isDragging = false;
    bool isHorizontalScroll = false;

    // Performance optimization
    int lastDrawnScrollX = 0;
    int lastDrawnScrollY = 0;
    static const int SCROLL_REDRAW_THRESHOLD = 1;  // Pixels moved before redraw (immediate response)
    unsigned long lastDrawTime = 0;
    static const unsigned long MIN_DRAW_INTERVAL = 8;  // Min 8ms between redraws (120 FPS)

    // Screen rotation
    uint8_t rotation = 1;  // 0=0°, 1=90°, 2=180°, 3=270°

    void drawCard(int x, int y, int w, int h, const char* title, const char* value, uint32_t color);
    void drawHeader();
    void drawContent();
    void rotateScreen();

    // API fetch functions
    bool fetchBTCPrice();
    bool fetchBlockData();
    bool fetchMempoolData();
    bool fetchDCASignal();
    bool fetchTradingSignal();

public:
    void init(ScreenManager* mgr);
    void update();
    void handleTouch(int16_t x, int16_t y);
};

#endif
