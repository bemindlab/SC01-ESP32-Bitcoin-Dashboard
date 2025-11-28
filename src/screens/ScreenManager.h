#ifndef SCREEN_MANAGER_H
#define SCREEN_MANAGER_H

#include <Arduino.h>
#include "../DisplayConfig.h"
#include <FT6X36.h>

// Screen types
enum Screen {
    SCREEN_WIFI_SCAN,
    SCREEN_WIFI_CONNECT,
    SCREEN_DASHBOARD
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

// Screen Manager
class ScreenManager {
private:
    LGFX* lcd;
    FT6X36* touch;
    BaseScreen* currentScreen;
    Screen currentScreenType;

    // Touch callback
    static ScreenManager* _instance;
    static void touchCallback(TPoint point, TEvent e);

public:
    ScreenManager(LGFX* display, FT6X36* touchController);
    void switchScreen(Screen screen);
    void update();
    void handleTouch();

    LGFX* getLCD() { return lcd; }
    FT6X36* getTouch() { return touch; }
    Screen getCurrentScreen() { return currentScreenType; }
};

#endif
