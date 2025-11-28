#include "WiFiScanScreen.h"

void WiFiScanScreen::init(ScreenManager* mgr) {
    manager = mgr;
    selectedIndex = -1;
    scrollOffset = 0;
    networkCount = 0;

    LGFX* lcd = manager->getLCD();
    lcd->fillScreen(COLOR_BG);

    drawHeader();

    // Show scanning message
    lcd->setTextColor(COLOR_TEXT, COLOR_BG);
    lcd->setTextSize(3);
    lcd->setCursor(120, 140);
    lcd->print("Scanning...");

    scanNetworks();
    drawNetworkList();
}

void WiFiScanScreen::scanNetworks() {
    WiFi.mode(WIFI_STA);
    WiFi.disconnect();
    delay(100);

    Serial.println("Scanning for WiFi networks...");
    int n = WiFi.scanNetworks();

    networkCount = min(n, MAX_NETWORKS);
    Serial.printf("Found %d networks\n", networkCount);

    for (int i = 0; i < networkCount; i++) {
        networks[i].ssid = WiFi.SSID(i);
        networks[i].rssi = WiFi.RSSI(i);
        networks[i].encrypted = (WiFi.encryptionType(i) != WIFI_AUTH_OPEN);

        Serial.printf("  %d: %s (%d dBm) %s\n",
                     i, networks[i].ssid.c_str(), networks[i].rssi,
                     networks[i].encrypted ? "🔒" : "Open");
    }
}

void WiFiScanScreen::drawHeader() {
    LGFX* lcd = manager->getLCD();

    // Header background
    lcd->fillRect(0, 0, 480, 50, COLOR_BG);

    // Title
    lcd->setTextColor(COLOR_HEADER, COLOR_BG);
    lcd->setTextSize(3);
    lcd->setCursor(10, 12);
    lcd->print("Select WiFi Network");

    // Refresh button
    lcd->drawRoundRect(400, 10, 70, 30, 5, COLOR_HEADER);
    lcd->setTextSize(2);
    lcd->setCursor(410, 17);
    lcd->print("SCAN");
}

void WiFiScanScreen::drawNetworkList() {
    LGFX* lcd = manager->getLCD();

    // Clear list area
    lcd->fillRect(0, SCROLL_START_Y, 480, 260, COLOR_BG);

    // Draw each network
    for (int i = 0; i < networkCount; i++) {
        int y = SCROLL_START_Y + (i * ITEM_HEIGHT) - scrollOffset;

        // Only draw if visible
        if (y >= SCROLL_START_Y && y < 310) {
            drawNetwork(i, y, i == selectedIndex);
        }
    }
}

void WiFiScanScreen::drawNetwork(int index, int y, bool selected) {
    LGFX* lcd = manager->getLCD();

    // Background
    uint32_t bgColor = selected ? COLOR_ITEM_SELECTED : COLOR_ITEM_BG;
    lcd->fillRoundRect(10, y, 460, ITEM_HEIGHT - 5, 8, bgColor);

    // SSID
    lcd->setTextColor(selected ? COLOR_BG : COLOR_TEXT, bgColor);
    lcd->setTextSize(2);
    lcd->setCursor(20, y + 8);

    String displaySSID = networks[index].ssid;
    if (displaySSID.length() > 25) {
        displaySSID = displaySSID.substring(0, 25) + "...";
    }
    lcd->print(displaySSID);

    // Signal strength bars
    int bars = getSignalBars(networks[index].rssi);
    uint32_t signalColor = selected ? COLOR_BG : getSignalColor(networks[index].rssi);

    int barX = 20;
    int barY = y + 30;
    for (int i = 0; i < 4; i++) {
        int barHeight = 4 + (i * 3);
        if (i < bars) {
            lcd->fillRect(barX + (i * 8), barY + (12 - barHeight), 6, barHeight, signalColor);
        } else {
            lcd->drawRect(barX + (i * 8), barY + (12 - barHeight), 6, barHeight,
                         selected ? COLOR_BG : COLOR_TEXT_DIM);
        }
    }

    // Lock icon if encrypted
    if (networks[index].encrypted) {
        lcd->setTextSize(2);
        lcd->setCursor(430, y + 15);
        lcd->print(selected ? "!" : "#");
    }

    // RSSI value
    lcd->setTextSize(1);
    lcd->setCursor(380, y + 32);
    lcd->printf("%ddBm", networks[index].rssi);
}

int WiFiScanScreen::getSignalBars(int rssi) {
    if (rssi >= -50) return 4;
    if (rssi >= -60) return 3;
    if (rssi >= -70) return 2;
    return 1;
}

uint32_t WiFiScanScreen::getSignalColor(int rssi) {
    if (rssi >= -60) return COLOR_SIGNAL_GOOD;
    if (rssi >= -70) return COLOR_SIGNAL_MED;
    return COLOR_SIGNAL_WEAK;
}

void WiFiScanScreen::update() {
    // Nothing to update continuously
}

void WiFiScanScreen::handleTouch(int16_t x, int16_t y) {
    Serial.printf("Touch at: %d, %d\n", x, y);

    // Check if refresh button tapped
    if (x >= 400 && x <= 470 && y >= 10 && y <= 40) {
        Serial.println("Refresh button tapped");
        init(manager);  // Re-scan
        return;
    }

    // Check if network item tapped
    if (y >= SCROLL_START_Y && y <= 310) {
        int tappedIndex = (y - SCROLL_START_Y + scrollOffset) / ITEM_HEIGHT;

        if (tappedIndex >= 0 && tappedIndex < networkCount) {
            Serial.printf("Selected network: %s\n", networks[tappedIndex].ssid.c_str());
            selectedIndex = tappedIndex;
            drawNetworkList();

            // Wait a moment to show selection
            delay(300);

            // Show connecting message
            LGFX* lcd = manager->getLCD();
            lcd->fillRect(0, 260, 480, 60, COLOR_BG);
            lcd->setTextColor(COLOR_ITEM_SELECTED, COLOR_BG);
            lcd->setTextSize(2);
            lcd->setCursor(140, 280);
            lcd->print("Connecting...");

            // If encrypted, use stored password (TODO: password input screen)
            String password = "0850761423";  // Hardcoded for now

            // Connect to WiFi
            WiFi.begin(networks[selectedIndex].ssid.c_str(), password.c_str());

            int attempts = 0;
            while (WiFi.status() != WL_CONNECTED && attempts < 40) {
                delay(500);
                Serial.print(".");
                attempts++;
            }

            if (WiFi.status() == WL_CONNECTED) {
                Serial.println("\nWiFi Connected!");
                Serial.printf("IP: %s\n", WiFi.localIP().toString().c_str());

                // Show success
                lcd->fillRect(0, 260, 480, 60, COLOR_BG);
                lcd->setTextColor(COLOR_SIGNAL_GOOD, COLOR_BG);
                lcd->setTextSize(2);
                lcd->setCursor(150, 280);
                lcd->print("Connected!");
                delay(1000);

                // Switch to dashboard
                manager->switchScreen(SCREEN_DASHBOARD);
            } else {
                // Show failure
                lcd->fillRect(0, 260, 480, 60, COLOR_BG);
                lcd->setTextColor(COLOR_SIGNAL_WEAK, COLOR_BG);
                lcd->setTextSize(2);
                lcd->setCursor(130, 280);
                lcd->print("Failed! Retry?");
                delay(2000);
                selectedIndex = -1;
                drawNetworkList();
            }
        }
    }
}

String WiFiScanScreen::getSelectedSSID() {
    if (selectedIndex >= 0 && selectedIndex < networkCount) {
        return networks[selectedIndex].ssid;
    }
    return "";
}

bool WiFiScanScreen::isSelectedEncrypted() {
    if (selectedIndex >= 0 && selectedIndex < networkCount) {
        return networks[selectedIndex].encrypted;
    }
    return false;
}
