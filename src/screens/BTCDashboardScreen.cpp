#include "BTCDashboardScreen.h"

void BTCDashboardScreen::init(ScreenManager* mgr) {
    manager = mgr;
    lastPriceUpdate = 0;
    lastBlockUpdate = 0;
    lastMempoolUpdate = 0;

    setupUI();

    // Fetch initial data
    Serial.println("Fetching initial Bitcoin data...");
    fetchPrice();
    delay(100);
    fetchBlocks();
    delay(100);
    fetchMempool();
    delay(100);
    fetchFees();

    updateUI();
    Serial.println("Dashboard ready!");
}

bool BTCDashboardScreen::fetchPrice() {
    HTTPClient http;
    http.begin(API_PRICE);
    http.setTimeout(10000);

    int httpCode = http.GET();
    if (httpCode == HTTP_CODE_OK) {
        String payload = http.getString();
        StaticJsonDocument<256> doc;
        DeserializationError error = deserializeJson(doc, payload);

        if (!error) {
            btcData.priceUSD = doc["USD"].as<float>();
            btcData.priceEUR = doc["EUR"].as<float>();
            http.end();
            Serial.printf("Price updated: $%.0f\n", btcData.priceUSD);
            return true;
        }
    }
    http.end();
    Serial.println("Failed to fetch price");
    return false;
}

bool BTCDashboardScreen::fetchBlocks() {
    HTTPClient http;
    http.begin(API_BLOCKS);
    http.setTimeout(10000);

    int httpCode = http.GET();
    if (httpCode == HTTP_CODE_OK) {
        String payload = http.getString();
        DynamicJsonDocument doc(4096);
        DeserializationError error = deserializeJson(doc, payload);

        if (!error && doc.is<JsonArray>() && doc.size() > 0) {
            JsonObject latestBlock = doc[0];
            btcData.blockHeight = latestBlock["height"].as<int>();
            btcData.blockHash = latestBlock["id"].as<String>();
            btcData.blockTxCount = latestBlock["tx_count"].as<int>();
            btcData.blockTime = latestBlock["timestamp"].as<uint32_t>();
            http.end();
            Serial.printf("Block updated: %d (%d TXs)\n", btcData.blockHeight, btcData.blockTxCount);
            return true;
        }
    }
    http.end();
    Serial.println("Failed to fetch blocks");
    return false;
}

bool BTCDashboardScreen::fetchMempool() {
    HTTPClient http;
    http.begin(API_MEMPOOL);
    http.setTimeout(10000);

    int httpCode = http.GET();
    if (httpCode == HTTP_CODE_OK) {
        String payload = http.getString();
        StaticJsonDocument<512> doc;
        DeserializationError error = deserializeJson(doc, payload);

        if (!error) {
            btcData.mempoolCount = doc["count"].as<int>();
            btcData.mempoolSize = doc["vsize"].as<float>() / 1000000.0;
            http.end();
            Serial.printf("Mempool updated: %d TXs (%.1f MB)\n", btcData.mempoolCount, btcData.mempoolSize);
            return true;
        }
    }
    http.end();
    Serial.println("Failed to fetch mempool");
    return false;
}

bool BTCDashboardScreen::fetchFees() {
    HTTPClient http;
    http.begin(API_FEES);
    http.setTimeout(10000);

    int httpCode = http.GET();
    if (httpCode == HTTP_CODE_OK) {
        String payload = http.getString();
        StaticJsonDocument<256> doc;
        DeserializationError error = deserializeJson(doc, payload);

        if (!error) {
            btcData.feeFast = doc["fastestFee"].as<int>();
            btcData.feeMedium = doc["halfHourFee"].as<int>();
            btcData.feeSlow = doc["hourFee"].as<int>();
            http.end();
            Serial.printf("Fees updated: Fast=%d Med=%d Slow=%d\n",
                         btcData.feeFast, btcData.feeMedium, btcData.feeSlow);
            return true;
        }
    }
    http.end();
    Serial.println("Failed to fetch fees");
    return false;
}

void BTCDashboardScreen::drawCard(int x, int y, int w, int h, uint32_t borderColor,
                                   const char* title, const char* content, uint32_t contentColor) {
    LGFX* lcd = manager->getLCD();

    // Draw card background with triple border
    lcd->fillRoundRect(x, y, w, h, 8, COLOR_CARD_BG);
    lcd->drawRoundRect(x, y, w, h, 8, borderColor);
    lcd->drawRoundRect(x + 1, y + 1, w - 2, h - 2, 7, borderColor);
    lcd->drawRoundRect(x + 2, y + 2, w - 4, h - 4, 6, borderColor);

    // Draw title
    lcd->setTextColor(COLOR_TEXT_WHITE, COLOR_CARD_BG);
    lcd->setTextSize(2);
    lcd->setCursor(x + 8, y + 5);
    lcd->print(title);

    // Draw content - HUGE
    lcd->setTextColor(contentColor, COLOR_CARD_BG);
    lcd->setTextSize(4);
    lcd->setCursor(x + 8, y + 35);
    lcd->print(content);
}

void BTCDashboardScreen::drawWiFiIcon() {
    LGFX* lcd = manager->getLCD();

    // Draw WiFi icon button with border
    lcd->drawRoundRect(WIFI_ICON_X, WIFI_ICON_Y, WIFI_ICON_SIZE, WIFI_ICON_SIZE, 5, COLOR_TEXT_LIGHT);
    lcd->drawRoundRect(WIFI_ICON_X + 1, WIFI_ICON_Y + 1, WIFI_ICON_SIZE - 2, WIFI_ICON_SIZE - 2, 4, COLOR_TEXT_LIGHT);

    // Draw WiFi symbol (simplified arcs)
    int cx = WIFI_ICON_X + WIFI_ICON_SIZE / 2;
    int cy = WIFI_ICON_Y + WIFI_ICON_SIZE - 8;

    // Signal strength bars
    lcd->fillCircle(cx, cy, 2, COLOR_TEXT_WHITE);  // Center dot

    // Arc 1 (innermost)
    lcd->drawArc(cx, cy, 5, 3, 200, 340, COLOR_TEXT_WHITE);

    // Arc 2 (middle)
    lcd->drawArc(cx, cy, 9, 7, 200, 340, COLOR_TEXT_WHITE);

    // Arc 3 (outermost)
    lcd->drawArc(cx, cy, 13, 11, 200, 340, COLOR_TEXT_WHITE);
}

void BTCDashboardScreen::drawSettingsIcon() {
    LGFX* lcd = manager->getLCD();

    // Draw Settings icon button with border
    lcd->drawRoundRect(SETTINGS_ICON_X, SETTINGS_ICON_Y, SETTINGS_ICON_SIZE, SETTINGS_ICON_SIZE, 5, COLOR_TEXT_LIGHT);
    lcd->drawRoundRect(SETTINGS_ICON_X + 1, SETTINGS_ICON_Y + 1, SETTINGS_ICON_SIZE - 2, SETTINGS_ICON_SIZE - 2, 4, COLOR_TEXT_LIGHT);

    // Draw gear/cog symbol (simplified)
    int cx = SETTINGS_ICON_X + SETTINGS_ICON_SIZE / 2;
    int cy = SETTINGS_ICON_Y + SETTINGS_ICON_SIZE / 2;

    // Center circle
    lcd->fillCircle(cx, cy, 5, COLOR_TEXT_WHITE);
    lcd->fillCircle(cx, cy, 3, COLOR_BG);  // Inner hole

    // Gear teeth (simplified as rectangles)
    lcd->fillRect(cx - 2, SETTINGS_ICON_Y + 3, 4, 4, COLOR_TEXT_WHITE);   // Top
    lcd->fillRect(cx - 2, SETTINGS_ICON_Y + 23, 4, 4, COLOR_TEXT_WHITE);  // Bottom
    lcd->fillRect(SETTINGS_ICON_X + 3, cy - 2, 4, 4, COLOR_TEXT_WHITE);   // Left
    lcd->fillRect(SETTINGS_ICON_X + 23, cy - 2, 4, 4, COLOR_TEXT_WHITE);  // Right
}

void BTCDashboardScreen::setupUI() {
    LGFX* lcd = manager->getLCD();
    lcd->fillScreen(COLOR_BG);

    // Title bar
    lcd->setTextColor(COLOR_BTC_ORANGE, COLOR_BG);
    lcd->setTextSize(3);
    lcd->setCursor(10, 5);
    lcd->print("BTC Dashboard");

    // Draw WiFi settings icon
    drawWiFiIcon();

    // Draw Settings icon
    drawSettingsIcon();

    // Draw initial cards
    drawCard(CARD_MARGIN, 45, CARD_WIDTH, CARD_HEIGHT,
             COLOR_BORDER_ORANGE, "PRICE", "Loading", COLOR_BTC_GOLD);

    drawCard(480 - CARD_WIDTH - CARD_MARGIN, 45, CARD_WIDTH, CARD_HEIGHT,
             COLOR_BORDER_CYAN, "BLOCK", "...", COLOR_BLOCK_CYAN);

    drawCard(CARD_MARGIN, 320 - CARD_HEIGHT - 25, CARD_WIDTH, CARD_HEIGHT,
             COLOR_BORDER_YELLOW, "MEMPOOL", "...", COLOR_MEMPOOL_YELLOW);

    drawCard(480 - CARD_WIDTH - CARD_MARGIN, 320 - CARD_HEIGHT - 25, CARD_WIDTH, CARD_HEIGHT,
             COLOR_BORDER_GREEN, "FAST FEE", "...", COLOR_FEE_ORANGE);

    // Status bar with swipe hint
    lcd->setTextColor(COLOR_TEXT_DIM, COLOR_BG);
    lcd->setTextSize(1);
    lcd->setCursor(10, 320 - 15);
    lcd->print("Loading data...");

    // Swipe hint on right side
    lcd->setCursor(340, 320 - 15);
    lcd->print("Swipe for News >");
}

void BTCDashboardScreen::updateUI() {
    LGFX* lcd = manager->getLCD();
    char buf[64];

    // Update Price Card
    if (btcData.priceUSD >= 100000) {
        snprintf(buf, sizeof(buf), "$%.0fk", btcData.priceUSD / 1000);
    } else {
        snprintf(buf, sizeof(buf), "$%.0f", btcData.priceUSD);
    }
    drawCard(CARD_MARGIN, 45, CARD_WIDTH, CARD_HEIGHT,
             COLOR_BORDER_ORANGE, "PRICE", buf, COLOR_PRICE_GREEN);

    // Update Block Card
    snprintf(buf, sizeof(buf), "%d", btcData.blockHeight);
    drawCard(480 - CARD_WIDTH - CARD_MARGIN, 45, CARD_WIDTH, CARD_HEIGHT,
             COLOR_BORDER_CYAN, "BLOCK", buf, COLOR_BLOCK_CYAN);

    // Add TX count
    lcd->setTextColor(COLOR_TEXT_LIGHT, COLOR_CARD_BG);
    lcd->setTextSize(2);
    lcd->setCursor(480 - CARD_WIDTH - CARD_MARGIN + 10, 45 + 82);
    lcd->printf("%d TXs", btcData.blockTxCount);

    // Update Mempool Card
    if (btcData.mempoolCount >= 1000) {
        snprintf(buf, sizeof(buf), "%.1fk", btcData.mempoolCount / 1000.0);
    } else {
        snprintf(buf, sizeof(buf), "%d", btcData.mempoolCount);
    }
    drawCard(CARD_MARGIN, 320 - CARD_HEIGHT - 25, CARD_WIDTH, CARD_HEIGHT,
             COLOR_BORDER_YELLOW, "MEMPOOL", buf, COLOR_MEMPOOL_YELLOW);

    // Add size
    lcd->setTextColor(COLOR_TEXT_LIGHT, COLOR_CARD_BG);
    lcd->setTextSize(2);
    lcd->setCursor(CARD_MARGIN + 10, 320 - CARD_HEIGHT - 25 + 82);
    lcd->printf("%.1fMB", btcData.mempoolSize);

    // Update Fees Card
    snprintf(buf, sizeof(buf), "%d", btcData.feeFast);
    drawCard(480 - CARD_WIDTH - CARD_MARGIN, 320 - CARD_HEIGHT - 25, CARD_WIDTH, CARD_HEIGHT,
             COLOR_BORDER_GREEN, "FAST FEE", buf, COLOR_FEE_ORANGE);

    // Add other fees
    lcd->setTextColor(COLOR_TEXT_LIGHT, COLOR_CARD_BG);
    lcd->setTextSize(2);
    lcd->setCursor(480 - CARD_WIDTH - CARD_MARGIN + 10, 320 - CARD_HEIGHT - 25 + 82);
    lcd->printf("M:%d S:%d", btcData.feeMedium, btcData.feeSlow);

    // Update status bar
    lcd->fillRect(0, 320 - 20, 480, 20, COLOR_BG);
    lcd->setTextColor(COLOR_BTC_ORANGE, COLOR_BG);
    lcd->setTextSize(2);
    lcd->setCursor(10, 320 - 18);
    lcd->print("LIVE");

    lcd->setTextColor(COLOR_TEXT_DIM, COLOR_BG);
    lcd->setCursor(70, 320 - 18);
    lcd->printf("mempool.space");

    // Swipe hint on right side
    lcd->setTextSize(1);
    lcd->setCursor(340, 320 - 15);
    lcd->print("Swipe for News >");

    // Show IP address if needed (debug)
    // lcd->printf(" | %d.%d.%d.%d",
    //            WiFi.localIP()[0], WiFi.localIP()[1],
    //            WiFi.localIP()[2], WiFi.localIP()[3]);
}

void BTCDashboardScreen::update() {
    // Check WiFi connection
    if (WiFi.status() != WL_CONNECTED) {
        return;
    }

    unsigned long now = millis();

    // Update price every 30 seconds
    if (now - lastPriceUpdate >= PRICE_INTERVAL) {
        if (fetchPrice()) {
            lastPriceUpdate = now;
            updateUI();
        }
    }

    // Update blocks every 60 seconds
    if (now - lastBlockUpdate >= BLOCK_INTERVAL) {
        if (fetchBlocks()) {
            lastBlockUpdate = now;
            updateUI();
        }
    }

    // Update mempool & fees every 30 seconds
    if (now - lastMempoolUpdate >= MEMPOOL_INTERVAL) {
        fetchMempool();
        fetchFees();
        lastMempoolUpdate = now;
        updateUI();
    }
}

void BTCDashboardScreen::handleTouch(int16_t x, int16_t y) {
    Serial.printf("Dashboard touch at: %d, %d\n", x, y);

    // Check if Settings icon was tapped
    if (x >= SETTINGS_ICON_X && x <= (SETTINGS_ICON_X + SETTINGS_ICON_SIZE) &&
        y >= SETTINGS_ICON_Y && y <= (SETTINGS_ICON_Y + SETTINGS_ICON_SIZE)) {

        Serial.println("Settings icon tapped - switching to Settings");

        // Visual feedback - highlight icon
        LGFX* lcd = manager->getLCD();
        lcd->fillRoundRect(SETTINGS_ICON_X, SETTINGS_ICON_Y, SETTINGS_ICON_SIZE, SETTINGS_ICON_SIZE, 5, COLOR_BTC_ORANGE);
        delay(200);

        // Switch to Settings screen
        manager->switchScreen(SCREEN_SETTINGS);
        return;
    }

    // Check if WiFi icon was tapped
    if (x >= WIFI_ICON_X && x <= (WIFI_ICON_X + WIFI_ICON_SIZE) &&
        y >= WIFI_ICON_Y && y <= (WIFI_ICON_Y + WIFI_ICON_SIZE)) {

        Serial.println("WiFi icon tapped - switching to WiFi config");

        // Visual feedback - highlight icon
        LGFX* lcd = manager->getLCD();
        lcd->fillRoundRect(WIFI_ICON_X, WIFI_ICON_Y, WIFI_ICON_SIZE, WIFI_ICON_SIZE, 5, COLOR_BTC_ORANGE);
        delay(200);

        // Switch to WiFi scan screen
        manager->switchScreen(SCREEN_WIFI_SCAN);
    }
}
