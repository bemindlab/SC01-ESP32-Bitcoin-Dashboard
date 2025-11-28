#include "BTCTradingSuggestionScreen.h"
#include "../Config.h"

extern ConfigManager globalConfig;

void BTCTradingSuggestionScreen::init(ScreenManager* mgr) {
    manager = mgr;
    openaiClient = new OpenAIClient();

    // Get API key from config
    String apiKey = globalConfig.getOpenAIApiKey();
    if (apiKey.length() > 0) {
        openaiClient->setApiKey(apiKey);
    }

    hasSuggestion = false;
    isLoading = false;
    scrollOffset = 0;
    maxScrollOffset = 0;
    isDragging = false;
    animationFrame = 0;
    errorMessage = "";
    lastUpdate = 0;

    // Initialize touch feedback
    feedback.init(manager->getLCD());

    // Clear screen
    LGFX* lcd = manager->getLCD();
    lcd->fillScreen(COLOR_BG);

    // Draw initial UI
    drawHeader();
    drawEmptyState();

    // Register touch feedback for buttons
    backButtonFeedbackId = feedback.registerIcon(
        BACK_BTN_X, BACK_BTN_Y, BACK_BTN_SIZE,
        COLOR_HEADER_BG, COLOR_BTC_ORANGE, 200
    );

    refreshButtonFeedbackId = feedback.registerIcon(
        REFRESH_BTN_X, REFRESH_BTN_Y, REFRESH_BTN_SIZE,
        COLOR_HEADER_BG, COLOR_BTC_ORANGE, 200
    );

    Serial.println("BTC Trading Suggestion Screen initialized");
}

void BTCTradingSuggestionScreen::update() {
    // Update touch feedback animations
    feedback.update();

    LGFX* lcd = manager->getLCD();

    if (isLoading) {
        // Animate loading spinner
        animationFrame++;
        drawLoadingAnimation();
    }

    // Auto-refresh if interval elapsed
    if (hasSuggestion && !isLoading) {
        unsigned long now = millis();
        if (now - lastUpdate >= UPDATE_INTERVAL) {
            refreshSuggestion();
        }
    }
}

void BTCTradingSuggestionScreen::handleTouch(int16_t x, int16_t y) {
    Serial.printf("BTCTradingSuggestionScreen touch: (%d, %d)\n", x, y);

    // Back button (top-left)
    if (x >= BACK_BTN_X && x <= BACK_BTN_X + BACK_BTN_SIZE &&
        y >= BACK_BTN_Y && y <= BACK_BTN_Y + BACK_BTN_SIZE) {
        Serial.println("Back button tapped - returning to dashboard");

        // Visual feedback
        feedback.flash(backButtonFeedbackId);

        manager->switchScreen(SCREEN_DASHBOARD);
        return;
    }

    // Refresh button (top-right)
    if (x >= REFRESH_BTN_X && x <= REFRESH_BTN_X + REFRESH_BTN_SIZE &&
        y >= REFRESH_BTN_Y && y <= REFRESH_BTN_Y + REFRESH_BTN_SIZE) {
        if (!isLoading) {
            Serial.println("Refresh button tapped");

            // Visual feedback
            feedback.flash(refreshButtonFeedbackId);

            refreshSuggestion();
        }
        return;
    }

    // Scrolling (if content exists)
    if (hasSuggestion && y > HEADER_HEIGHT) {
        if (!isDragging) {
            touchStartY = y;
            isDragging = true;
        } else {
            int delta = y - touchStartY;
            scrollOffset -= delta / 2;

            // Clamp scroll offset
            if (scrollOffset < 0) scrollOffset = 0;
            if (scrollOffset > maxScrollOffset) scrollOffset = maxScrollOffset;

            touchStartY = y;

            // Redraw content with new offset
            drawRecommendation();
        }
    }
}

void BTCTradingSuggestionScreen::setBTCData(const BTCData& data) {
    currentBTCData = data;
    Serial.println("BTC data set in Trading screen");

    // Auto-fetch suggestion if we don't have one
    if (!hasSuggestion && !isLoading) {
        refreshSuggestion();
    }
}

void BTCTradingSuggestionScreen::refreshSuggestion() {
    if (isLoading) return;

    // Check if API key is configured
    String apiKey = globalConfig.getOpenAIApiKey();
    if (apiKey.length() == 0) {
        errorMessage = "OpenAI API key not configured. Use serial command: SET_OPENAI_KEY=sk-proj-...";
        LGFX* lcd = manager->getLCD();
        lcd->fillScreen(COLOR_BG);
        drawHeader();
        drawErrorState();
        return;
    }

    Serial.println("Refreshing trading suggestion...");
    isLoading = true;
    animationFrame = 0;
    errorMessage = "";

    LGFX* lcd = manager->getLCD();
    lcd->fillScreen(COLOR_BG);
    drawHeader();
    drawLoadingAnimation();

    // Fetch suggestion from OpenAI
    bool success = openaiClient->fetchTradingSuggestion(currentBTCData, suggestion);

    isLoading = false;

    if (success && suggestion.isValid) {
        hasSuggestion = true;
        scrollOffset = 0;
        lastUpdate = millis();
        calculateMaxScroll();

        // Draw the suggestion
        lcd->fillScreen(COLOR_BG);
        drawHeader();
        drawMarketSummary();
        drawSignalBadge();
        drawRecommendation();
        drawDisclaimer();

        Serial.println("Trading suggestion refreshed successfully!");
    } else {
        hasSuggestion = false;
        errorMessage = "Failed to fetch trading suggestion. Check API key and internet connection.";

        lcd->fillScreen(COLOR_BG);
        drawHeader();
        drawErrorState();

        Serial.println("Failed to refresh trading suggestion");
    }
}

uint32_t BTCTradingSuggestionScreen::getSignalColor(TradingSignal signal) {
    switch (signal) {
        case SIGNAL_STRONG_BUY: return COLOR_STRONG_BUY;
        case SIGNAL_BUY: return COLOR_BUY;
        case SIGNAL_HOLD: return COLOR_HOLD;
        case SIGNAL_SELL: return COLOR_SELL;
        case SIGNAL_STRONG_SELL: return COLOR_STRONG_SELL;
        default: return COLOR_UNCERTAIN;
    }
}

const char* BTCTradingSuggestionScreen::getSignalText(TradingSignal signal) {
    switch (signal) {
        case SIGNAL_STRONG_BUY: return "STRONG BUY";
        case SIGNAL_BUY: return "BUY";
        case SIGNAL_HOLD: return "HOLD";
        case SIGNAL_SELL: return "SELL";
        case SIGNAL_STRONG_SELL: return "STRONG SELL";
        default: return "UNCERTAIN";
    }
}

const char* BTCTradingSuggestionScreen::getSignalIcon(TradingSignal signal) {
    switch (signal) {
        case SIGNAL_STRONG_BUY: return "^^";
        case SIGNAL_BUY: return "^";
        case SIGNAL_HOLD: return "=";
        case SIGNAL_SELL: return "v";
        case SIGNAL_STRONG_SELL: return "vv";
        default: return "?";
    }
}

void BTCTradingSuggestionScreen::drawHeader() {
    LGFX* lcd = manager->getLCD();

    // Header background
    lcd->fillRect(0, 0, 480, HEADER_HEIGHT, COLOR_HEADER_BG);

    // Title
    lcd->setTextColor(COLOR_BTC_ORANGE);
    lcd->setFont(&fonts::FreeSansBold12pt7b);
    lcd->setCursor(50, 25);
    lcd->print("Trading Analysis");

    // Draw buttons
    drawBackButton();
    drawRefreshButton();

    // Bottom border
    lcd->drawLine(0, HEADER_HEIGHT, 480, HEADER_HEIGHT, COLOR_BTC_ORANGE);
}

void BTCTradingSuggestionScreen::drawBackButton() {
    LGFX* lcd = manager->getLCD();

    // Draw back arrow "<"
    lcd->setTextColor(COLOR_TEXT_WHITE);
    lcd->setFont(&fonts::FreeSansBold12pt7b);
    lcd->setCursor(BACK_BTN_X, BACK_BTN_Y + 20);
    lcd->print("<");
}

void BTCTradingSuggestionScreen::drawRefreshButton() {
    LGFX* lcd = manager->getLCD();

    // Draw refresh icon
    lcd->setTextColor(isLoading ? COLOR_LOADING : COLOR_TEXT_WHITE);
    lcd->setFont(&fonts::FreeSansBold12pt7b);
    lcd->setCursor(REFRESH_BTN_X, REFRESH_BTN_Y + 20);
    lcd->print("@");
}

void BTCTradingSuggestionScreen::drawMarketSummary() {
    LGFX* lcd = manager->getLCD();

    int y = HEADER_HEIGHT + 10;

    // Market context box
    lcd->drawRoundRect(10, y, 460, 60, 5, COLOR_BTC_ORANGE);

    lcd->setTextColor(COLOR_TEXT_LIGHT);
    lcd->setFont(&fonts::FreeSans9pt7b);

    // Price
    lcd->setCursor(20, y + 20);
    lcd->printf("BTC: $%.0f", currentBTCData.priceUSD);

    // Block & Mempool
    lcd->setCursor(20, y + 40);
    lcd->printf("Block: %d | Mempool: %d TXs",
               currentBTCData.blockHeight,
               currentBTCData.mempoolCount);
}

void BTCTradingSuggestionScreen::drawSignalBadge() {
    LGFX* lcd = manager->getLCD();

    int y = HEADER_HEIGHT + 80;

    // Signal
    uint32_t signalColor = getSignalColor(suggestion.signal);
    lcd->setTextColor(signalColor);
    lcd->setFont(&fonts::FreeSansBold12pt7b);
    lcd->setCursor(20, y + 20);
    lcd->print(getSignalIcon(suggestion.signal));
    lcd->print(" ");
    lcd->print(getSignalText(suggestion.signal));

    // Confidence bar
    lcd->setCursor(20, y + 45);
    lcd->setTextColor(COLOR_TEXT_LIGHT);
    lcd->setFont(&fonts::FreeSans9pt7b);
    lcd->printf("Confidence: %d%%", suggestion.confidence);

    // Draw confidence bar
    int barWidth = (suggestion.confidence * 200) / 100;
    lcd->fillRect(150, y + 35, barWidth, 15, signalColor);
    lcd->drawRect(150, y + 35, 200, 15, COLOR_TEXT_DIM);
}

void BTCTradingSuggestionScreen::drawRecommendation() {
    LGFX* lcd = manager->getLCD();

    int startY = HEADER_HEIGHT + 135;
    int endY = 270;  // Leave space for disclaimer

    // Clear content area
    lcd->fillRect(0, startY, 480, endY - startY, COLOR_BG);

    // Title
    lcd->setTextColor(COLOR_BTC_ORANGE);
    lcd->setFont(&fonts::FreeSansBold9pt7b);
    lcd->setCursor(20, startY + 15);
    lcd->print("Recommendation:");

    // Recommendation text
    lcd->setTextColor(COLOR_TEXT_LIGHT);
    lcd->setFont(&fonts::FreeSans9pt7b);

    int y = startY + 40 - scrollOffset;
    int lineHeight = TRADING_LINE_HEIGHT;

    // Word wrap recommendation
    String text = suggestion.recommendation;
    int startPos = 0;

    while (startPos < text.length() && y < endY) {
        int endPos = min((int)text.length(), startPos + 50);

        // Try to break at space
        if (endPos < text.length()) {
            int spacePos = text.lastIndexOf(' ', endPos);
            if (spacePos > startPos) {
                endPos = spacePos;
            }
        }

        String line = text.substring(startPos, endPos);

        if (y >= startY && y < endY) {
            lcd->setCursor(20, y);
            lcd->print(line);
        }

        y += lineHeight;
        startPos = endPos + 1;
    }

    // Key factors
    if (suggestion.keyFactorCount > 0) {
        y += 10;
        if (y >= startY && y < endY) {
            lcd->setTextColor(COLOR_BTC_ORANGE);
            lcd->setFont(&fonts::FreeSansBold9pt7b);
            lcd->setCursor(20, y);
            lcd->print("Key Factors:");
        }

        y += 20;

        for (int i = 0; i < suggestion.keyFactorCount; i++) {
            if (y >= endY) break;

            lcd->setTextColor(COLOR_TEXT_LIGHT);
            lcd->setFont(&fonts::FreeSans9pt7b);

            if (y >= startY && y < endY) {
                lcd->setCursor(20, y);
                lcd->print("- ");
                lcd->print(suggestion.keyFactors[i].substring(0, 45));
            }

            y += lineHeight;
        }
    }

    // Swipe hint
    lcd->setTextColor(COLOR_TEXT_DIM);
    lcd->setFont(&fonts::FreeSans9pt7b);
    lcd->setCursor(120, 305);
    lcd->print("< Swipe to Dashboard >");
}

void BTCTradingSuggestionScreen::drawLoadingAnimation() {
    LGFX* lcd = manager->getLCD();

    int centerX = 240;
    int centerY = 180;

    // Clear loading area
    lcd->fillRect(centerX - 100, centerY - 50, 200, 100, COLOR_BG);

    // Spinning animation
    int spinnerRadius = 20;
    int angle = (animationFrame * 10) % 360;
    int x1 = centerX + spinnerRadius * cos(angle * PI / 180);
    int y1 = centerY + spinnerRadius * sin(angle * PI / 180);

    lcd->fillCircle(x1, y1, 5, COLOR_LOADING);

    // Loading text
    lcd->setTextColor(COLOR_TEXT_LIGHT);
    lcd->setFont(&fonts::FreeSans9pt7b);
    lcd->setCursor(centerX - 80, centerY + 40);
    lcd->print("Analyzing market...");
}

void BTCTradingSuggestionScreen::drawEmptyState() {
    LGFX* lcd = manager->getLCD();

    int centerX = 240;
    int centerY = 180;

    lcd->setTextColor(COLOR_TEXT_DIM);
    lcd->setFont(&fonts::FreeSans12pt7b);

    lcd->setCursor(centerX - 120, centerY - 20);
    lcd->print("No suggestion loaded");

    lcd->setFont(&fonts::FreeSans9pt7b);
    lcd->setCursor(centerX - 130, centerY + 20);
    lcd->print("Tap @ to fetch AI analysis");

    // Swipe hint
    lcd->setTextColor(COLOR_TEXT_DIM);
    lcd->setCursor(120, 305);
    lcd->print("< Swipe to Dashboard >");
}

void BTCTradingSuggestionScreen::drawErrorState() {
    LGFX* lcd = manager->getLCD();

    int centerX = 240;
    int centerY = 160;

    lcd->setTextColor(COLOR_ERROR);
    lcd->setFont(&fonts::FreeSans12pt7b);
    lcd->setCursor(centerX - 50, centerY - 40);
    lcd->print("Error");

    lcd->setTextColor(COLOR_TEXT_LIGHT);
    lcd->setFont(&fonts::FreeSans9pt7b);

    // Word wrap error message
    int y = centerY;
    int startPos = 0;

    while (startPos < errorMessage.length() && y < 280) {
        int endPos = min((int)errorMessage.length(), startPos + 45);
        String chunk = errorMessage.substring(startPos, endPos);

        lcd->setCursor(20, y);
        lcd->print(chunk);
        y += 20;
        startPos = endPos;
    }

    // Retry hint
    lcd->setTextColor(COLOR_TEXT_DIM);
    lcd->setCursor(centerX - 100, y + 30);
    lcd->print("Tap @ to try again");

    // Swipe hint
    lcd->setCursor(120, 305);
    lcd->print("< Swipe to Dashboard >");
}

void BTCTradingSuggestionScreen::drawDisclaimer() {
    LGFX* lcd = manager->getLCD();

    lcd->setTextColor(COLOR_TEXT_DIM);
    lcd->setFont(&fonts::FreeSans9pt7b);
    lcd->setCursor(30, 285);
    lcd->print("Not financial advice. Trade at own risk.");
}

void BTCTradingSuggestionScreen::calculateMaxScroll() {
    // Estimate scroll needed based on content
    int lineCount = 0;

    // Recommendation lines
    lineCount += (suggestion.recommendation.length() / 50) + 2;

    // Key factors
    lineCount += suggestion.keyFactorCount + 2;

    int totalHeight = lineCount * TRADING_LINE_HEIGHT;
    int visibleHeight = 270 - (HEADER_HEIGHT + 135);

    maxScrollOffset = max(0, totalHeight - visibleHeight);

    Serial.printf("Calculated max scroll: %d (lines: %d, totalHeight: %d)\n",
                  maxScrollOffset, lineCount, totalHeight);
}
