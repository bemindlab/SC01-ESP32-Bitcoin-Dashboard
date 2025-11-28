#ifndef SCREEN_MANAGER_H
#define SCREEN_MANAGER_H

#include <Arduino.h>
#include "../DisplayConfig.h"
#include <FT6X36.h>

// Screen types
enum Screen {
#ifndef SINGLE_SCREEN_MODE
    SCREEN_WIFI_SCAN,
    SCREEN_WIFI_CONNECT,
#endif
    SCREEN_DASHBOARD,
#ifndef SINGLE_SCREEN_MODE
    SCREEN_BTC_NEWS,
    SCREEN_TRADING_SUGGESTION,
    SCREEN_SETTINGS
#endif
};

// Forward declarations
class ScreenManager;

// Base screen class
class BaseScreen {
public:
    virtual void init(ScreenManager* manager) = 0;
    virtual void update() = 0;
    virtual void handleTouch(int16_t x, int16_t y) = 0;
    virtual ~BaseScreen() {}
};

#ifndef SINGLE_SCREEN_MODE
// Swipe gesture structure
struct SwipeGesture {
    int16_t startX, startY;
    int16_t endX, endY;
    unsigned long startTime;
    bool isActive;
};

// Swipe detection parameters
#define SWIPE_MIN_DISTANCE 80    // Minimum pixels for valid swipe
#define SWIPE_MAX_TIME 500       // Maximum time (ms) for swipe
#define SWIPE_THRESHOLD_Y 50     // Max vertical deviation
#endif

// Screen Manager
class ScreenManager {
private:
    LGFX* lcd;
    FT6X36* touch;
    BaseScreen* currentScreen;
    Screen currentScreenType;

#ifndef SINGLE_SCREEN_MODE
    // Swipe gesture tracking
    SwipeGesture swipeGesture;
#endif

    // Touch callback
    static ScreenManager* _instance;
    static void touchCallback(TPoint point, TEvent e);
    static void touchPressCallback(TPoint point, TEvent e);
    static void touchReleaseCallback(TPoint point, TEvent e);

#ifndef SINGLE_SCREEN_MODE
    // Swipe detection
    void handleSwipe(int16_t deltaX, int16_t deltaY, unsigned long duration);
    bool isHorizontalSwipe(int16_t deltaX, int16_t deltaY);
#endif

public:
    ScreenManager(LGFX* display, FT6X36* touchController);
    void switchScreen(Screen screen);
    void update();
    void handleTouch();

    LGFX* getLCD() { return lcd; }
    FT6X36* getTouch() { return touch; }
    Screen getCurrentScreen() { return currentScreenType; }
    BaseScreen* getCurrentScreenInstance() { return currentScreen; }
};

#endif
