#ifndef MOCK_LGFX_H
#define MOCK_LGFX_H

#include <stdint.h>

/**
 * Mock LGFX display for native unit testing
 * Simulates display operations without hardware
 */
class MockLGFX {
public:
    MockLGFX() : initCalled(false), rotation(0), screenColor(0),
                 brightness(128), lastX(0), lastY(0), lastW(0), lastH(0) {}

    // Display initialization
    void init() { initCalled = true; }
    bool isInitialized() { return initCalled; }

    // Display settings
    void setRotation(uint8_t r) { rotation = r; }
    uint8_t getRotation() { return rotation; }

    void setBrightness(uint8_t b) { brightness = b; }
    uint8_t getBrightness() { return brightness; }

    // Drawing operations
    void fillScreen(uint32_t color) { screenColor = color; }
    uint32_t getScreenColor() { return screenColor; }

    void fillRect(int16_t x, int16_t y, int16_t w, int16_t h, uint32_t color) {
        lastX = x;
        lastY = y;
        lastW = w;
        lastH = h;
        lastColor = color;
    }

    void drawRect(int16_t x, int16_t y, int16_t w, int16_t h, uint32_t color) {
        lastX = x;
        lastY = y;
        lastW = w;
        lastH = h;
        lastColor = color;
    }

    void drawRoundRect(int16_t x, int16_t y, int16_t w, int16_t h, int16_t r, uint32_t color) {
        lastX = x;
        lastY = y;
        lastW = w;
        lastH = h;
        lastColor = color;
        lastRadius = r;
    }

    void fillRoundRect(int16_t x, int16_t y, int16_t w, int16_t h, int16_t r, uint32_t color) {
        lastX = x;
        lastY = y;
        lastW = w;
        lastH = h;
        lastColor = color;
        lastRadius = r;
    }

    void fillCircle(int16_t x, int16_t y, int16_t r, uint32_t color) {
        lastX = x;
        lastY = y;
        lastRadius = r;
        lastColor = color;
    }

    void drawArc(int16_t x, int16_t y, int16_t r1, int16_t r2, int16_t a1, int16_t a2, uint32_t color) {
        lastX = x;
        lastY = y;
        lastRadius = r1;
        lastColor = color;
    }

    // Text operations
    void setTextColor(uint32_t color) { textColor = color; }
    void setTextSize(float size) { textSize = size; }
    void setCursor(int16_t x, int16_t y) { cursorX = x; cursorY = y; }
    void print(const char* str) { lastText = str; }
    void println(const char* str) { lastText = str; }

    // Verification helpers
    int16_t getLastX() { return lastX; }
    int16_t getLastY() { return lastY; }
    int16_t getLastW() { return lastW; }
    int16_t getLastH() { return lastH; }
    uint32_t getLastColor() { return lastColor; }
    int16_t getLastRadius() { return lastRadius; }
    uint32_t getTextColor() { return textColor; }
    float getTextSize() { return textSize; }
    const char* getLastText() { return lastText; }

private:
    bool initCalled;
    uint8_t rotation;
    uint32_t screenColor;
    uint8_t brightness;

    int16_t lastX, lastY, lastW, lastH;
    uint32_t lastColor;
    int16_t lastRadius;

    uint32_t textColor;
    float textSize;
    int16_t cursorX, cursorY;
    const char* lastText;
};

#endif // MOCK_LGFX_H
