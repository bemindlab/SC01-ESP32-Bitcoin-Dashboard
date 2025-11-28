#ifndef TOUCH_FEEDBACK_MANAGER_H
#define TOUCH_FEEDBACK_MANAGER_H

#include <Arduino.h>
#include "../DisplayConfig.h"

// Maximum number of concurrent feedback elements
#define MAX_FEEDBACK_ELEMENTS 20

// Touch feedback colors
namespace TouchColors {
    // Button states
    const uint32_t BUTTON_NORMAL    = 0x0C0C0C;  // Dark gray
    const uint32_t BUTTON_PRESSED   = 0xFF9500;  // Bitcoin orange
    const uint32_t BUTTON_HIGHLIGHT = 0xFFBF00;  // Gold

    // List selection
    const uint32_t LIST_NORMAL      = 0x000000;  // Black
    const uint32_t LIST_SELECTED    = 0x1A1A2E;  // Dark blue-gray
    const uint32_t LIST_BORDER      = 0xFF9500;  // Bitcoin orange

    // Icon states
    const uint32_t ICON_NORMAL      = 0xCCCCCC;  // Light gray
    const uint32_t ICON_PRESSED     = 0xFF9500;  // Bitcoin orange
    const uint32_t ICON_FLASH       = 0xFFFFFF;  // White

    // Scroll elements
    const uint32_t SCROLL_BAR       = 0x666666;  // Medium gray
    const uint32_t SCROLL_THUMB     = 0xFF9500;  // Bitcoin orange
    const uint32_t SCROLL_ACTIVE    = 0xFFBF00;  // Gold
}

// Feedback type
enum FeedbackType {
    FEEDBACK_NONE,
    FEEDBACK_BUTTON,
    FEEDBACK_ICON_FLASH,
    FEEDBACK_LIST_SELECT,
    FEEDBACK_HIGHLIGHT
};

// Feedback element structure
struct FeedbackElement {
    int16_t x, y, w, h;
    uint32_t normalColor;
    uint32_t pressedColor;
    uint32_t borderColor;
    unsigned long pressTime;
    unsigned long duration;
    FeedbackType type;
    bool isActive;
    bool isPersistent;
    int radius;  // For rounded corners
};

/**
 * TouchFeedbackManager
 * Manages visual feedback for touch interactions
 * Non-blocking design using timers
 */
class TouchFeedbackManager {
private:
    FeedbackElement elements[MAX_FEEDBACK_ELEMENTS];
    int elementCount;
    LGFX* lcd;

    // Helper to draw element
    void drawElement(const FeedbackElement& elem, bool pressed);

public:
    TouchFeedbackManager();

    // Initialize with LCD reference
    void init(LGFX* display);

    // Register touchable area
    int registerButton(int16_t x, int16_t y, int16_t w, int16_t h,
                      uint32_t normalColor, uint32_t pressedColor,
                      int radius = 5, unsigned long duration = 200);

    int registerIcon(int16_t x, int16_t y, int16_t size,
                    uint32_t normalColor, uint32_t flashColor,
                    unsigned long duration = 200);

    int registerListItem(int16_t x, int16_t y, int16_t w, int16_t h,
                        uint32_t normalColor, uint32_t selectedColor,
                        uint32_t borderColor);

    // Touch events
    void onTouchDown(int id);
    void onTouchUp(int id);
    void flash(int id);  // One-shot flash effect

    // Non-blocking update (call in loop)
    void update();

    // Clear specific or all feedback
    void clear(int id);
    void clearAll();

    // Check if feedback is active
    bool isActive(int id);
};

#endif // TOUCH_FEEDBACK_MANAGER_H
