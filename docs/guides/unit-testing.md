# Unit Testing Guide - Bitcoin Dashboard

This guide covers the comprehensive unit test suite for the Bitcoin Dashboard project, including screen tests, mocks, and testing best practices.

## Table of Contents

- [Overview](#overview)
- [Test Infrastructure](#test-infrastructure)
- [Running Tests](#running-tests)
- [Test Coverage](#test-coverage)
- [Mock Objects](#mock-objects)
- [Screen Tests](#screen-tests)
- [Writing New Tests](#writing-new-tests)
- [CI/CD Integration](#cicd-integration)

## Overview

The Bitcoin Dashboard uses the **Unity Test Framework** for native (desktop) unit testing. Tests are designed to run without hardware, using mocks for display, touch, and WiFi components.

### Testing Philosophy

- **Unit tests** verify individual components in isolation
- **Mocks** simulate hardware without dependencies
- **Fast feedback** - tests run on desktop in seconds
- **Comprehensive coverage** - all screens and logic tested
- **CI-friendly** - automated testing in pipelines

## Test Infrastructure

### Directory Structure

```
test/
├── mocks/                          # Mock hardware objects
│   ├── MockLGFX.h                 # Display mock
│   ├── MockFT6X36.h               # Touch controller mock
│   ├── MockHTTPClient.h           # HTTP client mock
│   ├── MockWiFi.h                 # WiFi mock
│   └── MockScreenManager.h        # Screen manager mock
└── native/                         # Native (desktop) tests
    ├── test_btc_dashboard_screen/ # Dashboard screen tests
    ├── test_wifi_scan_screen/     # WiFi scan screen tests
    ├── test_settings_screen/      # Settings screen tests
    ├── test_news_screen/          # News screen tests
    ├── test_trading_screen/       # Trading screen tests
    ├── test_btc_data_parsing/     # Data parsing tests
    ├── test_data_formatting/      # Data formatting tests
    └── test_screen_logic/         # Screen logic tests
```

### PlatformIO Configuration

Tests are configured in `platformio.ini`:

```ini
[env:native]
platform = native
test_framework = unity
test_filter = native/*
build_flags =
    -std=c++11
    -DUNIT_TEST
lib_deps =
    bblanchon/ArduinoJson@^6.21.0
```

## Running Tests

### Run All Tests

```bash
# Using PlatformIO CLI
pio test -e native

# Using Make
make test
```

### Run Specific Test Suite

```bash
# Dashboard screen tests
pio test -e native -f native/test_btc_dashboard_screen

# WiFi scan screen tests
pio test -e native -f native/test_wifi_scan_screen

# Settings screen tests
pio test -e native -f native/test_settings_screen

# News screen tests
pio test -e native -f native/test_news_screen

# Trading screen tests
pio test -e native -f native/test_trading_screen
```

### Run Tests with Verbose Output

```bash
pio test -e native -v
```

## Test Coverage

### Overall Coverage

| Component | Tests | Coverage |
|-----------|-------|----------|
| BTCDashboardScreen | 22 tests | Layout, Touch, Formatting, Updates |
| WiFiScanScreen | 24 tests | Network List, Touch, RSSI, Scrolling |
| SettingsScreen | 18 tests | Layout, Touch, Navigation |
| BTCNewsScreen | 22 tests | Scrolling, Loading, Animation |
| BTCTradingSuggestionScreen | 23 tests | Signals, Confidence, Updates |
| **Total** | **109+ tests** | **Comprehensive** |

### Screen Test Categories

Each screen test suite covers:

1. **Layout Tests** - Element positions, bounds checking, no overlaps
2. **Touch Detection** - Button/icon hit testing, boundaries
3. **Data Formatting** - Text display, truncation, wrapping
4. **State Management** - Loading, success, error states
5. **Update Logic** - Timing intervals, refresh triggers
6. **Scrolling** - Offset calculation, clamping, indicators

## Mock Objects

### MockLGFX (Display Mock)

Simulates the LovyanGFX display library for testing UI rendering.

```cpp
#include "test/mocks/MockLGFX.h"

MockLGFX lcd;

// Initialize
lcd.init();
lcd.setRotation(1);
lcd.setBrightness(255);

// Drawing operations
lcd.fillScreen(0x000000);
lcd.fillRect(10, 10, 100, 50, 0xFF0000);

// Verification
TEST_ASSERT_EQUAL_INT(10, lcd.getLastX());
TEST_ASSERT_EQUAL_UINT32(0xFF0000, lcd.getLastColor());
```

**Features:**
- Tracks all drawing operations
- Records last coordinates, colors, sizes
- Verifies text operations
- Simulates rotation and brightness

### MockFT6X36 (Touch Mock)

Simulates the FT6X36 capacitive touch controller.

```cpp
#include "test/mocks/MockFT6X36.h"

MockFT6X36 touch;

// Initialize
touch.begin(40);

// Simulate touch
touch.simulateTouch(240, 160);
TEST_ASSERT_TRUE(touch.touched());

uint16_t x, y;
touch.getPoint(&x, &y);
TEST_ASSERT_EQUAL_INT(240, x);
TEST_ASSERT_EQUAL_INT(160, y);

// Simulate release
touch.simulateRelease();
TEST_ASSERT_FALSE(touch.touched());
```

**Features:**
- Simulates touch events at specific coordinates
- Tracks touch count
- Supports single-touch operations
- Validates initialization

### MockWiFi (WiFi Mock)

Simulates ESP32 WiFi functionality.

```cpp
#include "test/mocks/MockWiFi.h"

// Connect to network
WiFi.begin("TestNetwork", "password");
TEST_ASSERT_EQUAL_INT(WL_CONNECTED, WiFi.status());

// Scan networks
const char* ssids[] = {"Network1", "Network2"};
int rssis[] = {-45, -60};
uint8_t encryption[] = {3, 3};
WiFi.setMockNetworks(2, ssids, rssis, encryption);

int count = WiFi.scanNetworks();
TEST_ASSERT_EQUAL_INT(2, count);
TEST_ASSERT_EQUAL_STRING("Network1", WiFi.SSID(0));
TEST_ASSERT_EQUAL_INT(-45, WiFi.RSSI(0));
```

**Features:**
- Simulates connection states
- Mock network scanning
- RSSI and encryption type support
- IP address simulation

### MockScreenManager

Simulates the screen management system.

```cpp
#include "test/mocks/MockScreenManager.h"

MockScreenManager manager;

// Switch screens
manager.switchScreen(SCREEN_DASHBOARD);
TEST_ASSERT_EQUAL_INT(SCREEN_DASHBOARD, manager.getCurrentScreen());

manager.switchScreen(SCREEN_SETTINGS);
TEST_ASSERT_EQUAL_INT(SCREEN_SETTINGS, manager.getCurrentScreen());
TEST_ASSERT_EQUAL_INT(SCREEN_DASHBOARD, manager.getLastScreen());

// Access mocked hardware
MockLGFX* lcd = manager.getLCD();
MockFT6X36* touch = manager.getTouch();
```

**Features:**
- Tracks screen transitions
- Provides access to display and touch mocks
- Counts screen switches
- Reset functionality for tests

## Screen Tests

### BTCDashboardScreen Tests

**Location:** `test/native/test_btc_dashboard_screen/test_main.cpp`

**Coverage:**
- Card positions and layout (4 cards: Price, Block, Mempool, Fees)
- WiFi and Settings icon touch detection
- Price formatting ($45,678 vs $123k)
- Mempool count formatting (789 vs 12.3k)
- Update intervals (30s price, 60s blocks, 30s mempool)
- Status bar positioning

**Key Tests:**
```cpp
test_card_positions_within_bounds()
test_cards_do_not_overlap()
test_wifi_icon_touch_center()
test_price_format_over_100k()
test_price_update_interval()
```

### WiFiScanScreen Tests

**Location:** `test/native/test_wifi_scan_screen/test_main.cpp`

**Coverage:**
- Network list layout (50px items, scrollable)
- Touch-to-network-index calculation
- RSSI to signal bars conversion (4-3-2-1)
- RSSI to signal color mapping (green/yellow/orange)
- SSID truncation (max 25 chars + "...")
- Scroll offset clamping

**Key Tests:**
```cpp
test_network_items_fit_on_screen()
test_touch_to_network_index_first_item()
test_rssi_to_signal_bars_excellent()
test_ssid_truncation_long()
test_scroll_calculation()
```

### SettingsScreen Tests

**Location:** `test/native/test_settings_screen/test_main.cpp`

**Coverage:**
- Layout optimization for 480x320 (exactly 310px used)
- 5 options fit perfectly on screen
- Option touch detection with 6px padding
- Back button expanded touch area (50x35px)
- No footer overflow
- All options touchable

**Key Tests:**
```cpp
test_settings_layout_fits_screen()
test_option_positions_within_bounds()
test_all_options_touchable()
test_total_vertical_space_usage()
```

### BTCNewsScreen Tests

**Location:** `test/native/test_news_screen/test_main.cpp`

**Coverage:**
- Content area scrolling (280px height)
- Text word wrapping (60 chars/line)
- Scroll indicator positioning
- Loading animation frames
- State management (loading/success/error)
- Line height and spacing (20px)

**Key Tests:**
```cpp
test_max_scroll_calculation()
test_text_word_wrapping_threshold()
test_scroll_indicator_position_calculation()
test_spinner_angle_calculation()
```

### BTCTradingSuggestionScreen Tests

**Location:** `test/native/test_trading_screen/test_main.cpp`

**Coverage:**
- Trading signal colors (5 levels: STRONG_BUY to STRONG_SELL)
- Signal text and icons ("^^", "^", "=", "v", "vv")
- Confidence bar width calculation (0-100%)
- 5-minute update interval
- Recommendation text wrapping
- Market summary positioning

**Key Tests:**
```cpp
test_signal_color_strong_buy()
test_signal_text_hold()
test_confidence_bar_width_50_percent()
test_update_interval_5_minutes()
```

## Writing New Tests

### Test Template

```cpp
#include <unity.h>

// Define constants
#define MY_CONSTANT 100

// Test function
void test_my_feature() {
    // Arrange
    int value = 50;

    // Act
    int result = value * 2;

    // Assert
    TEST_ASSERT_EQUAL_INT(100, result);
}

// Setup (runs before each test)
void setUp(void) {
    // Initialize test fixtures
}

// Teardown (runs after each test)
void tearDown(void) {
    // Clean up
}

// Main test runner
int main(int argc, char **argv) {
    UNITY_BEGIN();

    RUN_TEST(test_my_feature);

    return UNITY_END();
}
```

### Unity Test Assertions

Common assertions used in the test suite:

```cpp
// Integer comparisons
TEST_ASSERT_EQUAL_INT(expected, actual);
TEST_ASSERT_TRUE(condition);
TEST_ASSERT_FALSE(condition);

// String comparisons
TEST_ASSERT_EQUAL_STRING("expected", actual);

// Numeric comparisons
TEST_ASSERT_EQUAL_UINT32(expected, actual);
TEST_ASSERT_GREATER_THAN(threshold, actual);
TEST_ASSERT_LESS_THAN(threshold, actual);

// Pointer checks
TEST_ASSERT_NOT_NULL(pointer);
TEST_ASSERT_NULL(pointer);
```

### Best Practices

1. **Test One Thing** - Each test should verify a single behavior
2. **Clear Names** - Use descriptive test function names
3. **Arrange-Act-Assert** - Structure tests clearly
4. **Independent Tests** - Tests should not depend on each other
5. **Use Mocks** - Avoid hardware dependencies
6. **Test Edge Cases** - Boundaries, nulls, extremes
7. **Readable Assertions** - Make failures easy to debug

### Example: Adding a New Screen Test

1. Create test directory:
```bash
mkdir -p test/native/test_my_screen
```

2. Create test file `test/native/test_my_screen/test_main.cpp`:
```cpp
#include <unity.h>

#define MY_SCREEN_WIDTH 480
#define MY_SCREEN_HEIGHT 320

void test_my_screen_layout() {
    int elementX = 10;
    int elementY = 20;

    TEST_ASSERT_TRUE(elementX >= 0);
    TEST_ASSERT_TRUE(elementX < MY_SCREEN_WIDTH);
    TEST_ASSERT_TRUE(elementY >= 0);
    TEST_ASSERT_TRUE(elementY < MY_SCREEN_HEIGHT);
}

void setUp(void) {}
void tearDown(void) {}

int main(int argc, char **argv) {
    UNITY_BEGIN();
    RUN_TEST(test_my_screen_layout);
    return UNITY_END();
}
```

3. Run the new test:
```bash
pio test -e native -f native/test_my_screen
```

## CI/CD Integration

### GitHub Actions Example

Create `.github/workflows/test.yml`:

```yaml
name: Unit Tests

on: [push, pull_request]

jobs:
  test:
    runs-on: ubuntu-latest

    steps:
      - uses: actions/checkout@v3

      - name: Set up Python
        uses: actions/setup-python@v4
        with:
          python-version: '3.x'

      - name: Install PlatformIO
        run: pip install platformio

      - name: Run Unit Tests
        run: pio test -e native

      - name: Upload Test Results
        if: always()
        uses: actions/upload-artifact@v3
        with:
          name: test-results
          path: .pio/test/
```

### Makefile Integration

Add to `Makefile`:

```makefile
.PHONY: test test-verbose test-dashboard test-wifi test-settings

test:
\t@echo "Running all unit tests..."
\tpio test -e native

test-verbose:
\t@echo "Running tests with verbose output..."
\tpio test -e native -v

test-dashboard:
\t@echo "Running dashboard screen tests..."
\tpio test -e native -f native/test_btc_dashboard_screen

test-wifi:
\t@echo "Running WiFi scan screen tests..."
\tpio test -e native -f native/test_wifi_scan_screen

test-settings:
\t@echo "Running settings screen tests..."
\tpio test -e native -f native/test_settings_screen
```

## Test Output Example

```
test/native/test_btc_dashboard_screen/test_main.cpp:14:test_card_positions_within_bounds:PASS
test/native/test_btc_dashboard_screen/test_main.cpp:38:test_cards_do_not_overlap:PASS
test/native/test_btc_dashboard_screen/test_main.cpp:60:test_wifi_icon_touch_center:PASS
test/native/test_btc_dashboard_screen/test_main.cpp:95:test_price_format_over_100k:PASS

-----------------------
22 Tests 0 Failures 0 Ignored
OK
```

## Troubleshooting

### Test Fails on CI but Passes Locally

- Check platform-specific paths
- Verify all dependencies are in `lib_deps`
- Ensure `-DUNIT_TEST` flag is set

### Mock Not Found

- Verify mock header is in `test/mocks/`
- Check `#include` paths are correct
- Ensure test filter matches directory structure

### Assertion Failures

- Use verbose mode: `pio test -e native -v`
- Check test output for detailed error messages
- Verify expected vs. actual values
- Add debug `printf()` statements if needed

## Resources

- [Unity Test Framework](http://www.throwtheswitch.org/unity)
- [PlatformIO Testing](https://docs.platformio.org/en/latest/advanced/unit-testing/)
- [Writing Good Unit Tests](https://martinfowler.com/articles/practical-test-pyramid.html)

---

**Last Updated:** 2025-01-28
**Version:** 1.0
