#include "ScreenManager.h"
#ifndef SINGLE_SCREEN_MODE
#include "WiFiScanScreen.h"
#endif
#include "BTCDashboardScreen.h"
#ifndef SINGLE_SCREEN_MODE
#include "BTCNewsScreen.h"
#include "BTCTradingSuggestionScreen.h"
#include "SettingsScreen.h"
#endif

ScreenManager* ScreenManager::_instance = nullptr;

ScreenManager::ScreenManager(LGFX* display, FT6X36* touchController) {
    lcd = display;
    touch = touchController;
    currentScreen = nullptr;
#ifdef SINGLE_SCREEN_MODE
    currentScreenType = SCREEN_DASHBOARD;
#else
    currentScreenType = SCREEN_WIFI_SCAN;
#endif
    _instance = this;

#ifndef SINGLE_SCREEN_MODE
    // Initialize swipe gesture
    swipeGesture.isActive = false;
#endif

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
#ifndef SINGLE_SCREEN_MODE
        case SCREEN_WIFI_SCAN:
            currentScreen = new WiFiScanScreen();
            break;

        case SCREEN_WIFI_CONNECT:
            // TODO: Implement WiFi connect screen
            Serial.println("WiFi Connect screen not yet implemented");
            break;
#endif

        case SCREEN_DASHBOARD:
            currentScreen = new BTCDashboardScreen();
            break;

#ifndef SINGLE_SCREEN_MODE
        case SCREEN_BTC_NEWS:
            currentScreen = new BTCNewsScreen();
            break;

        case SCREEN_TRADING_SUGGESTION:
            currentScreen = new BTCTradingSuggestionScreen();
            break;

        case SCREEN_SETTINGS:
            currentScreen = new SettingsScreen();
            break;
#endif
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

#ifdef SINGLE_SCREEN_MODE
    // In single screen mode, only handle taps (no swipe navigation)
    if (e == TEvent::TouchEnd || e == TEvent::Tap) {
        Serial.printf("Tap at: (%d, %d)\n", transformedX, transformedY);
        if (_instance->currentScreen != nullptr) {
            _instance->currentScreen->handleTouch(transformedX, transformedY);
        }
    }
#else
    // Handle different touch events with swipe gesture support
    if (e == TEvent::TouchStart) {
        // Start tracking potential swipe gesture
        _instance->swipeGesture.startX = transformedX;
        _instance->swipeGesture.startY = transformedY;
        _instance->swipeGesture.startTime = millis();
        _instance->swipeGesture.isActive = true;

        Serial.printf("Touch start: (%d, %d)\n", transformedX, transformedY);
    }
    else if (e == TEvent::TouchMove) {
        // Track movement for swipe detection
        if (_instance->swipeGesture.isActive) {
            _instance->swipeGesture.endX = transformedX;
            _instance->swipeGesture.endY = transformedY;
        }
    }
    else if (e == TEvent::TouchEnd) {
        if (_instance->swipeGesture.isActive) {
            // End tracking and evaluate if this was a swipe or tap
            _instance->swipeGesture.endX = transformedX;
            _instance->swipeGesture.endY = transformedY;
            unsigned long duration = millis() - _instance->swipeGesture.startTime;

            int16_t deltaX = _instance->swipeGesture.endX - _instance->swipeGesture.startX;
            int16_t deltaY = _instance->swipeGesture.endY - _instance->swipeGesture.startY;

            Serial.printf("Touch end: delta(%d, %d) duration=%lu\n", deltaX, deltaY, duration);

            // Check if this was a swipe gesture
            int absX = abs(deltaX);
            int absY = abs(deltaY);

            if (absX >= SWIPE_MIN_DISTANCE && absX > absY && duration <= SWIPE_MAX_TIME) {
                // This is a horizontal swipe
                Serial.println("Swipe detected!");
                _instance->handleSwipe(deltaX, deltaY, duration);
            } else {
                // This is a tap
                Serial.printf("Tap detected at: (%d, %d)\n", transformedX, transformedY);
                if (_instance->currentScreen != nullptr) {
                    _instance->currentScreen->handleTouch(transformedX, transformedY);
                }
            }

            _instance->swipeGesture.isActive = false;
        }
    }
    else if (e == TEvent::Tap) {
        // Direct tap event (fallback)
        Serial.printf("Direct tap event: (%d, %d)\n", transformedX, transformedY);
        if (_instance->currentScreen != nullptr) {
            _instance->currentScreen->handleTouch(transformedX, transformedY);
        }
    }
#endif
}

#ifndef SINGLE_SCREEN_MODE
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

    // Get current BTC data to share across screens
    BTCData currentData;
    bool hasData = false;

    if (currentScreenType == SCREEN_DASHBOARD) {
        BTCDashboardScreen* dashboard = static_cast<BTCDashboardScreen*>(currentScreen);
        currentData = dashboard->getBTCData();
        hasData = true;
    }

    // Determine swipe direction and handle circular navigation
    if (deltaX > 0) {
        // Right swipe (clockwise: Dashboard → Trading → News → Dashboard)
        Serial.println("Right swipe detected!");

        switch (currentScreenType) {
            case SCREEN_DASHBOARD:
                // Dashboard → Trading
                Serial.println("Navigating from Dashboard to Trading");
                switchScreen(SCREEN_TRADING_SUGGESTION);
                if (hasData) {
                    BTCTradingSuggestionScreen* trading =
                        static_cast<BTCTradingSuggestionScreen*>(currentScreen);
                    trading->setBTCData(currentData);
                }
                break;

            case SCREEN_BTC_NEWS:
                // News → Dashboard
                Serial.println("Navigating from News to Dashboard");
                switchScreen(SCREEN_DASHBOARD);
                break;

            case SCREEN_TRADING_SUGGESTION:
                // Trading → News
                Serial.println("Navigating from Trading to News");
                switchScreen(SCREEN_BTC_NEWS);
                if (hasData) {
                    BTCNewsScreen* news = static_cast<BTCNewsScreen*>(currentScreen);
                    news->setBTCData(currentData);
                }
                break;

            default:
                Serial.println("Swipe not handled for this screen");
                break;
        }
    } else {
        // Left swipe (counter-clockwise: Dashboard → News → Trading → Dashboard)
        Serial.println("Left swipe detected!");

        switch (currentScreenType) {
            case SCREEN_DASHBOARD:
                // Dashboard → News
                Serial.println("Navigating from Dashboard to News");
                switchScreen(SCREEN_BTC_NEWS);
                if (hasData) {
                    BTCNewsScreen* news = static_cast<BTCNewsScreen*>(currentScreen);
                    news->setBTCData(currentData);
                }
                break;

            case SCREEN_BTC_NEWS:
                // News → Trading
                Serial.println("Navigating from News to Trading");
                switchScreen(SCREEN_TRADING_SUGGESTION);
                if (hasData) {
                    BTCTradingSuggestionScreen* trading =
                        static_cast<BTCTradingSuggestionScreen*>(currentScreen);
                    trading->setBTCData(currentData);
                }
                break;

            case SCREEN_TRADING_SUGGESTION:
                // Trading → Dashboard (completes circle)
                Serial.println("Navigating from Trading to Dashboard");
                switchScreen(SCREEN_DASHBOARD);
                break;

            default:
                Serial.println("Swipe not handled for this screen");
                break;
        }
    }
}

bool ScreenManager::isHorizontalSwipe(int16_t deltaX, int16_t deltaY) {
    // Horizontal swipe if: |deltaX| > |deltaY| and |deltaY| < threshold
    int absX = abs(deltaX);
    int absY = abs(deltaY);

    return (absX > absY) && (absY < SWIPE_THRESHOLD_Y);
}
#endif // SINGLE_SCREEN_MODE
