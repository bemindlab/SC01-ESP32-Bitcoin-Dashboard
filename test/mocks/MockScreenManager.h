#ifndef MOCK_SCREEN_MANAGER_H
#define MOCK_SCREEN_MANAGER_H

#include "MockLGFX.h"
#include "MockFT6X36.h"

// Screen IDs (matching ScreenManager.h)
enum ScreenID {
    SCREEN_DASHBOARD = 0,
    SCREEN_WIFI_SCAN,
    SCREEN_SETTINGS,
    SCREEN_NEWS,
    SCREEN_TRADING
};

/**
 * Mock ScreenManager for native unit testing
 * Simulates screen management without hardware
 */
class MockScreenManager {
public:
    MockScreenManager() : currentScreen(SCREEN_DASHBOARD), switchCount(0) {
        lcd = new MockLGFX();
        touch = new MockFT6X36();
    }

    ~MockScreenManager() {
        delete lcd;
        delete touch;
    }

    // Screen management
    void switchScreen(ScreenID screen) {
        lastScreen = currentScreen;
        currentScreen = screen;
        switchCount++;
    }

    ScreenID getCurrentScreen() { return currentScreen; }
    ScreenID getLastScreen() { return lastScreen; }
    uint32_t getSwitchCount() { return switchCount; }

    // Hardware access
    MockLGFX* getLCD() { return lcd; }
    MockFT6X36* getTouch() { return touch; }

    // Reset for testing
    void reset() {
        currentScreen = SCREEN_DASHBOARD;
        lastScreen = SCREEN_DASHBOARD;
        switchCount = 0;
        lcd->fillScreen(0);
    }

private:
    MockLGFX* lcd;
    MockFT6X36* touch;
    ScreenID currentScreen;
    ScreenID lastScreen;
    uint32_t switchCount;
};

#endif // MOCK_SCREEN_MANAGER_H
