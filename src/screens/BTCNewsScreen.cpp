#include "BTCNewsScreen.h"

void BTCNewsScreen::init(ScreenManager* mgr) {
    manager = mgr;
    geminiClient = new GeminiClient();

    hasNews = false;
    isLoading = false;
    scrollOffset = 0;
    maxScrollOffset = 0;
    isDragging = false;
    animationFrame = 0;
    errorMessage = "";

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

    Serial.println("BTC News Screen initialized");
}

void BTCNewsScreen::update() {
    // Update touch feedback animations (non-blocking)
    feedback.update();

    LGFX* lcd = manager->getLCD();

    if (isLoading) {
        // Animate loading spinner
        animationFrame++;
        drawLoadingAnimation();
    }

    // Note: Drawing is handled in handleTouch and refresh methods
    // to avoid constant redrawing
}

void BTCNewsScreen::handleTouch(int16_t x, int16_t y) {
    Serial.printf("BTCNewsScreen touch: (%d, %d)\n", x, y);

    // Back button (top-left)
    if (x >= BACK_BTN_X && x <= BACK_BTN_X + BACK_BTN_SIZE &&
        y >= BACK_BTN_Y && y <= BACK_BTN_Y + BACK_BTN_SIZE) {
        Serial.println("Back button tapped - returning to dashboard");

        // Visual feedback - non-blocking flash
        feedback.flash(backButtonFeedbackId);

        manager->switchScreen(SCREEN_DASHBOARD);
        return;
    }

    // Refresh button (top-right)
    if (x >= REFRESH_BTN_X && x <= REFRESH_BTN_X + REFRESH_BTN_SIZE &&
        y >= REFRESH_BTN_Y && y <= REFRESH_BTN_Y + REFRESH_BTN_SIZE) {
        if (!isLoading) {
            Serial.println("Refresh button tapped");

            // Visual feedback - non-blocking flash
            feedback.flash(refreshButtonFeedbackId);

            refreshNews();
        }
        return;
    }

    // Scrolling (if content exists)
    if (hasNews && y > HEADER_HEIGHT) {
        if (!isDragging) {
            touchStartY = y;
            isDragging = true;
        } else {
            int delta = y - touchStartY;
            scrollOffset -= delta / 2; // Dampen scrolling

            // Clamp scroll offset
            if (scrollOffset < 0) scrollOffset = 0;
            if (scrollOffset > maxScrollOffset) scrollOffset = maxScrollOffset;

            touchStartY = y;

            // Redraw content with new offset
            drawNewsContent();
        }
    }
}

void BTCNewsScreen::setBTCData(const BTCData& data) {
    currentBTCData = data;
    Serial.println("BTC data set in News screen");
}

void BTCNewsScreen::refreshNews() {
    if (isLoading) return;

    Serial.println("Refreshing Bitcoin news...");
    isLoading = true;
    animationFrame = 0;
    errorMessage = "";

    LGFX* lcd = manager->getLCD();
    lcd->fillScreen(COLOR_BG);
    drawHeader();
    drawLoadingAnimation();

    // Fetch news from Gemini API
    bool success = geminiClient->fetchBitcoinNews(currentBTCData, newsText);

    isLoading = false;

    if (success && newsText.length() > 0) {
        hasNews = true;
        scrollOffset = 0;
        calculateMaxScroll();

        // Draw the news content
        lcd->fillScreen(COLOR_BG);
        drawHeader();
        drawNewsContent();

        Serial.println("News refreshed successfully!");
    } else {
        hasNews = false;
        errorMessage = newsText.length() > 0 ? newsText : "Failed to fetch news";

        lcd->fillScreen(COLOR_BG);
        drawHeader();
        drawErrorState();

        Serial.println("Failed to refresh news");
    }
}

void BTCNewsScreen::drawHeader() {
    LGFX* lcd = manager->getLCD();

    // Header background
    lcd->fillRect(0, 0, 480, HEADER_HEIGHT, COLOR_HEADER_BG);

    // Title
    lcd->setTextColor(COLOR_BTC_ORANGE);
    lcd->setFont(&fonts::FreeSansBold12pt7b);
    lcd->setCursor(50, 25);
    lcd->print("BTC News & Analysis");

    // Draw buttons
    drawBackButton();
    drawRefreshButton();

    // Bottom border
    lcd->drawLine(0, HEADER_HEIGHT, 480, HEADER_HEIGHT, COLOR_BTC_ORANGE);
}

void BTCNewsScreen::drawBackButton() {
    LGFX* lcd = manager->getLCD();

    // Draw back arrow "<"
    lcd->setTextColor(COLOR_TEXT_WHITE);
    lcd->setFont(&fonts::FreeSansBold12pt7b);
    lcd->setCursor(BACK_BTN_X, BACK_BTN_Y + 20);
    lcd->print("<");
}

void BTCNewsScreen::drawRefreshButton() {
    LGFX* lcd = manager->getLCD();

    // Draw refresh icon (circular arrow)
    lcd->setTextColor(isLoading ? COLOR_LOADING : COLOR_TEXT_WHITE);
    lcd->setFont(&fonts::FreeSansBold12pt7b);
    lcd->setCursor(REFRESH_BTN_X, REFRESH_BTN_Y + 20);
    lcd->print("@"); // Using @ as a simple refresh symbol

    // Alternative: draw a simple circular arrow
    // lcd->drawCircle(REFRESH_BTN_X + 15, REFRESH_BTN_Y + 15, 10, COLOR_TEXT_WHITE);
}

void BTCNewsScreen::drawLoadingAnimation() {
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

void BTCNewsScreen::drawNewsContent() {
    LGFX* lcd = manager->getLCD();

    // Clear content area (below header)
    lcd->fillRect(0, HEADER_HEIGHT + 1, 480, 320 - HEADER_HEIGHT - 1, COLOR_BG);

    // Set text parameters
    lcd->setFont(&fonts::FreeSans9pt7b);
    lcd->setTextColor(COLOR_TEXT_LIGHT);

    int x = NEWS_PADDING;
    int y = HEADER_HEIGHT + NEWS_PADDING + 15 - scrollOffset;
    int lineHeight = NEWS_LINE_HEIGHT;
    int maxWidth = 480 - (NEWS_PADDING * 2);

    // Split text into lines and render
    int startPos = 0;
    int lineCount = 0;

    while (startPos < newsText.length() && lineCount < NEWS_MAX_SCROLL_LINES) {
        // Find next newline or end of string
        int endPos = newsText.indexOf('\n', startPos);
        if (endPos == -1) endPos = newsText.length();

        String line = newsText.substring(startPos, endPos);

        // Check if line fits on screen vertically
        if (y >= HEADER_HEIGHT && y < 320) {
            // Simple word wrapping (split long lines)
            if (line.length() > 60) {
                // Split into chunks
                for (int i = 0; i < line.length(); i += 60) {
                    String chunk = line.substring(i, min((int)line.length(), i + 60));
                    if (y >= HEADER_HEIGHT && y < 320) {
                        lcd->setCursor(x, y);
                        lcd->print(chunk);
                    }
                    y += lineHeight;
                    lineCount++;
                }
            } else {
                lcd->setCursor(x, y);
                lcd->print(line);
                y += lineHeight;
                lineCount++;
            }
        } else {
            y += lineHeight;
            lineCount++;
        }

        startPos = endPos + 1;
    }

    // Draw scroll indicator if needed
    if (maxScrollOffset > 0) {
        int scrollBarHeight = 200;
        int scrollBarY = HEADER_HEIGHT + 10;
        int scrollIndicatorY = scrollBarY + (scrollOffset * scrollBarHeight / maxScrollOffset);

        lcd->drawRect(475, scrollBarY, 3, scrollBarHeight, COLOR_TEXT_DIM);
        lcd->fillRect(475, scrollIndicatorY, 3, 20, COLOR_BTC_ORANGE);
    }

    // Swipe hint at bottom
    lcd->setTextColor(COLOR_TEXT_DIM);
    lcd->setFont(&fonts::FreeSans9pt7b);
    lcd->setCursor(150, 310);
    lcd->print("< Swipe to Dashboard");
}

void BTCNewsScreen::drawEmptyState() {
    LGFX* lcd = manager->getLCD();

    int centerX = 240;
    int centerY = 180;

    lcd->setTextColor(COLOR_TEXT_DIM);
    lcd->setFont(&fonts::FreeSans12pt7b);

    lcd->setCursor(centerX - 100, centerY - 20);
    lcd->print("No news loaded");

    lcd->setFont(&fonts::FreeSans9pt7b);
    lcd->setCursor(centerX - 120, centerY + 20);
    lcd->print("Tap @ to fetch latest news");

    // Swipe hint
    lcd->setTextColor(COLOR_TEXT_DIM);
    lcd->setCursor(150, 310);
    lcd->print("< Swipe to Dashboard");
}

void BTCNewsScreen::drawErrorState() {
    LGFX* lcd = manager->getLCD();

    int centerX = 240;
    int centerY = 180;

    lcd->setTextColor(COLOR_ERROR);
    lcd->setFont(&fonts::FreeSans12pt7b);
    lcd->setCursor(centerX - 80, centerY - 40);
    lcd->print("Error");

    lcd->setTextColor(COLOR_TEXT_LIGHT);
    lcd->setFont(&fonts::FreeSans9pt7b);

    // Word wrap error message
    int y = centerY;
    int maxWidth = 400;
    int startPos = 0;

    while (startPos < errorMessage.length()) {
        int endPos = min((int)errorMessage.length(), startPos + 50);
        String chunk = errorMessage.substring(startPos, endPos);

        lcd->setCursor(centerX - 150, y);
        lcd->print(chunk);
        y += 20;
        startPos = endPos;
    }

    // Retry hint
    lcd->setTextColor(COLOR_TEXT_DIM);
    lcd->setCursor(centerX - 100, y + 30);
    lcd->print("Tap @ to try again");

    // Swipe hint
    lcd->setCursor(150, 310);
    lcd->print("< Swipe to Dashboard");
}

void BTCNewsScreen::calculateMaxScroll() {
    // Calculate approximate number of lines
    int lineCount = 1;
    for (int i = 0; i < newsText.length(); i++) {
        if (newsText.charAt(i) == '\n') lineCount++;
    }

    // Add extra lines for word-wrapped content
    lineCount *= 1.2; // Rough estimate

    int totalHeight = lineCount * NEWS_LINE_HEIGHT;
    int visibleHeight = 320 - HEADER_HEIGHT - (NEWS_PADDING * 2);

    maxScrollOffset = max(0, totalHeight - visibleHeight);

    Serial.printf("Calculated max scroll: %d (lines: %d, totalHeight: %d)\n",
                  maxScrollOffset, lineCount, totalHeight);
}

void BTCNewsScreen::wrapText(const String& text, int maxWidth, int fontSize) {
    // TODO: Implement proper word wrapping based on font metrics
    // For now, using simple character-based wrapping in drawNewsContent
}
