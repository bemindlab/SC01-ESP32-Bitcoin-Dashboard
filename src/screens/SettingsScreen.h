#ifndef SETTINGS_SCREEN_H
#define SETTINGS_SCREEN_H

#include "ScreenManager.h"
#include "../Config.h"
#include "../ui/TouchFeedbackManager.h"

class SettingsScreen : public BaseScreen {
private:
    ScreenManager* manager;
    TouchFeedbackManager feedback;

    // UI state
    int selectedOption;
    bool editingKey;
    String tempApiKey;

    // Touch feedback IDs
    int backButtonFeedbackId;
    int optionFeedbackIds[5];  // One for each option

    // Colors
    static const uint32_t COLOR_BG = 0x000000;
    static const uint32_t COLOR_HEADER_BG = 0x1A1A1A;
    static const uint32_t COLOR_TEXT_WHITE = 0xFFFFFF;
    static const uint32_t COLOR_TEXT_LIGHT = 0xCCCCCC;
    static const uint32_t COLOR_TEXT_DIM = 0x999999;
    static const uint32_t COLOR_BTC_ORANGE = 0xFF9500;
    static const uint32_t COLOR_SELECTED = 0x0066FF;
    static const uint32_t COLOR_SUCCESS = 0x00FF00;
    static const uint32_t COLOR_ERROR = 0xFF3333;

    // UI Layout
    static const int HEADER_HEIGHT = 40;
    static const int BACK_BTN_X = 10;
    static const int BACK_BTN_Y = 5;
    static const int OPTION_HEIGHT = 60;
    static const int OPTION_PADDING = 10;

    // Settings options
    enum SettingOption {
        OPTION_GEMINI_KEY = 0,
        OPTION_WIFI_SETTINGS,
        OPTION_INTERVALS,
        OPTION_RESET_CONFIG,
        OPTION_BACK,
        OPTION_COUNT
    };

    // UI Methods
    void drawHeader();
    void drawBackButton();
    void drawOptions();
    void drawOption(int index, const char* label, const char* value, bool isSelected);
    void drawKeyEditor();

    // Action handlers
    void handleOptionSelect(int option);
    void editGeminiKey();
    void resetConfiguration();

public:
    void init(ScreenManager* mgr) override;
    void update() override;
    void handleTouch(int16_t x, int16_t y) override;
};

#endif
