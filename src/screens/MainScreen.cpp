#include "MainScreen.h"
#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include "../api/GeminiClient.h"

// Global BTC data instance
BTCData btcData;

void MainScreen::init(ScreenManager* mgr) {
    manager = mgr;
    LGFX* lcd = manager->getLCD();

    lcd->fillScreen(0x000000);

    // Calculate max vertical scroll (total content height - visible area)
    // 8 cards × 88px (80 + 8 spacing) = 704px
    // Visible area = 320 - 28 (header) = 292px
    maxScrollY = (8 * 88) - 292;
    if (maxScrollY < 0) maxScrollY = 0;

    // Calculate max horizontal scroll
    // 4 columns × 236px (228 + 8 spacing) = 944px
    // Visible area = 480px
    maxScrollX = 944 - 480;
    if (maxScrollX < 0) maxScrollX = 0;

    drawHeader();

    // Fetch initial data if WiFi connected
    if (WiFi.status() == WL_CONNECTED) {
        Serial.println("Fetching initial BTC data...");
        fetchBTCPrice();
        fetchBlockData();
        fetchMempoolData();
        fetchDCASignal();
        fetchTradingSignal();
    }

    drawContent();

    Serial.println("Main Screen initialized with scroll support");
}

void MainScreen::update() {
    unsigned long now = millis();

    // Update price data every 30s
    if (now - lastPriceUpdate >= PRICE_UPDATE) {
        lastPriceUpdate = now;

        // Fetch fresh data from APIs
        Serial.println("Fetching BTC data...");
        if (fetchBTCPrice()) {
            Serial.printf("Price: $%.0f\n", btcData.priceUSD);
        }
        if (fetchBlockData()) {
            Serial.printf("Block: %lu\n", btcData.blockHeight);
        }
        if (fetchMempoolData()) {
            Serial.printf("Mempool: %lu TX, Fee: %d sat/vB\n",
                         btcData.mempoolCount, btcData.feeFast);
        }

        drawContent();
    }

    // Update AI signals every 5 minutes
    if (now - lastAIUpdate >= AI_UPDATE) {
        lastAIUpdate = now;

        Serial.println("Fetching AI signals...");
        if (fetchDCASignal()) {
            Serial.printf("DCA: %s\n", btcData.dcaRecommendation);
        }
        if (fetchTradingSignal()) {
            Serial.printf("Trading Signal: %s\n", btcData.tradingSignal);
        }

        drawContent();
    }
}

void MainScreen::handleTouch(int16_t x, int16_t y) {
    // Rotation button in top-right corner (header area)
    if (y < 28 && x > 440) {
        rotateScreen();
        return;
    }

    // Start of new touch
    if (!isDragging) {
        isDragging = true;
        initialTouchX = x;
        initialTouchY = y;
        lastTouchX = x;
        lastTouchY = y;
        isHorizontalScroll = false;
    } else {
        // Calculate deltas
        int16_t deltaY = y - lastTouchY;

        // Lock to vertical scrolling only (horizontal scroll disabled)
        isHorizontalScroll = false;

        // Vertical scrolling only - update position instantly
        scrollOffsetY -= deltaY;

        // Clamp vertical scroll offset
        if (scrollOffsetY < 0) scrollOffsetY = 0;
        if (scrollOffsetY > maxScrollY) scrollOffsetY = maxScrollY;

        lastTouchY = y;

        // IMMEDIATE redraw with every touch movement for smooth scrolling
        unsigned long now = millis();
        if (now - lastDrawTime >= MIN_DRAW_INTERVAL) {
            drawContent();
            lastDrawnScrollY = scrollOffsetY;
            lastDrawTime = now;
        }
    }
}

void MainScreen::drawContent() {
    LGFX* lcd = manager->getLCD();

    // Use startWrite/endWrite for batch operations (much faster)
    lcd->startWrite();

    // Set clipping region to content area only (prevents cards from drawing over header)
    lcd->setClipRect(0, 29, 480, 291);

    // Clear content area (preserve header) - do AFTER setting clip region for faster clear
    lcd->fillRect(0, 29, 480, 291, 0x000000);

    // Consistent color palette: Bitcoin Orange
    uint32_t cardColor = 0xF7931A;

    // Calculate positions with vertical scroll offset only
    int baseY = 35 - scrollOffsetY;
    int baseX = 8;  // Fixed horizontal position (no horizontal scroll)

    // Row 1: Price and 24h Change
    char priceStr[32];
    snprintf(priceStr, sizeof(priceStr), "$%.0f", btcData.priceUSD);
    int x1 = baseX;
    int x2 = baseX + 236;
    if (baseY > -80 && baseY < 320 && x1 < 480 && (x1 + 228) > 0) {
        drawCard(x1, baseY, 228, 80, "BTC Price", priceStr, cardColor);
    }

    char changeStr[32];
    float change = btcData.priceUSD * 0.02; // Placeholder
    snprintf(changeStr, sizeof(changeStr), "+$%.0f", change);
    if (baseY > -80 && baseY < 320 && x2 < 480 && (x2 + 228) > 0) {
        drawCard(x2, baseY, 228, 80, "24h Change", changeStr, cardColor);
    }

    // Row 2: Block and Mempool
    baseY += 88;
    char blockStr[32];
    snprintf(blockStr, sizeof(blockStr), "%lu", btcData.blockHeight);
    if (baseY > -80 && baseY < 320 && x1 < 480 && (x1 + 228) > 0) {
        drawCard(x1, baseY, 228, 80, "Block Height", blockStr, cardColor);
    }

    char mempoolStr[32];
    snprintf(mempoolStr, sizeof(mempoolStr), "%lu TX", btcData.mempoolCount);
    if (baseY > -80 && baseY < 320 && x2 < 480 && (x2 + 228) > 0) {
        drawCard(x2, baseY, 228, 80, "Mempool", mempoolStr, cardColor);
    }

    // Row 3: Fees and Network
    baseY += 88;
    char feeStr[32];
    snprintf(feeStr, sizeof(feeStr), "%d sat/vB", btcData.feeFast);
    if (baseY > -80 && baseY < 320 && x1 < 480 && (x1 + 228) > 0) {
        drawCard(x1, baseY, 228, 80, "Fast Fee", feeStr, cardColor);
    }

    char networkStr[32];
    if (WiFi.status() == WL_CONNECTED) {
        int rssi = WiFi.RSSI();
        snprintf(networkStr, sizeof(networkStr), "%d dBm", rssi);
    } else {
        snprintf(networkStr, sizeof(networkStr), "No WiFi");
    }
    if (baseY > -80 && baseY < 320 && x2 < 480 && (x2 + 228) > 0) {
        drawCard(x2, baseY, 228, 80, "Signal", networkStr, cardColor);
    }

    // Row 4: DCA and Trading Signal (AI-powered)
    baseY += 88;
    char dcaStr[32];
    snprintf(dcaStr, sizeof(dcaStr), "%s", btcData.dcaRecommendation);
    if (baseY > -80 && baseY < 320 && x1 < 480 && (x1 + 228) > 0) {
        // Color based on recommendation: BUY=green, SELL=red, WAIT=yellow
        uint32_t dcaColor = cardColor;
        if (strcmp(btcData.dcaRecommendation, "BUY") == 0) {
            dcaColor = 0x00FF00; // Green
        } else if (strcmp(btcData.dcaRecommendation, "SELL") == 0) {
            dcaColor = 0xFF0000; // Red
        } else {
            dcaColor = 0xFFFF00; // Yellow (WAIT)
        }
        drawCard(x1, baseY, 228, 80, "DCA Signal", dcaStr, dcaColor);
    }

    char signalStr[32];
    snprintf(signalStr, sizeof(signalStr), "%s", btcData.tradingSignal);
    if (baseY > -80 && baseY < 320 && x2 < 480 && (x2 + 228) > 0) {
        // Color based on signal: BUY=green, SELL=red, HOLD=gray
        uint32_t signalColor = cardColor;
        if (strcmp(btcData.tradingSignal, "BUY") == 0) {
            signalColor = 0x00FF00; // Green
        } else if (strcmp(btcData.tradingSignal, "SELL") == 0) {
            signalColor = 0xFF0000; // Red
        } else {
            signalColor = 0x808080; // Gray (HOLD)
        }
        drawCard(x2, baseY, 228, 80, "Trading (15m-1h)", signalStr, signalColor);
    }

    // Clear clipping region
    lcd->clearClipRect();

    // Redraw header on top to ensure it's always visible (z-index fix)
    drawHeader();

    // End batch write operation
    lcd->endWrite();

    isDragging = false;
}

void MainScreen::rotateScreen() {
    LGFX* lcd = manager->getLCD();

    rotation = (rotation + 1) % 4;
    lcd->setRotation(rotation);

    Serial.printf("Screen rotated to: %d\n", rotation * 90);

    // Redraw everything
    lcd->fillScreen(0x000000);
    scrollOffsetX = 0;
    scrollOffsetY = 0;
    drawHeader();
    drawContent();
}

void MainScreen::drawHeader() {
    LGFX* lcd = manager->getLCD();

    // Header bar - Bitcoin orange background
    lcd->fillRect(0, 0, 480, 28, 0xF7931A);
    lcd->fillRect(0, 28, 480, 1, 0xFFFFFF);  // White separator line

    // Title - white text
    lcd->setTextColor(0xFFFFFF, 0xF7931A);
    lcd->setTextSize(2);
    lcd->setCursor(10, 8);
    lcd->print("Bitcoin Dashboard");

    // Time - white text
    char timeStr[16];
    unsigned long uptime = millis() / 1000;
    unsigned long hours = uptime / 3600;
    unsigned long mins = (uptime % 3600) / 60;
    snprintf(timeStr, sizeof(timeStr), "%02luh %02lum", hours, mins);

    lcd->setTextColor(0xFFFFFF, 0xF7931A);
    lcd->setTextSize(1);
    lcd->setCursor(380, 12);
    lcd->print(timeStr);
}

void MainScreen::drawCard(int x, int y, int w, int h, const char* title, const char* value, uint32_t color) {
    LGFX* lcd = manager->getLCD();

    // Simplified card - no rounded corners or shadows for faster rendering
    lcd->fillRect(x, y, w, h, 0x252525);              // Card background
    lcd->drawRect(x, y, w, h, 0x404040);              // Border

    // Title
    lcd->setTextColor(0xAAAAAA, 0x252525);
    lcd->setTextSize(1);
    lcd->setCursor(x + 8, y + 10);
    lcd->print(title);

    // Value
    lcd->setTextColor(0xFFFFFF, 0x252525);
    lcd->setTextSize(3);
    lcd->setCursor(x + 8, y + 35);
    lcd->print(value);
}

// API Fetch Functions
bool MainScreen::fetchBTCPrice() {
    if (WiFi.status() != WL_CONNECTED) {
        Serial.println("❌ Price fetch: WiFi not connected");
        return false;
    }

    HTTPClient http;
    http.begin("https://mempool.space/api/v1/prices");
    http.setTimeout(10000);

    Serial.println("📡 Fetching BTC price...");
    int httpCode = http.GET();
    Serial.printf("HTTP code: %d\n", httpCode);

    if (httpCode == 200) {
        String payload = http.getString();
        Serial.printf("Price payload: %s\n", payload.c_str());

        StaticJsonDocument<256> doc;
        DeserializationError error = deserializeJson(doc, payload);

        if (!error) {
            btcData.priceUSD = doc["USD"];
            btcData.priceEUR = doc["EUR"];
            Serial.printf("✓ Price: USD $%.2f, EUR €%.2f\n", btcData.priceUSD, btcData.priceEUR);
            http.end();
            return true;
        } else {
            Serial.printf("❌ JSON parse error: %s\n", error.c_str());
        }
    }

    http.end();
    return false;
}

bool MainScreen::fetchBlockData() {
    if (WiFi.status() != WL_CONNECTED) {
        Serial.println("❌ Block fetch: WiFi not connected");
        return false;
    }

    HTTPClient http;
    // Use tip/height endpoint instead - much smaller response
    http.begin("https://mempool.space/api/blocks/tip/height");
    http.setTimeout(10000);

    Serial.println("📡 Fetching block height...");
    int httpCode = http.GET();
    Serial.printf("HTTP code: %d\n", httpCode);

    if (httpCode == 200) {
        String payload = http.getString();
        Serial.printf("Block height payload: %s\n", payload.c_str());

        // Response is just a number
        btcData.blockHeight = payload.toInt();
        Serial.printf("✓ Block height: %lu\n", btcData.blockHeight);
        http.end();
        return true;
    }

    http.end();
    return false;
}

bool MainScreen::fetchMempoolData() {
    if (WiFi.status() != WL_CONNECTED) {
        Serial.println("❌ Mempool fetch: WiFi not connected");
        return false;
    }

    HTTPClient http;

    // Fetch fee rates (small payload)
    Serial.println("📡 Fetching fee rates...");
    http.begin("https://mempool.space/api/v1/fees/recommended");
    http.setTimeout(10000);

    int httpCode = http.GET();
    Serial.printf("Fees HTTP code: %d\n", httpCode);

    if (httpCode == 200) {
        String payload = http.getString();
        Serial.printf("Fees payload: %s\n", payload.c_str());

        StaticJsonDocument<256> doc;
        DeserializationError error = deserializeJson(doc, payload);

        if (!error) {
            btcData.feeFast = doc["fastestFee"];
            btcData.feeMedium = doc["halfHourFee"];
            btcData.feeSlow = doc["hourFee"];
            Serial.printf("✓ Fees: fast=%d, medium=%d, slow=%d sat/vB\n",
                         btcData.feeFast, btcData.feeMedium, btcData.feeSlow);
        } else {
            Serial.printf("❌ Fees JSON parse error: %s\n", error.c_str());
        }
    }
    http.end();

    // Fetch mempool count using streaming parser to avoid huge fee_histogram
    Serial.println("📡 Fetching mempool count...");
    http.begin("https://mempool.space/api/mempool");
    http.setTimeout(10000);

    httpCode = http.GET();
    Serial.printf("Mempool HTTP code: %d\n", httpCode);

    if (httpCode == 200) {
        String payload = http.getString();

        // Extract only the "count" field using string parsing to save memory
        int countStart = payload.indexOf("\"count\":");
        if (countStart != -1) {
            countStart += 8; // Skip "count":
            int countEnd = payload.indexOf(',', countStart);
            if (countEnd == -1) countEnd = payload.indexOf('}', countStart);

            String countStr = payload.substring(countStart, countEnd);
            btcData.mempoolCount = countStr.toInt();
            Serial.printf("✓ Mempool count: %lu TX\n", btcData.mempoolCount);
            http.end();
            return true;
        } else {
            Serial.println("❌ Could not find count in mempool response");
        }
    }

    http.end();
    return false;
}

bool MainScreen::fetchDCASignal() {
    if (WiFi.status() != WL_CONNECTED) {
        Serial.println("❌ DCA fetch: WiFi not connected");
        return false;
    }

    Serial.println("📡 Fetching DCA recommendation from Gemini AI...");

    // Create Gemini client
    GeminiClient gemini;

    // Fetch DCA recommendation
    String recommendation;
    bool success = gemini.fetchDCARecommendation(btcData, recommendation);

    if (success) {
        // Copy recommendation to btcData
        strncpy(btcData.dcaRecommendation, recommendation.c_str(), sizeof(btcData.dcaRecommendation) - 1);
        btcData.dcaRecommendation[sizeof(btcData.dcaRecommendation) - 1] = '\0';

        Serial.printf("✓ DCA Recommendation: %s\n", btcData.dcaRecommendation);
        return true;
    } else {
        Serial.println("❌ Failed to fetch DCA recommendation");
        return false;
    }
}

bool MainScreen::fetchTradingSignal() {
    if (WiFi.status() != WL_CONNECTED) {
        Serial.println("❌ Trading signal fetch: WiFi not connected");
        return false;
    }

    Serial.println("📡 Fetching trading signal from Gemini AI (15m-1h)...");

    // Create Gemini client
    GeminiClient gemini;

    // Fetch trading signal
    String signal;
    bool success = gemini.fetchTradingSignal(btcData, signal);

    if (success) {
        // Copy signal to btcData
        strncpy(btcData.tradingSignal, signal.c_str(), sizeof(btcData.tradingSignal) - 1);
        btcData.tradingSignal[sizeof(btcData.tradingSignal) - 1] = '\0';

        Serial.printf("✓ Trading Signal (15m-1h): %s\n", btcData.tradingSignal);
        return true;
    } else {
        Serial.println("❌ Failed to fetch trading signal");
        return false;
    }
}
