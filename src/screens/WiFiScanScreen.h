#ifndef WIFI_SCAN_SCREEN_H
#define WIFI_SCAN_SCREEN_H

#include "ScreenManager.h"
#include "../ui/TouchFeedbackManager.h"
#include <WiFi.h>

#define MAX_NETWORKS 10
#define ITEM_HEIGHT 50
#define SCROLL_START_Y 60

struct WiFiNetwork {
    String ssid;
    int rssi;
    bool encrypted;
};

class WiFiScanScreen : public BaseScreen {
private:
    ScreenManager* manager;
    TouchFeedbackManager feedback;
    WiFiNetwork networks[MAX_NETWORKS];
    int networkCount;
    int selectedIndex;
    int scrollOffset;

    // Touch feedback IDs
    int refreshButtonFeedbackId;
    int networkFeedbackIds[MAX_NETWORKS];

    // Colors
    const uint32_t COLOR_BG = 0x000000;
    const uint32_t COLOR_HEADER = 0xFF9500;
    const uint32_t COLOR_ITEM_BG = 0x1A1F3A;
    const uint32_t COLOR_ITEM_SELECTED = 0xFF9500;
    const uint32_t COLOR_TEXT = 0xFFFFFF;
    const uint32_t COLOR_TEXT_DIM = 0x999999;
    const uint32_t COLOR_SIGNAL_GOOD = 0x00FF00;
    const uint32_t COLOR_SIGNAL_MED = 0xFFFF00;
    const uint32_t COLOR_SIGNAL_WEAK = 0xFF6600;

    void scanNetworks();
    void drawHeader();
    void drawNetworkList();
    void drawNetwork(int index, int y, bool selected);
    int getSignalBars(int rssi);
    uint32_t getSignalColor(int rssi);

public:
    void init(ScreenManager* mgr) override;
    void update() override;
    void handleTouch(int16_t x, int16_t y) override;

    String getSelectedSSID();
    bool isSelectedEncrypted();
};

#endif
