# Documentation Index

**Project:** Bitcoin Dashboard for Panlee SC01 Plus
**Last Updated:** 2025-11-28
**Total Documents:** 11 files across 5 categories

---

## Table of Contents

- [Hardware Documentation](#hardware-documentation)
- [Testing Documentation](#testing-documentation)
- [Guide Documentation](#guide-documentation)
- [Development Documentation](#development-documentation)
- [Feature Documentation](#feature-documentation)
- [Quick Links](#quick-links)

---

## Hardware Documentation

### 📦 [`hardware/`](./hardware/)

| # | Document | Type | Size | Description |
|---|----------|------|------|-------------|
| 1 | [WT32-SC01_Plus_ESP32-S3_Datasheet.pdf](./hardware/WT32-SC01_Plus_ESP32-S3_Datasheet.pdf) | PDF | 1.05 MB | Official hardware datasheet and specifications |

**Hardware Contents:**
- ESP32-S3-WROVER-N16R2 module specifications
- 3.5" IPS LCD display (ST7796 driver, 320x480, 8-bit parallel)
- FT6X36 capacitive touch controller (I2C interface)
- Complete GPIO pin mappings and electrical characteristics
- Firmware upload procedures and bootloader instructions
- Power supply specifications and consumption
- Physical dimensions and mounting options

**Quick Specs:**
- **MCU:** ESP32-S3-WROVER-N16R2 (Dual-core Xtensa LX7, 240MHz)
- **Flash:** 16MB SPI Flash
- **PSRAM:** 2MB PSRAM (OPI mode)
- **Display:** 320x480 IPS TFT, 8-bit parallel interface
- **Touch:** I2C (SDA=GPIO6, SCL=GPIO5, INT=GPIO7)
- **WiFi:** 802.11 b/g/n (2.4GHz only)
- **Bluetooth:** BLE 5.0

---

## Testing Documentation

### 🧪 [`testing/`](./testing/)

| # | Document | Last Updated | Lines | Description |
|---|----------|--------------|-------|-------------|
| 2 | [unit-testing-plan.md](./testing/unit-testing-plan.md) | 2025-11-28 | 469 | Comprehensive unit testing strategy and implementation plan |
| 3 | [testing-implementation-status.md](./testing/testing-implementation-status.md) | 2025-11-28 | 339 | Current test implementation status, results, and coverage |
| 4 | [simulator-testing.md](./testing/simulator-testing.md) | - | - | Testing procedures using desktop simulators |

**Testing Summary:**

**Phase 1: Native Tests ✅ Complete**
- **Total Tests:** 53 passing (0 failing)
- **Execution Time:** ~3.2 seconds
- **Coverage:** ~90% of testable business logic
- **Test Command:** `make test-native`

**Test Suites:**
1. **BTC Data Parsing (12 tests)** - JSON parsing for all API endpoints
   - Price API (USD/EUR)
   - Blocks API (height, TX count, timestamp)
   - Mempool API (count, size)
   - Fees API (fast/medium/slow)
   - Error handling (malformed JSON, missing fields, null values)

2. **Data Formatting (19 tests)** - Display text formatting
   - Price formatting (under/over 100k with 'k' suffix)
   - Mempool count formatting (under/over 1000)
   - Block height formatting
   - Mempool size in MB
   - Fee rate formatting (sat/vB)
   - Transaction count formatting

3. **Screen Logic (22 tests)** - UI interaction logic
   - WiFi icon touch detection (bounds checking)
   - Network selection touch calculation
   - Coordinate transformation (landscape mode)
   - Update interval timing
   - RSSI to signal bars conversion

**Phase 2: Hardware Tests ⏳ Pending**
- Display rendering tests
- Touch input tests
- WiFi connection tests
- API integration tests
- Full workflow tests

**Mock Implementations:**
- `test/mocks/MockLGFX.h` - Display operations mock
- `test/mocks/MockHTTPClient.h` - HTTP client mock with preset responses
- `test/mocks/MockWiFi.h` - WiFi operations mock

**Test Infrastructure:**
```
test/
├── native/                    # Native (desktop) tests ✅
│   ├── test_btc_data_parsing/
│   ├── test_data_formatting/
│   └── test_screen_logic/
├── embedded/                  # Hardware tests ⏳
│   ├── test_display/
│   ├── test_touch/
│   ├── test_wifi/
│   ├── test_api_integration/
│   └── test_workflow/
└── mocks/                    # Mock implementations ✅
    ├── MockLGFX.h
    ├── MockHTTPClient.h
    └── MockWiFi.h
```

---

## Guide Documentation

### 📘 [`guides/`](./guides/)

| # | Document | Purpose | Key Topics |
|---|----------|---------|------------|
| 5 | [debugging-guide.md](./guides/debugging-guide.md) | Debugging procedures and troubleshooting | Serial monitor, exception decoder, crash analysis |
| 6 | [manual-upload-guide.md](./guides/manual-upload-guide.md) | Manual firmware upload instructions | Bootloader mode, upload procedure, common errors |

**Debugging Guide Contents:**
- Serial monitor setup (115200 baud)
- Reading stack traces and exception messages
- Using ESP32 exception decoder
- Common error patterns and solutions
- Memory debugging techniques
- Touch and display debugging
- Network debugging (WiFi, API calls)

**Manual Upload Guide Contents:**
- When to use manual upload (auto-upload failures)
- Hardware button procedure (BOOT + RST)
- Step-by-step upload sequence
- Troubleshooting upload errors
- Alternative upload methods
- USB driver issues

**Common Commands:**
```bash
# Serial monitor
make monitor

# Debug monitor with exception decoder
make debug-monitor

# Manual upload
make upload
# (Hold BOOT, press RST, release both when upload starts)
```

---

## Development Documentation

### 🚀 [`development/`](./development/)

| # | Document | Status | Description |
|---|----------|--------|-------------|
| 7 | [development-plan.md](./development/development-plan.md) | Planning | Project roadmap and development phases |
| 8 | [ui-improvements.md](./development/ui-improvements.md) | Implemented | UI/UX enhancement proposals and iterations |

**Development Phases:**

**Phase 1 ✅ Complete - Core Dashboard**
- ✅ WiFi scan and network selection with touch interface
- ✅ Bitcoin dashboard with 4-card layout
- ✅ Real-time data from mempool.space API
- ✅ WiFi icon for easy reconfiguration
- ✅ High-contrast Bitcoin-themed color palette
- ✅ Touch screen navigation with coordinate transformation

**Phase 2 ✅ In Progress - Testing Infrastructure**
- ✅ Native unit tests (53 passing)
- ✅ Mock implementations for hardware components
- ✅ Test execution framework (PlatformIO + Unity)
- ✅ Makefile test commands
- ⏳ Hardware integration tests (pending)

**Phase 3 ⏳ Planned - Advanced Testing & CI/CD**
- ⏳ Hardware integration tests on device
- ⏳ GitHub Actions CI/CD pipeline
- ⏳ Coverage reporting
- ⏳ Pre-commit hooks

**Phase 4 📋 Future - Additional Features**
- 📋 Bitcoin news feed integration
- 📋 Price alerts and notifications
- 📋 Settings screen (WiFi credentials storage)
- 📋 Historical price charts
- 📋 Multiple fiat currency support

**UI Improvements:**
- Ultra-high contrast color scheme (pure black background)
- Pure color accents (green, cyan, yellow, orange)
- Large text sizes (4x) for readability
- Triple borders on cards for emphasis
- Optimized WiFi icon positioning (top-right)
- Touch feedback with visual confirmation

---

## Feature Documentation

### ✨ [`features/`](./features/)

| # | Document | Status | Complexity | Description |
|---|----------|--------|------------|-------------|
| 9 | [btc-news-feature-plan.md](./features/btc-news-feature-plan.md) | Proposed | Medium | Bitcoin news feed integration planning |

**Planned Features:**

**Bitcoin News Feed:**
- Live Bitcoin news from multiple sources
- Scrollable news list with touch interface
- News caching for offline viewing
- Configurable update intervals
- News category filtering
- Source selection preferences
- Read/unread status tracking

**Implementation Considerations:**
- API selection (CoinDesk, CoinTelegraph, Bitcoin Magazine)
- Memory constraints (ESP32-S3 has limited RAM)
- Network bandwidth (avoid excessive API calls)
- UI layout (news cards vs. list view)
- Touch scrolling performance
- News content truncation
- Update notification system

---

## Quick Links

### Project Files
- [Main README](../README.md) - Project overview and setup
- [CLAUDE.md](../CLAUDE.md) - Development guidelines and agent configuration
- [Makefile](../Makefile) - Build and test commands
- [platformio.ini](../platformio.ini) - PlatformIO configuration

### Source Code
- `src/main.cpp` - Application entry point
- `src/DisplayConfig.h` - LovyanGFX hardware configuration
- `src/screens/ScreenManager.h/cpp` - Screen navigation system
- `src/screens/WiFiScanScreen.h/cpp` - WiFi selection UI
- `src/screens/BTCDashboardScreen.h/cpp` - Bitcoin data display

### Test Code
- `test/native/` - Native unit tests (53 tests)
- `test/mocks/` - Mock implementations
- `test/embedded/` - Hardware integration tests (pending)

### Build Commands
```bash
# Build and Upload
make build          # Build firmware
make upload         # Upload to device
make all            # Build + upload
make flash          # Upload + monitor

# Testing
make test           # Run all tests
make test-native    # Run native tests (desktop)
make test-hardware  # Run hardware tests (device)

# Monitoring
make monitor        # Serial monitor (115200 baud)
make debug-monitor  # Monitor with exception decoder

# Maintenance
make clean          # Clean build files
make devices        # List connected devices
make update         # Update PlatformIO & libraries
```

---

## Document Statistics

### By Category
- **Hardware:** 1 document (1 PDF)
- **Testing:** 3 documents (469 + 339 + ? lines)
- **Guides:** 2 documents
- **Development:** 2 documents
- **Features:** 1 document

### By Type
- **Technical Specifications:** 1 (hardware datasheet)
- **Planning Documents:** 3 (testing plan, development plan, feature plan)
- **Status Reports:** 1 (testing implementation status)
- **Procedural Guides:** 2 (debugging, manual upload)
- **Design Documents:** 2 (UI improvements, news feature)

### By Status
- ✅ **Implemented/Complete:** 4 documents
- ⏳ **In Progress:** 2 documents
- 📋 **Planned/Proposed:** 2 documents
- 📖 **Reference:** 3 documents

---

## Documentation Standards

Following CLAUDE.md guidelines:

1. **Directory Structure:**
   - All docs in `docs/` directory
   - Organized by category (hardware, testing, guides, development, features)
   - Clear, descriptive filenames

2. **Document Format:**
   - Markdown for text documents (.md)
   - PDF for datasheets and specifications
   - Include document metadata (title, date, purpose)
   - Maintain cross-references between documents

3. **Versioning:**
   - Include "Last Updated" dates where applicable
   - Track status (Complete, In Progress, Planned)
   - Note version information for specifications

4. **Maintenance:**
   - Update this index when adding new documents
   - Check cross-references when moving/renaming files
   - Archive outdated documents instead of deleting
   - Keep README.md in sync with INDEX.md

---

## Related Resources

### External Documentation
- [ESP32-S3 Technical Reference](https://www.espressif.com/sites/default/files/documentation/esp32-s3_technical_reference_manual_en.pdf)
- [ST7796 Display Driver Datasheet](https://www.displayfuture.com/Display/datasheet/controller/ST7796s.pdf)
- [FT6X36 Touch Controller Datasheet](https://www.buydisplay.com/download/ic/FT6236-FT6336.pdf)
- [LovyanGFX Documentation](https://github.com/lovyan03/LovyanGFX)
- [mempool.space API Documentation](https://mempool.space/docs/api)

### Community Resources
- [PlatformIO Documentation](https://docs.platformio.org/)
- [Arduino ESP32 Core](https://github.com/espressif/arduino-esp32)
- [ESP32-S3 Forum](https://www.esp32.com/)

---

**Index Version:** 1.0
**Total Documents Indexed:** 11
**Last Review:** 2025-11-28
**Maintained By:** Project Documentation Team
