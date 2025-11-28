#include "ScreenManager.h"
#include "WiFiScanScreen.h"
#include "BTCDashboardScreen.h"
#include "BTCNewsScreen.h"
#include "SettingsScreen.h"

ScreenManager* ScreenManager::_instance = nullptr;

ScreenManager::ScreenManager(LGFX* display, FT6X36* touchController) {
    lcd = display;
    touch = touchController;
    currentScreen = nullptr;
    currentScreenType = SCREEN_WIFI_SCAN;
    _instance = this;

    // Initialize swipe gesture
    swipeGesture.isActive = false;

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

        case SCREEN_BTC_NEWS:
            currentScreen = new BTCNewsScreen();
            break;

        case SCREEN_SETTINGS:
            currentScreen = new SettingsScreen();
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
    if (_instance == nullptr) return;

    // Transform coordinates for landscape mode (rotation 1)
    int16_t transformedX = point.y;
    int16_t transformedY = 320 - point.x;

    // Handle different touch events
    if (e == TEvent::Tap) {
        Serial.printf("Touch event: raw(%d,%d) -> transformed(%d,%d)\n",
                     point.x, point.y, transformedX, transformedY);

        if (_instance->currentScreen != nullptr) {
            _instance->currentScreen->handleTouch(transformedX, transformedY);
        }
    }
    else if (e == TEvent::DragStart) {
        // Start tracking swipe gesture
        _instance->swipeGesture.startX = transformedX;
        _instance->swipeGesture.startY = transformedY;
        _instance->swipeGesture.startTime = millis();
        _instance->swipeGesture.isActive = true;

        Serial.printf("Swipe start: (%d, %d)\n", transformedX, transformedY);
    }
    else if (e == TEvent::DragEnd) {
        if (_instance->swipeGesture.isActive) {
            // End tracking and evaluate swipe
            _instance->swipeGesture.endX = transformedX;
            _instance->swipeGesture.endY = transformedY;
            unsigned long duration = millis() - _instance->swipeGesture.startTime;

            int16_t deltaX = _instance->swipeGesture.endX - _instance->swipeGesture.startX;
            int16_t deltaY = _instance->swipeGesture.endY - _instance->swipeGesture.startY;

            Serial.printf("Swipe end: delta(%d, %d) duration=%lu\n", deltaX, deltaY, duration);

            _instance->handleSwipe(deltaX, deltaY, duration);
            _instance->swipeGesture.isActive = false;
        }
    }
}

void ScreenManager::handleSwipe(int16_t deltaX, int16_t deltaY, unsigned long duration) {
    // Check if this is a valid horizontal swipe
    if (!isHorizontalSwipe(deltaX, deltaY)) {
        Serial.println("Not a horizontal swipe, ignoring");
        return;
    }

    // Check duration
    if (duration > SWIPE_MAX_TIME) {
        Serial.printf("Swipe too slow (%lu ms), ignoring\n", duration);
        return;
    }

    // Check distance
    int absX = abs(deltaX);
    if (absX < SWIPE_MIN_DISTANCE) {
        Serial.printf("Swipe too short (%d px), ignoring\n", absX);
        return;
    }

    // Determine swipe direction and handle navigation
    if (deltaX > 0) {
        // Right swipe
        Serial.println("Right swipe detected!");
        if (currentScreenType == SCREEN_BTC_NEWS) {
            Serial.println("Navigating from News to Dashboard");
            switchScreen(SCREEN_DASHBOARD);
        }
    } else {
        // Left swipe
        Serial.println("Left swipe detected!");
        if (currentScreenType == SCREEN_DASHBOARD) {
            Serial.println("Navigating from Dashboard to News");

            // Get current BTC data from dashboard
            BTCDashboardScreen* dashboard = static_cast<BTCDashboardScreen*>(currentScreen);
            BTCData currentData = dashboard->getBTCData();

            // Switch to news screen
            switchScreen(SCREEN_BTC_NEWS);

            // Pass data to news screen
            BTCNewsScreen* newsScreen = static_cast<BTCNewsScreen*>(currentScreen);
            newsScreen->setBTCData(currentData);
        }
    }
}

bool ScreenManager::isHorizontalSwipe(int16_t deltaX, int16_t deltaY) {
    // Horizontal swipe if: |deltaX| > |deltaY| and |deltaY| < threshold
    int absX = abs(deltaX);
    int absY = abs(deltaY);

    return (absX > absY) && (absY < SWIPE_THRESHOLD_Y);
}
