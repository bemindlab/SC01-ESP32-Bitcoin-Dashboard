#include "TouchFeedbackManager.h"

TouchFeedbackManager::TouchFeedbackManager() {
    elementCount = 0;
    lcd = nullptr;

    // Initialize all elements
    for (int i = 0; i < MAX_FEEDBACK_ELEMENTS; i++) {
        elements[i].isActive = false;
        elements[i].type = FEEDBACK_NONE;
    }
}

void TouchFeedbackManager::init(LGFX* display) {
    lcd = display;
}

int TouchFeedbackManager::registerButton(int16_t x, int16_t y, int16_t w, int16_t h,
                                        uint32_t normalColor, uint32_t pressedColor,
                                        int radius, unsigned long duration) {
    if (elementCount >= MAX_FEEDBACK_ELEMENTS || lcd == nullptr) {
        return -1;
    }

    int id = elementCount++;
    elements[id].x = x;
    elements[id].y = y;
    elements[id].w = w;
    elements[id].h = h;
    elements[id].normalColor = normalColor;
    elements[id].pressedColor = pressedColor;
    elements[id].borderColor = pressedColor;
    elements[id].duration = duration;
    elements[id].type = FEEDBACK_BUTTON;
    elements[id].isActive = false;
    elements[id].isPersistent = false;
    elements[id].radius = radius;

    return id;
}

int TouchFeedbackManager::registerIcon(int16_t x, int16_t y, int16_t size,
                                      uint32_t normalColor, uint32_t flashColor,
                                      unsigned long duration) {
    if (elementCount >= MAX_FEEDBACK_ELEMENTS || lcd == nullptr) {
        return -1;
    }

    int id = elementCount++;
    elements[id].x = x;
    elements[id].y = y;
    elements[id].w = size;
    elements[id].h = size;
    elements[id].normalColor = normalColor;
    elements[id].pressedColor = flashColor;
    elements[id].borderColor = flashColor;
    elements[id].duration = duration;
    elements[id].type = FEEDBACK_ICON_FLASH;
    elements[id].isActive = false;
    elements[id].isPersistent = false;
    elements[id].radius = 5;

    return id;
}

int TouchFeedbackManager::registerListItem(int16_t x, int16_t y, int16_t w, int16_t h,
                                          uint32_t normalColor, uint32_t selectedColor,
                                          uint32_t borderColor) {
    if (elementCount >= MAX_FEEDBACK_ELEMENTS || lcd == nullptr) {
        return -1;
    }

    int id = elementCount++;
    elements[id].x = x;
    elements[id].y = y;
    elements[id].w = w;
    elements[id].h = h;
    elements[id].normalColor = normalColor;
    elements[id].pressedColor = selectedColor;
    elements[id].borderColor = borderColor;
    elements[id].duration = 0;  // Persistent until cleared
    elements[id].type = FEEDBACK_LIST_SELECT;
    elements[id].isActive = false;
    elements[id].isPersistent = true;
    elements[id].radius = 0;

    return id;
}

void TouchFeedbackManager::onTouchDown(int id) {
    if (id < 0 || id >= elementCount || lcd == nullptr) {
        return;
    }

    elements[id].isActive = true;
    elements[id].pressTime = millis();

    // Draw pressed state immediately
    drawElement(elements[id], true);
}

void TouchFeedbackManager::onTouchUp(int id) {
    if (id < 0 || id >= elementCount || lcd == nullptr) {
        return;
    }

    // For non-persistent feedback, clear immediately
    if (!elements[id].isPersistent) {
        elements[id].isActive = false;
        drawElement(elements[id], false);
    }
}

void TouchFeedbackManager::flash(int id) {
    if (id < 0 || id >= elementCount || lcd == nullptr) {
        return;
    }

    elements[id].isActive = true;
    elements[id].pressTime = millis();

    // Draw flash effect
    drawElement(elements[id], true);
}

void TouchFeedbackManager::update() {
    if (lcd == nullptr) return;

    unsigned long now = millis();

    for (int i = 0; i < elementCount; i++) {
        if (!elements[i].isActive) continue;
        if (elements[i].isPersistent) continue;

        // Check if duration has elapsed
        if (now - elements[i].pressTime >= elements[i].duration) {
            elements[i].isActive = false;
            drawElement(elements[i], false);
        }
    }
}

void TouchFeedbackManager::drawElement(const FeedbackElement& elem, bool pressed) {
    if (lcd == nullptr) return;

    uint32_t fillColor = pressed ? elem.pressedColor : elem.normalColor;
    uint32_t borderColor = pressed ? elem.borderColor : elem.normalColor;

    switch (elem.type) {
        case FEEDBACK_BUTTON:
            // Fill button
            if (elem.radius > 0) {
                lcd->fillRoundRect(elem.x, elem.y, elem.w, elem.h, elem.radius, fillColor);
                if (pressed) {
                    lcd->drawRoundRect(elem.x, elem.y, elem.w, elem.h, elem.radius, borderColor);
                }
            } else {
                lcd->fillRect(elem.x, elem.y, elem.w, elem.h, fillColor);
                if (pressed) {
                    lcd->drawRect(elem.x, elem.y, elem.w, elem.h, borderColor);
                }
            }
            break;

        case FEEDBACK_ICON_FLASH:
            // Flash icon with fill
            if (pressed) {
                lcd->fillRoundRect(elem.x, elem.y, elem.w, elem.h, elem.radius, fillColor);
            } else {
                // Clear by drawing background color (assuming black)
                lcd->fillRoundRect(elem.x, elem.y, elem.w, elem.h, elem.radius, 0x000000);
            }
            break;

        case FEEDBACK_LIST_SELECT:
            // Highlight list item
            if (pressed) {
                lcd->fillRect(elem.x, elem.y, elem.w, elem.h, fillColor);
                lcd->drawRect(elem.x, elem.y, elem.w, elem.h, borderColor);
                // Double border for emphasis
                lcd->drawRect(elem.x + 1, elem.y + 1, elem.w - 2, elem.h - 2, borderColor);
            } else {
                // Clear highlight
                lcd->fillRect(elem.x, elem.y, elem.w, elem.h, fillColor);
            }
            break;

        case FEEDBACK_HIGHLIGHT:
            // Simple color change
            lcd->fillRect(elem.x, elem.y, elem.w, elem.h, fillColor);
            break;

        default:
            break;
    }
}

void TouchFeedbackManager::clear(int id) {
    if (id < 0 || id >= elementCount) {
        return;
    }

    elements[id].isActive = false;
    drawElement(elements[id], false);
}

void TouchFeedbackManager::clearAll() {
    for (int i = 0; i < elementCount; i++) {
        if (elements[i].isActive) {
            elements[i].isActive = false;
            drawElement(elements[i], false);
        }
    }
}

bool TouchFeedbackManager::isActive(int id) {
    if (id < 0 || id >= elementCount) {
        return false;
    }
    return elements[id].isActive;
}
