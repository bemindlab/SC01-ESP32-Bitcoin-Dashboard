# Bitcoin Dashboard - Test Suite

Comprehensive unit test suite for the Bitcoin Dashboard project.

## Quick Start

```bash
# Run all tests
pio test -e native

# Run specific test suite
pio test -e native -f native/test_btc_dashboard_screen

# Run with verbose output
pio test -e native -v
```

## Test Organization

### Mock Objects (`test/mocks/`)

Mock hardware components for testing without physical devices:

- **MockLGFX.h** - LovyanGFX display mock
- **MockFT6X36.h** - FT6X36 touch controller mock
- **MockHTTPClient.h** - HTTP client mock for API calls
- **MockWiFi.h** - ESP32 WiFi mock
- **MockScreenManager.h** - Screen management mock

### Native Tests (`test/native/`)

Unit tests that run on desktop without ESP32 hardware:

#### Screen Tests

| Test Suite | Tests | Focus Areas |
|------------|-------|-------------|
| **test_btc_dashboard_screen** | 22 | Card layout, touch detection, price formatting, update intervals |
| **test_wifi_scan_screen** | 24 | Network list, RSSI bars, SSID truncation, scrolling |
| **test_settings_screen** | 18 | 480x320 layout optimization, option touch, navigation |
| **test_news_screen** | 22 | Content scrolling, text wrapping, loading animations |
| **test_trading_screen** | 23 | Trading signals, confidence bars, update intervals |

#### Logic Tests

| Test Suite | Tests | Focus Areas |
|------------|-------|-------------|
| **test_btc_data_parsing** | - | JSON parsing, API response handling |
| **test_data_formatting** | - | Number formatting, string operations |
| **test_screen_logic** | 20 | Touch calculations, coordinate transforms, timing |

**Total: 109+ unit tests**

## Test Coverage by Screen

### BTCDashboardScreen (test_btc_dashboard_screen)

✅ **Layout Tests**
- Card positions within display bounds (480x320)
- Cards do not overlap
- Symmetrical spacing
- Icons within display bounds

✅ **Touch Detection**
- WiFi icon touch (center, boundaries, outside)
- Settings icon touch
- Icons do not overlap

✅ **Data Formatting**
- Price formatting under/over $100k
- Mempool count formatting (1k threshold)

✅ **Update Logic**
- Price interval (30s)
- Block interval (60s)
- Mempool interval (30s)

✅ **UI Elements**
- Status bar positioning
- Swipe hint placement

### WiFiScanScreen (test_wifi_scan_screen)

✅ **Layout Tests**
- Network items fit on screen
- First item position
- Sequential positioning
- Items within display

✅ **Touch Detection**
- Touch to network index (first, third, last)
- Touch with scroll offset
- Touch validation (above/below list)
- Refresh button touch

✅ **Signal Strength**
- RSSI to bars (excellent/good/fair/weak)
- RSSI to color (green/yellow/orange)

✅ **Display**
- SSID truncation (25 chars + "...")
- Scroll offset clamping
- Scroll calculation

### SettingsScreen (test_settings_screen)

✅ **Layout Optimization**
- Perfect fit for 480x320 (310px used)
- 5 options positioned correctly
- Options do not overlap
- Option width fits screen

✅ **Touch Detection**
- Back button (expanded 50x35px area)
- All 5 options touchable
- Padding areas excluded
- Option boundaries

✅ **Visual Spacing**
- Reasonable padding (6px)
- Reasonable option height (48px)
- >90% screen space usage

✅ **Navigation**
- Back to dashboard
- WiFi settings navigation

### BTCNewsScreen (test_news_screen)

✅ **Layout**
- Header height (40px)
- Content area (280px)
- Buttons within bounds
- No button overlap

✅ **Touch Detection**
- Back button
- Refresh button
- Content area scrolling

✅ **Scrolling**
- Offset clamping (min/max)
- Max scroll calculation
- Scroll damping (50%)
- Scroll indicator

✅ **Text Rendering**
- Line height (20px)
- Lines per screen (13)
- Text area width (460px)
- Word wrapping (60 chars)

✅ **State Management**
- Initial state
- Loading state
- Success state
- Error state

✅ **Animation**
- Frame increment
- Spinner angle calculation

### BTCTradingSuggestionScreen (test_trading_screen)

✅ **Layout**
- Market summary box
- Signal badge position
- Recommendation area (135px)
- Disclaimer position
- All elements fit (≤320px)

✅ **Trading Signals**
- Signal colors (5 levels)
- Signal text (STRONG_BUY → STRONG_SELL)
- Signal icons (^^, ^, =, v, vv)

✅ **Confidence Bar**
- Width calculation (0-100%)
- Bar positioning

✅ **Update Logic**
- 5-minute interval (300000ms)
- Should/shouldn't update timing

✅ **Scrolling**
- Offset bounds
- Max scroll calculation

✅ **Touch Detection**
- Back button
- Refresh button
- Content scrolling

✅ **State Management**
- Initial/loading/success/error states

✅ **Text Display**
- Recommendation wrapping
- Key factors display

## Running Tests

### All Tests
```bash
pio test -e native
```

### Individual Test Suites
```bash
# Dashboard
pio test -e native -f native/test_btc_dashboard_screen

# WiFi Scan
pio test -e native -f native/test_wifi_scan_screen

# Settings
pio test -e native -f native/test_settings_screen

# News
pio test -e native -f native/test_news_screen

# Trading
pio test -e native -f native/test_trading_screen

# Screen Logic
pio test -e native -f native/test_screen_logic

# Data Parsing
pio test -e native -f native/test_btc_data_parsing

# Data Formatting
pio test -e native -f native/test_data_formatting
```

### With Verbose Output
```bash
pio test -e native -v
```

## Test Output Format

```
Testing...
test/native/test_btc_dashboard_screen/test_main.cpp:14:test_card_positions_within_bounds:PASS
test/native/test_btc_dashboard_screen/test_main.cpp:38:test_cards_do_not_overlap:PASS
test/native/test_btc_dashboard_screen/test_main.cpp:60:test_wifi_icon_touch_center:PASS
...

-----------------------
22 Tests 0 Failures 0 Ignored
OK
```

## Adding New Tests

1. Create test directory:
```bash
mkdir -p test/native/test_my_feature
```

2. Create `test_main.cpp`:
```cpp
#include <unity.h>

void test_my_feature() {
    TEST_ASSERT_EQUAL_INT(42, 42);
}

void setUp(void) {}
void tearDown(void) {}

int main(int argc, char **argv) {
    UNITY_BEGIN();
    RUN_TEST(test_my_feature);
    return UNITY_END();
}
```

3. Run test:
```bash
pio test -e native -f native/test_my_feature
```

## Mock Usage Examples

### Display Mock
```cpp
#include "test/mocks/MockLGFX.h"

MockLGFX lcd;
lcd.fillRect(10, 20, 100, 50, 0xFF0000);

TEST_ASSERT_EQUAL_INT(10, lcd.getLastX());
TEST_ASSERT_EQUAL_INT(20, lcd.getLastY());
TEST_ASSERT_EQUAL_UINT32(0xFF0000, lcd.getLastColor());
```

### Touch Mock
```cpp
#include "test/mocks/MockFT6X36.h"

MockFT6X36 touch;
touch.simulateTouch(240, 160);

TEST_ASSERT_TRUE(touch.touched());
TEST_ASSERT_EQUAL_INT(240, touch.getTouchX());
```

### WiFi Mock
```cpp
#include "test/mocks/MockWiFi.h"

WiFi.begin("TestNet", "pass");
TEST_ASSERT_EQUAL_INT(WL_CONNECTED, WiFi.status());
```

## Documentation

For comprehensive testing guide, see:
- **[Unit Testing Guide](../docs/guides/unit-testing-guide.md)** - Complete testing documentation

## CI/CD

Tests can be run in CI pipelines (GitHub Actions, GitLab CI, etc.):

```yaml
# .github/workflows/test.yml
- name: Run Unit Tests
  run: pio test -e native
```

## Dependencies

- **Unity Test Framework** - C unit testing framework
- **PlatformIO** - Build system and test runner
- **ArduinoJson** - JSON parsing (tested separately)

## Test Philosophy

1. **Fast** - Tests run in seconds on desktop
2. **Isolated** - No hardware dependencies
3. **Comprehensive** - All screens and logic covered
4. **Maintainable** - Clear test names and structure
5. **Automated** - Easy CI/CD integration

---

**Total Test Count:** 109+ tests
**Platform:** Native (desktop)
**Framework:** Unity
**Coverage:** All screens + core logic
