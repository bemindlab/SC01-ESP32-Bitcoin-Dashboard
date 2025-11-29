# Unit Testing Plan - Bitcoin Dashboard

## Executive Summary

This document outlines a comprehensive unit testing strategy for the SC01 Plus Bitcoin Dashboard project. The system uses ESP32-S3 hardware with embedded components (display, touch, WiFi, APIs), requiring a hybrid testing approach combining native C++ unit tests and hardware integration tests.

## Current Architecture Analysis

### Component Overview
```
src/
├── main.cpp                          # Application entry point
├── DisplayConfig.h                   # Hardware display configuration
└── screens/
    ├── ScreenManager (h/cpp)         # Screen navigation & touch handling
    ├── WiFiScanScreen (h/cpp)        # WiFi network selection UI
    └── BTCDashboardScreen (h/cpp)    # Bitcoin data display & API integration
```

### Key Dependencies
- **Hardware:** LovyanGFX (display), FT6X36 (touch), ESP32 WiFi
- **Libraries:** ArduinoJson, HTTPClient
- **External APIs:** mempool.space (Bitcoin data)

## Testing Strategy

### 1. Native Unit Tests (Desktop)
**Framework:** PlatformIO Native + Unity/Google Test

**Testable Components:**
- Business logic (data parsing, calculations)
- Data structures (BTCData)
- API response parsing (without actual HTTP)
- Screen state management logic

**Advantages:**
- Fast execution (no hardware upload)
- Easy CI/CD integration
- Debugger support
- Mock external dependencies

### 2. Hardware Integration Tests (On-Device)
**Framework:** PlatformIO Test Framework

**Testable Components:**
- Display rendering
- Touch input detection
- WiFi connection
- Actual API calls
- Screen transitions
- Full user workflows

**Advantages:**
- Real hardware validation
- Actual network conditions
- Display/touch accuracy
- Performance testing

## Detailed Test Plan

### Phase 1: Test Infrastructure Setup

#### 1.1 PlatformIO Native Environment
```ini
[env:native]
platform = native
test_framework = unity
build_flags =
    -std=c++11
    -DUNIT_TEST
lib_deps =
    ArduinoJson@^6.21.0
```

#### 1.2 Hardware Test Environment
```ini
[env:test_sc01_plus]
platform = espressif32
board = esp32-s3-devkitc-1
framework = arduino
test_framework = unity
```

#### 1.3 Directory Structure
```
test/
├── native/                    # Native (desktop) tests
│   ├── test_btc_data_parsing/
│   ├── test_screen_logic/
│   └── test_data_formatting/
├── embedded/                  # On-device tests
│   ├── test_display/
│   ├── test_touch/
│   ├── test_wifi/
│   └── test_api_integration/
└── mocks/                     # Mock implementations
    ├── MockLGFX.h
    ├── MockHTTPClient.h
    └── MockWiFi.h
```

### Phase 2: Native Unit Tests

#### 2.1 BTCData Parsing Tests
**File:** `test/native/test_btc_data_parsing/test_main.cpp`

**Test Cases:**
- ✅ Parse valid price JSON
- ✅ Parse valid blocks JSON array
- ✅ Parse valid mempool JSON
- ✅ Parse valid fees JSON
- ✅ Handle malformed JSON gracefully
- ✅ Handle missing fields
- ✅ Handle null values
- ✅ Handle extreme values (very large/small numbers)

**Example:**
```cpp
void test_parse_valid_price_json() {
    const char* json = "{\"USD\":91396,\"EUR\":85000}";
    StaticJsonDocument<256> doc;
    deserializeJson(doc, json);

    BTCData data;
    data.priceUSD = doc["USD"].as<float>();
    data.priceEUR = doc["EUR"].as<float>();

    TEST_ASSERT_EQUAL_FLOAT(91396.0, data.priceUSD);
    TEST_ASSERT_EQUAL_FLOAT(85000.0, data.priceEUR);
}
```

#### 2.2 Data Formatting Tests
**File:** `test/native/test_data_formatting/test_main.cpp`

**Test Cases:**
- ✅ Format price under 100k correctly
- ✅ Format price over 100k with 'k' suffix
- ✅ Format mempool count under 1000
- ✅ Format mempool count over 1000 with 'k' suffix
- ✅ Format block height correctly
- ✅ Format mempool size in MB
- ✅ Format fee rates

**Example:**
```cpp
void test_format_price_over_100k() {
    float price = 125000.0;
    char buf[64];

    if (price >= 100000) {
        snprintf(buf, sizeof(buf), "$%.0fk", price / 1000);
    } else {
        snprintf(buf, sizeof(buf), "$%.0f", price);
    }

    TEST_ASSERT_EQUAL_STRING("$125k", buf);
}
```

#### 2.3 Screen State Logic Tests
**File:** `test/native/test_screen_logic/test_main.cpp`

**Test Cases:**
- ✅ WiFi icon touch detection (coordinates in bounds)
- ✅ WiFi icon touch detection (coordinates out of bounds)
- ✅ Network selection touch calculation
- ✅ Coordinate transformation (landscape mode)
- ✅ Screen transition logic
- ✅ Update interval timing logic

### Phase 3: Hardware Integration Tests

#### 3.1 Display Tests
**File:** `test/embedded/test_display/test_main.cpp`

**Test Cases:**
- ✅ Display initialization succeeds
- ✅ Display rotation set correctly (landscape)
- ✅ Display brightness control works
- ✅ Basic drawing functions work (fillRect, drawText)
- ✅ Card rendering doesn't crash
- ✅ Color values render correctly
- ✅ WiFi icon draws without errors

#### 3.2 Touch Tests
**File:** `test/embedded/test_touch/test_main.cpp`

**Test Cases:**
- ✅ Touch controller initializes
- ✅ Touch detection works (manual tap required)
- ✅ Coordinate transformation is accurate
- ✅ Touch events trigger callbacks
- ✅ Debouncing prevents double-taps

**Manual Test Procedure:**
```cpp
void test_touch_detection() {
    // Instruct user via Serial
    Serial.println("TAP THE SCREEN in 5 seconds");
    delay(5000);

    // Check if touch was detected
    TEST_ASSERT_TRUE(touchDetected);
    TEST_ASSERT_TRUE(touchX >= 0 && touchX <= 480);
    TEST_ASSERT_TRUE(touchY >= 0 && touchY <= 320);
}
```

#### 3.3 WiFi Tests
**File:** `test/embedded/test_wifi/test_main.cpp`

**Test Cases:**
- ✅ WiFi scan finds networks
- ✅ WiFi connect succeeds with valid credentials
- ✅ WiFi connect fails with invalid credentials
- ✅ WiFi reconnect after disconnect
- ✅ IP address assignment
- ✅ WiFi signal strength (RSSI) reading

**Note:** Requires actual WiFi network for testing

#### 3.4 API Integration Tests
**File:** `test/embedded/test_api_integration/test_main.cpp`

**Test Cases:**
- ✅ Price API returns valid response
- ✅ Blocks API returns valid response
- ✅ Mempool API returns valid response
- ✅ Fees API returns valid response
- ✅ API timeout handling (simulate slow network)
- ✅ API error handling (404, 500 responses)
- ✅ Network disconnection during API call
- ✅ Concurrent API calls don't interfere

**Note:** Requires internet connection

#### 3.5 Full Workflow Tests
**File:** `test/embedded/test_workflow/test_main.cpp`

**Test Cases:**
- ✅ Boot → WiFi scan → Network selection → Dashboard
- ✅ Dashboard → WiFi icon tap → WiFi scan
- ✅ WiFi connect failure → Retry flow
- ✅ Dashboard auto-refresh (price, blocks, mempool)
- ✅ Memory stability during long runs (no leaks)
- ✅ Display stays responsive during API calls

### Phase 4: Mock Implementations

#### 4.1 MockLGFX
**File:** `test/mocks/MockLGFX.h`

**Purpose:** Simulate display without hardware

```cpp
class MockLGFX {
public:
    void init() { initCalled = true; }
    void setRotation(uint8_t r) { rotation = r; }
    void fillScreen(uint32_t color) { screenColor = color; }
    // ... other methods

    // Verification helpers
    bool wasInitCalled() { return initCalled; }
    uint8_t getRotation() { return rotation; }

private:
    bool initCalled = false;
    uint8_t rotation = 0;
    uint32_t screenColor = 0;
};
```

#### 4.2 MockHTTPClient
**File:** `test/mocks/MockHTTPClient.h`

**Purpose:** Simulate API responses without network

```cpp
class MockHTTPClient {
public:
    void setMockResponse(int code, const char* payload) {
        mockCode = code;
        mockPayload = payload;
    }

    int GET() { return mockCode; }
    String getString() { return mockPayload; }

private:
    int mockCode = 200;
    String mockPayload;
};
```

#### 4.3 MockWiFi
**File:** `test/mocks/MockWiFi.h`

**Purpose:** Simulate WiFi without hardware

```cpp
class MockWiFi {
public:
    void begin(const char* ssid, const char* pass) {
        connected = true;
    }

    int status() { return connected ? WL_CONNECTED : WL_DISCONNECTED; }

    void setConnected(bool state) { connected = state; }

private:
    bool connected = false;
};
```

## Test Execution Strategy

### Local Development
```bash
# Run native tests (fast, during development)
pio test -e native

# Run hardware tests (slower, before commit)
pio test -e test_sc01_plus --upload-port /dev/cu.usbmodem1101
```

### CI/CD Pipeline
```yaml
# .github/workflows/test.yml
name: Unit Tests

on: [push, pull_request]

jobs:
  native-tests:
    runs-on: ubuntu-latest
    steps:
      - uses: actions/checkout@v2
      - name: Setup PlatformIO
        run: pip install platformio
      - name: Run Native Tests
        run: pio test -e native

  # Hardware tests require actual device (skip in CI)
  # Run manually before releases
```

### Pre-Commit Hooks
```bash
# .git/hooks/pre-commit
#!/bin/sh
pio test -e native
if [ $? -ne 0 ]; then
    echo "Native tests failed. Commit aborted."
    exit 1
fi
```

## Test Coverage Goals

### Phase 1 (MVP)
- ✅ 80% coverage of data parsing logic
- ✅ 70% coverage of formatting logic
- ✅ Basic hardware smoke tests

### Phase 2 (Production)
- ✅ 90% coverage of business logic
- ✅ 80% coverage of UI logic
- ✅ Comprehensive hardware tests
- ✅ API integration tests
- ✅ Full workflow tests

### Phase 3 (Advanced)
- ✅ Performance benchmarks
- ✅ Memory leak detection
- ✅ Stress tests (long runs)
- ✅ Edge case coverage

## Implementation Priority

### Week 1: Foundation
1. Set up test infrastructure (native + embedded)
2. Create mock implementations
3. Write first 5 native tests (data parsing)

### Week 2: Core Logic
4. Complete all data parsing tests
5. Complete all formatting tests
6. Add screen logic tests

### Week 3: Hardware
7. Implement display tests
8. Implement touch tests
9. Implement WiFi tests

### Week 4: Integration
10. Implement API integration tests
11. Implement workflow tests
12. Set up CI/CD

## Success Metrics

- **Code Coverage:** >80% for testable components
- **Test Execution Time:** <2 min for native, <5 min for embedded
- **Defect Detection:** Catch bugs before hardware upload
- **Regression Prevention:** No previously working features break
- **CI/CD Integration:** All tests run automatically on commit

## Challenges & Solutions

### Challenge 1: Hardware Dependencies
**Problem:** Many components require actual hardware (display, touch)

**Solution:**
- Use mocks for native tests
- Maintain separate hardware test suite
- Document manual test procedures

### Challenge 2: API Dependency
**Problem:** Tests depend on external API availability

**Solution:**
- Mock HTTP responses for unit tests
- Use separate integration test suite
- Cache known good responses for offline testing

### Challenge 3: Touch Testing
**Problem:** Touch requires manual interaction

**Solution:**
- Automated coordinate math tests
- Manual workflow tests with clear instructions
- Touch simulator for CI (future enhancement)

### Challenge 4: Memory Constraints
**Problem:** ESP32-S3 has limited RAM for tests

**Solution:**
- Keep test files small
- Test one component at a time
- Monitor memory usage during tests

## Future Enhancements

1. **Test Coverage Reports:** Generate HTML coverage reports
2. **Performance Benchmarks:** Track API response times, render times
3. **Fuzz Testing:** Random input generation for robustness
4. **Visual Regression Testing:** Screenshot comparison
5. **Load Testing:** Simulate heavy API usage
6. **Touch Automation:** Hardware-in-the-loop touch simulation

## Conclusion

This testing plan provides a comprehensive approach to ensuring quality in the Bitcoin Dashboard project. By combining native unit tests for business logic with hardware integration tests for embedded components, we achieve fast iteration during development while maintaining confidence in the final product.

**Next Steps:**
1. Review and approve this plan
2. Create test infrastructure (directories, platformio.ini)
3. Implement Phase 1 tests (native data parsing)
4. Iterate and expand coverage

---

**Document Version:** 1.0
**Last Updated:** 2025-11-28
**Author:** Claude Code
**Status:** Draft - Pending Approval
