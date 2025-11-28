#include "ScreenManager.h"
#include "WiFiScanScreen.h"
#include "BTCDashboardScreen.h"

ScreenManager* ScreenManager::_instance = nullptr;

ScreenManager::ScreenManager(LGFX* display, FT6X36* touchController) {
    lcd = display;
    touch = touchController;
    currentScreen = nullptr;
    currentScreenType = SCREEN_WIFI_SCAN;
    _instance = this;

    // Register touch callback
    touch->registerTouchHandler(touchCallback);
}

void ScreenManager::switchScreen(Screen screen) {
    // Delete old screen
    if (currentScreen != nullptr) {
        delete currentScreen;
    }

    currentScreenType = screen;

    // Create new screen
    switch (screen) {
        case SCREEN_WIFI_SCAN:
            currentScreen = new WiFiScanScreen();
            break;

        case SCREEN_WIFI_CONNECT:
            // TODO: Implement WiFi connect screen
            Serial.println("WiFi Connect screen not yet implemented");
            break;

        case SCREEN_DASHBOARD:
            currentScreen = new BTCDashboardScreen();
            break;
    }

    if (currentScreen != nullptr) {
        currentScreen->init(this);
    }
}

void ScreenManager::update() {
    // Process touch events
    touch->loop();

    // Update current screen
    if (currentScreen != nullptr) {
        currentScreen->update();
    }
}

void ScreenManager::handleTouch() {
    // Touch is handled via callback, this method is for compatibility
}

void ScreenManager::touchCallback(TPoint point, TEvent e) {
    if (_instance == nullptr || e != TEvent::Tap) return;

    // Transform coordinates for landscape mode (rotation 1)
    int16_t transformedX = point.y;
    int16_t transformedY = 320 - point.x;

    Serial.printf("Touch event: raw(%d,%d) -> transformed(%d,%d)\n",
                 point.x, point.y, transformedX, transformedY);

    if (_instance->currentScreen != nullptr) {
        _instance->currentScreen->handleTouch(transformedX, transformedY);
    }
}
