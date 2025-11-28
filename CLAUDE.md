# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Specialized Agents

The project includes specialized agents in `.claude/agents/` for different aspects of development:

### Available Agents

1. **esp32-hardware-specialist** - ESP32-S3 hardware, display drivers, GPIO configuration
2. **lvgl-ui-designer** - LVGL v8.3 UI design, layouts, dark theme styling
3. **bitcoin-api-specialist** - mempool.space API integration, Bitcoin data handling
4. **embedded-debugger** - Serial debugging, crash analysis, troubleshooting
5. **platformio-build-specialist** - Build configuration, dependencies, compilation

### Parallel Agent Tasks

Use multiple agents in parallel for complex tasks:

**Example 1: Adding a new Bitcoin metric to display**
- `bitcoin-api-specialist` - Integrate new API endpoint
- `lvgl-ui-designer` - Design UI card for new metric
- Run in parallel: API implementation + UI design

**Example 2: Display rendering issues**
- `esp32-hardware-specialist` - Check hardware configuration
- `embedded-debugger` - Analyze serial output for errors
- `lvgl-ui-designer` - Review LVGL rendering code
- Run in parallel: Hardware check + debug analysis

**Example 3: Build and optimization**
- `platformio-build-specialist` - Optimize build flags
- `esp32-hardware-specialist` - Review memory usage
- Run in parallel: Build optimization + memory analysis

**Example 4: Complete feature addition**
- `bitcoin-api-specialist` - Add API integration
- `lvgl-ui-designer` - Create UI components
- `platformio-build-specialist` - Verify dependencies
- Sequential: API → UI → Build verification

## Documentation Rules

- **Main documents directory:** `/Users/lps/Solutions/bemind-embeded/btc-dashboard/docs`
- **Do NOT create document files in root directory** - all documentation (guides, specs, diagrams, etc.) must go in the docs/ directory
- Exception: README.md, LICENSE, CLAUDE.md remain in root

### Documentation Workflow Hooks

Claude Code hooks are configured in `.claude/hooks/` to enforce documentation rules:
- `create-document.sh` - Validates new docs go to `docs/` directory
- `update-document.sh` - Reminds about cross-reference checks
- `delete-document.sh` - Warns about broken references and suggests archiving

**When creating new documentation:**
1. Always place files in `docs/` directory
2. Use clear, descriptive filenames (e.g., `hardware-setup.md`, `api-integration-guide.md`)
3. Include a header with document title and purpose
4. Update docs/README.md (if it exists) with a link to the new document

**When updating documentation:**
1. Maintain existing formatting and structure
2. Add date/version information if the document tracks changes
3. Ensure cross-references to other docs remain valid

**When deleting documentation:**
1. Check for references to the document in other files (README.md, other docs)
2. Remove or update any broken links
3. Consider archiving instead of deleting if the information may be useful later

## Project Overview

Bitcoin Dashboard for Panlee SC01 Plus - A real-time Bitcoin data display running on ESP32-S3 with 3.5" IPS touch screen. Fetches live data from mempool.space API (no API key required) and displays:
- BTC price (USD/EUR) - updates every 30 seconds
- Latest block info (height, TX count)
- Mempool statistics (pending TXs, size)
- Fee rates (Fast/Medium/Slow sat/vB)

## Development Commands

### Build and Upload
```bash
# Build the project
pio run

# Upload to device
pio run --target upload

# Monitor serial output (115200 baud)
pio device monitor

# Build + Upload + Monitor in one command
pio run --target upload && pio device monitor
```

### Cleaning
```bash
# Clean build artifacts
pio run --target clean
```

### Debugging
```bash
# Monitor with filtered output (useful for debugging specific components)
pio device monitor --filter esp32_exception_decoder

# Monitor with custom baud rate
pio device monitor -b 115200

# List connected devices
pio device list
```

## Hardware Configuration

**Target Board:** Panlee SC01 Plus (ESP32-S3 with 16MB flash, OPI PSRAM)

**Display:** ST7796 driver, 320x480 resolution (used in landscape 480x320)
- TFT pins are defined in platformio.ini build_flags
- Uses TFT_eSPI library with custom configuration
- Display rotation set to 1 (landscape) in main.cpp:303

**Touch:** FT6X36 capacitive touch controller via I2C
- SDA: GPIO 18
- SCL: GPIO 19

## Architecture

### Core Components

**Main Loop Flow (main.cpp:359-389):**
- LVGL task handler updates UI continuously
- Three independent timers control API fetches:
  - Price: 30s interval
  - Blocks: 60s interval
  - Mempool + Fees: 30s interval
- Each successful fetch triggers UI update

**Data Flow:**
1. API functions (fetchPrice, fetchBlocks, fetchMempool, fetchFees) make HTTP requests
2. JSON responses parsed with ArduinoJson into btcData struct
3. updateUI() reads btcData and updates LVGL labels
4. LVGL renders to TFT via my_disp_flush callback

**WiFi Configuration:**
- Credentials hardcoded in main.cpp:10-11
- Must be updated before first build
- ESP32-S3 only supports 2.4GHz networks
- Connection attempted at startup with 30-attempt timeout

### UI Structure (LVGL)

Four card-based layout with dark theme (main.cpp:178-267):
- Price Card (top-left): BTC/USD price in green
- Block Card (top-right): Latest block height + TX count in cyan
- Mempool Card (bottom-left): Pending TXs + size in purple
- Fees Card (bottom-right): Fast/Medium/Slow fee rates in red
- Status bar at bottom shows connection status

Colors use hex theme:
- Background: 0x1a1a2e
- Cards: 0x16213e with colored borders
- Bitcoin orange accent: 0xf7931a

### Display System

**LVGL Configuration (lv_conf.h):**
- 16-bit color depth with byte swap enabled
- 64KB memory allocation
- Custom tick using Arduino millis()
- Montserrat fonts 12-32pt enabled

**Buffer:** Single buffer of 480 * 40 pixels (main.cpp:31)
- Flush callback writes directly to TFT_eSPI
- No double buffering (memory constraint)

## API Integration

**mempool.space Endpoints:**
- `/api/v1/prices` - BTC price in multiple currencies
- `/api/v1/blocks` - Recent blocks array (uses first/latest)
- `/api/mempool` - Current mempool statistics
- `/api/v1/fees/recommended` - Recommended fee rates

All endpoints use 10-second HTTP timeout. Failed requests return false and don't update UI (stale data remains visible).

**JSON Parsing:**
- Static documents for simple responses (prices, fees)
- Dynamic document (4KB) for blocks array
- No error displayed to user on parse failure (logged to Serial only)

## Update Intervals

Configurable constants in main.cpp:19-22:
```cpp
PRICE_INTERVAL = 30000      // Price updates
BLOCK_INTERVAL = 60000      // Block info updates
MEMPOOL_INTERVAL = 30000    // Mempool + fees updates
```

Note: mempool.space may have rate limits for frequent requests.

## Dependencies

Managed via PlatformIO (platformio.ini:35-40):
- lvgl/lvgl@^8.3.0 - UI framework
- bodmer/TFT_eSPI@^2.5.0 - Display driver (configured via build flags)
- bblanchon/ArduinoJson@^6.21.0 - JSON parsing
- FT6X36 touch driver from GitHub (strange-v/FT6X36.git)

TFT_eSPI is configured entirely through build_flags - no User_Setup.h file needed.

## Code Modification Workflow

**Before making changes:**
1. Always test on actual hardware - emulation not available for this platform
2. WiFi credentials in main.cpp:10-11 must be updated for your network
3. Serial monitor is essential for debugging - all errors logged there

**When modifying UI:**
- UI elements created in setupUI() (main.cpp:178-267)
- LVGL objects are global pointers (labelPrice, labelBlock, etc.)
- Color scheme should maintain dark theme consistency
- Test at 480x320 landscape orientation

**When modifying API calls:**
- HTTP timeout is 10 seconds (configurable in each fetch function)
- Failed API calls don't crash - stale data remains on display
- ArduinoJson document sizes are optimized for current responses
- Adding new endpoints may require larger DynamicJsonDocument

**When changing timers:**
- millis() rollover (after ~49 days) is not handled
- Reducing intervals below 10s may trigger rate limiting
- All three timers are independent - can run at different frequencies

## Troubleshooting

**Display Issues:**
- All TFT pins defined in platformio.ini build_flags, not in code
- If display doesn't work, verify GPIO pins match hardware
- Try different rotation values (0-3) in main.cpp:303

**WiFi Connection:**
- Check Serial output for connection status
- Verify 2.4GHz network (ESP32-S3 doesn't support 5GHz)
- WiFi connection blocks setup() for up to 15 seconds

**Data Not Updating:**
- Serial monitor shows HTTP request errors
- Verify internet connectivity
- mempool.space may throttle requests
- API endpoints are hardcoded - no failover
