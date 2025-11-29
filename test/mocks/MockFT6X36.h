#ifndef MOCK_FT6X36_H
#define MOCK_FT6X36_H

#include <stdint.h>

/**
 * Mock FT6X36 touch controller for native unit testing
 * Simulates touch operations without hardware
 */
class MockFT6X36 {
public:
    MockFT6X36() : touchCount(0), touchX(0), touchY(0), touchDetected(false) {}

    // Initialize (simulated)
    bool begin(uint8_t threshold = 40) {
        initialized = true;
        return true;
    }

    // Get number of touches (0 or 1 for single-touch)
    uint8_t touched() {
        return touchDetected ? 1 : 0;
    }

    // Get touch coordinates
    void getPoint(uint16_t* x, uint16_t* y) {
        if (x) *x = touchX;
        if (y) *y = touchY;
    }

    // Mock methods for testing
    void simulateTouch(uint16_t x, uint16_t y) {
        touchDetected = true;
        touchX = x;
        touchY = y;
        touchCount++;
    }

    void simulateRelease() {
        touchDetected = false;
    }

    void reset() {
        touchDetected = false;
        touchX = 0;
        touchY = 0;
        touchCount = 0;
    }

    // Verification helpers
    bool isTouched() { return touchDetected; }
    uint16_t getTouchX() { return touchX; }
    uint16_t getTouchY() { return touchY; }
    uint32_t getTouchCount() { return touchCount; }
    bool isInitialized() { return initialized; }

private:
    bool initialized;
    bool touchDetected;
    uint16_t touchX;
    uint16_t touchY;
    uint32_t touchCount;
};

#endif // MOCK_FT6X36_H
