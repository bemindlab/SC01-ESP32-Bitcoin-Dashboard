# Documentation Index

This directory contains all documentation for the Bitcoin Dashboard project for Panlee SC01 Plus (WT32-SC01 Plus).

## Documentation Structure

All documentation is organized by category in subdirectories:

### 📦 Hardware ([`hardware/`](./hardware/))
Hardware specifications, datasheets, and pinout information.

- **[WT32-SC01_Plus_ESP32-S3_Datasheet.pdf](./hardware/WT32-SC01_Plus_ESP32-S3_Datasheet.pdf)** - Official hardware datasheet (1.05 MB)
  - ESP32-S3-WROVER-N16R2 module specifications
  - 3.5" LCD display (ST7796 driver) specifications
  - Capacitive touch panel (FT6X36) details
  - GPIO pin mappings and electrical characteristics

### 🧪 Testing ([`testing/`](./testing/))
Unit testing, integration testing, and quality assurance documentation.

- **[unit-testing-plan.md](./testing/unit-testing-plan.md)** - Comprehensive unit testing strategy
- **[testing-implementation-status.md](./testing/testing-implementation-status.md)** - Current test implementation status and results
- **[simulator-testing.md](./testing/simulator-testing.md)** - Testing procedures using simulators

### 📘 Guides ([`guides/`](./guides/))
Step-by-step guides and procedures for common tasks.

- **[debugging-guide.md](./guides/debugging-guide.md)** - Debugging procedures and troubleshooting tips
- **[manual-upload-guide.md](./guides/manual-upload-guide.md)** - Manual firmware upload instructions

### 🚀 Development ([`development/`](./development/))
Development planning, roadmaps, and improvement proposals.

- **[development-plan.md](./development/development-plan.md)** - Project development roadmap and planning
- **[ui-improvements.md](./development/ui-improvements.md)** - UI enhancement proposals and design changes

### ✨ Features ([`features/`](./features/))
Feature specifications and planning documents.

- **[btc-news-feature-plan.md](./features/btc-news-feature-plan.md)** - Bitcoin news feed feature planning

## Complete Document Index

### Hardware Documentation

| Document | Category | Description | Size |
|----------|----------|-------------|------|
| [WT32-SC01_Plus_ESP32-S3_Datasheet.pdf](./hardware/WT32-SC01_Plus_ESP32-S3_Datasheet.pdf) | Hardware | Official SC01 Plus hardware specifications and pinout | 1.05 MB |

**Contents:** ESP32-S3-WROVER-N16R2 specs, display (ST7796) configuration, touch controller (FT6X36) I2C setup, GPIO mappings, electrical characteristics, upload procedures.

### Testing Documentation

| Document | Category | Description | Last Updated |
|----------|----------|-------------|--------------|
| [unit-testing-plan.md](./testing/unit-testing-plan.md) | Testing | Comprehensive testing strategy with native and hardware tests | 2025-11-28 |
| [testing-implementation-status.md](./testing/testing-implementation-status.md) | Testing | Current test results and coverage metrics (53 passing tests) | 2025-11-28 |
| [simulator-testing.md](./testing/simulator-testing.md) | Testing | Simulator testing procedures and setup | - |

**Testing Summary:**
- **Phase 1 Complete:** 53 native unit tests passing (data parsing, formatting, screen logic)
- **Mock Implementations:** LGFX display, HTTPClient, WiFi
- **Test Execution:** `make test-native` (~3.2 seconds)
- **Coverage:** ~90% of testable business logic

### Guide Documentation

| Document | Category | Description | Purpose |
|----------|----------|-------------|---------|
| [debugging-guide.md](./guides/debugging-guide.md) | Guides | Debugging procedures and troubleshooting | Help diagnose issues with serial monitor and exception decoder |
| [manual-upload-guide.md](./guides/manual-upload-guide.md) | Guides | Manual firmware upload instructions | Step-by-step bootloader mode and upload procedure |

**Key Topics:**
- Serial monitor usage (115200 baud)
- Exception decoding for crash analysis
- Manual bootloader entry (BOOT + RST buttons)
- Common upload errors and solutions

### Development Documentation

| Document | Category | Description | Status |
|----------|----------|-------------|--------|
| [development-plan.md](./development/development-plan.md) | Development | Project roadmap and development phases | Planning |
| [ui-improvements.md](./development/ui-improvements.md) | Development | UI/UX enhancement proposals and design iterations | Implemented |

**Development Phases:**
1. **Phase 1 (Complete):** Core dashboard with WiFi selection and Bitcoin data display
2. **Phase 2 (In Progress):** Unit testing infrastructure with 53 passing tests
3. **Phase 3 (Planned):** Hardware integration tests and CI/CD
4. **Future:** Additional features (news feed, price alerts, settings)

### Feature Documentation

| Document | Category | Description | Status |
|----------|----------|-------------|--------|
| [btc-news-feature-plan.md](./features/btc-news-feature-plan.md) | Features | Bitcoin news feed integration planning | Proposed |

**Planned Features:**
- Live Bitcoin news from multiple sources
- Scrollable news list with touch support
- News caching for offline viewing
- Configurable update intervals

## Quick Reference

### Hardware Specifications (SC01 Plus)
- **MCU:** ESP32-S3-WROVER-N16R2
- **Flash:** 16MB SPI Flash
- **PSRAM:** 2MB PSRAM
- **Display:** 3.5" IPS TFT, 320x480 resolution, ST7796 driver (8-bit parallel)
- **Touch:** FT6X36 capacitive touch controller (I2C, SDA=GPIO6, SCL=GPIO5, INT=GPIO7)
- **Connectivity:** Wi-Fi 802.11 b/g/n (2.4GHz only), Bluetooth 5.0

### Testing Status
- ✅ **Native Unit Tests:** 53 passing tests (Phase 1 complete)
- ⏳ **Hardware Integration Tests:** Pending implementation (Phase 2)
- **Coverage:** ~90% of testable business logic
- **Run Tests:** `make test-native`

### Related Documentation
- **Project Instructions:** [../CLAUDE.md](../CLAUDE.md) - Development guidelines and agent configuration
- **Main README:** [../README.md](../README.md) - Project overview and setup instructions
- **Makefile:** [../Makefile](../Makefile) - Build and test command shortcuts

## Documentation Rules

Following CLAUDE.md guidelines:

1. **All documentation belongs in `docs/` directory** - Never create docs in project root (except README.md, LICENSE, CLAUDE.md)
2. **Organize by category** - Use subdirectories (hardware, testing, guides, development, features)
3. **Clear filenames** - Use descriptive names (e.g., `unit-testing-plan.md`, not `plan.md`)
4. **Include headers** - Every document should have a title and purpose
5. **Maintain cross-references** - Update links when moving or renaming files
6. **Version information** - Include date and status where applicable

## Contributing Documentation

When adding new documentation:

1. Place it in the appropriate category directory
2. Use a clear, descriptive filename
3. Add a reference to this README.md index
4. Include document metadata (title, date, purpose)
5. Maintain internal cross-references

For documentation questions or suggestions, see [CLAUDE.md](../CLAUDE.md).
