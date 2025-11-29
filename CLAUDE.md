# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Multi-Agent Development System

The project includes a **tmux-based multi-agent development system** that enables parallel workflows with automatic agent communication.

### Quick Start

```bash
# Start multi-agent session
make agents

# View status
make agents-status

# Attach to running session
make agents-attach

# Stop session
make agents-stop
```

### Available Agents

1. **esp32-hardware-specialist** - ESP32-S3 hardware, display drivers, GPIO configuration, memory management
2. **lvgl-ui-designer** - LVGL v8.3 UI design, layouts, dark theme styling, responsive design
3. **bitcoin-api-specialist** - mempool.space API integration, Bitcoin data handling, real-time updates
4. **embedded-debugger** - Serial debugging, crash analysis, troubleshooting, performance profiling
5. **platformio-build-specialist** - Build configuration, dependencies, compilation, optimization

### Tmux Session Layout

When you run `make agents`, a tmux session is created with 7 windows:

| Window | Name | Left Pane | Right Pane |
|--------|------|-----------|------------|
| 0 | coordinator | Main control panel | Broadcast monitor |
| 1 | esp32-hw | ESP32 Hardware Agent | Build output |
| 2 | ui-designer | UI/UX Designer Agent | UI preview/screenshots |
| 3 | btc-api | Bitcoin API Agent | API test output |
| 4 | debugger | Embedded Debugger Agent | Serial monitor |
| 5 | build | Build Specialist Agent | Build commands |
| 6 | monitor | Global communication monitor | All agent logs |

**Navigation:**
- `Ctrl-b` then `0-6`: Switch between windows
- `Ctrl-b d`: Detach from session (keeps running in background)
- `Ctrl-b [`: Enter scroll mode, `q` to exit

### Agent Communication System

Each agent has access to inter-agent communication functions:

```bash
# Log to agent's own log
agent_log "Starting API integration task"

# Broadcast to all agents
agent_broadcast "API endpoint updated, please retest UI"

# Send message to specific agent
agent_send ui-designer "New Bitcoin data structure available: BTCData.h"

# Read recent messages
agent_read

# Monitor agent's log in real-time
agent_monitor
```

**Communication logs location:** `.tmp/agent-comm/`

### Multi-Agent Workflows

#### Workflow 1: Adding a New Bitcoin Metric

**Parallel execution:**
1. **bitcoin-api-specialist**: Integrate new API endpoint
   ```bash
   agent_log "Fetching new metric from mempool.space"
   # Implement fetchNewMetric() in MainScreen.cpp
   agent_send ui-designer "New metric ready: hashrate data"
   ```

2. **lvgl-ui-designer**: Design UI card for new metric
   ```bash
   agent_log "Designing hashrate card layout"
   # Add new card to drawContent()
   agent_broadcast "New card added at row 5, needs testing"
   ```

3. **platformio-build-specialist**: Verify memory impact
   ```bash
   agent_log "Checking memory usage after new metric"
   # Run build and analyze RAM/Flash
   agent_broadcast "Memory OK: +2KB flash, no RAM impact"
   ```

**Completion:** All agents report via `agent_broadcast` when done

#### Workflow 2: Display Rendering Optimization

**Sequential with handoffs:**
1. **embedded-debugger**: Profile rendering performance
   ```bash
   agent_log "Analyzing scroll FPS via serial monitor"
   # Capture frame timing data
   agent_send esp32-hw "FPS drops below 60 during scroll"
   ```

2. **esp32-hardware-specialist**: Review hardware acceleration
   ```bash
   agent_log "Checking DMA and SPI configuration"
   # Optimize LovyanGFX settings
   agent_send ui-designer "Enable startWrite/endWrite batching"
   ```

3. **lvgl-ui-designer**: Implement optimizations
   ```bash
   agent_log "Simplifying card rendering (removing shadows)"
   # Update drawCard() function
   agent_send build "Ready for performance test build"
   ```

4. **platformio-build-specialist**: Build and deploy
   ```bash
   agent_log "Building optimized firmware"
   # make flash
   agent_broadcast "New firmware deployed, test FPS"
   ```

#### Workflow 3: API Integration + UI Update

**Coordinator-driven workflow:**
1. **Coordinator** broadcasts task breakdown:
   ```bash
   agent_broadcast "Task: Add DCA recommendation card"
   agent_send bitcoin-api "Implement fetchDCARecommendation() in GeminiClient"
   agent_send ui-designer "Design DCA card with BUY/SELL/WAIT states"
   agent_send build "Prepare test environment after API+UI ready"
   ```

2. **bitcoin-api-specialist** completes API work:
   ```bash
   agent_log "fetchDCARecommendation() implemented in GeminiClient.cpp"
   agent_log "Added dcaRecommendation field to BTCData.h"
   agent_send ui-designer "API ready, test with: btcData.dcaRecommendation"
   agent_send build "New dependencies: ArduinoJson document size +512 bytes"
   ```

3. **ui-designer** implements UI:
   ```bash
   agent_log "DCA card added at row 4, color-coded (green/red/yellow)"
   agent_log "Card reads btcData.dcaRecommendation for display"
   agent_broadcast "UI complete, ready for integration test"
   ```

4. **build** creates test build:
   ```bash
   agent_log "Building with DCA feature enabled"
   # make flash
   agent_send debugger "Monitor for DCA API calls and card rendering"
   ```

5. **debugger** validates:
   ```bash
   agent_log "Monitoring serial output for DCA API calls"
   # Check logs for successful Gemini API response
   agent_broadcast "✓ DCA feature working: API calls successful, card displays correctly"
   ```

### Agent Collaboration Best Practices

1. **Always log your work**: Use `agent_log` to track progress
2. **Communicate dependencies**: Use `agent_send` to notify dependent agents
3. **Broadcast milestones**: Use `agent_broadcast` for major completions
4. **Monitor other agents**: Check `.tmp/agent-comm/*.log` files
5. **Use the coordinator**: Window 0 shows all broadcasts for oversight

### Parallel vs Sequential

**Use Parallel when:**
- Tasks are independent (API + UI can be developed separately)
- No shared file modifications (different source files)
- Testing can be done independently

**Use Sequential when:**
- Output of one agent is input to another (API → UI → Build → Test)
- Shared file modifications (avoid merge conflicts)
- Dependencies must be resolved in order

### Example: Complete Feature Addition

**Task:** Add AI-powered trading signal with 15m-1h timeframe

```bash
# Window 0: Coordinator
agent_broadcast "New feature: Trading Signal (15m-1h timeframe)"
agent_send bitcoin-api "Create fetchTradingSignal() in GeminiClient"
agent_send ui-designer "Standby for BTCData field: tradingSignal"

# Window 3: bitcoin-api-specialist
agent_log "Starting Gemini API integration for trading signal"
# Edit GeminiClient.cpp, GeminiClient.h, BTCData.h
agent_send ui-designer "Field ready: btcData.tradingSignal (BUY/SELL/HOLD)"
agent_send build "New Gemini endpoint: /trading-signal"

# Window 2: ui-designer
agent_log "Designing trading signal card (green/red/gray states)"
# Edit MainScreen.cpp drawContent()
agent_broadcast "Trading card added at row 4, right side"
agent_send build "Ready for compilation"

# Window 5: build-specialist
agent_log "Compiling with trading signal feature"
# make build
agent_log "✓ Build successful: RAM 16.3%, Flash 16.5%"
agent_send debugger "Deploy and test"

# Window 4: debugger
agent_log "Uploading firmware and monitoring serial"
# make upload
# make monitor
agent_log "Trading signal API called successfully"
agent_log "Card displays: HOLD (gray)"
agent_broadcast "✓ Feature complete and tested"
```

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

### Using Make (Recommended)

The project includes a Makefile for convenient command shortcuts:

```bash
# Build and Upload
make build          # Build the project
make upload         # Upload firmware to device
make all            # Build and upload
make flash          # Upload and monitor

# Monitoring
make monitor        # Open serial monitor (115200 baud)
make debug-monitor  # Monitor with exception decoder

# Maintenance
make clean          # Clean build files
make devices        # List connected devices
make update         # Update PlatformIO and libraries

# Development
make erase          # Erase flash memory
make check          # Check configuration
make libs           # Show library dependencies
make info           # Show platform information
```

### Using PlatformIO CLI Directly

```bash
# Build and Upload
pio run                    # Build the project
pio run --target upload    # Upload to device
pio device monitor         # Monitor serial output (115200 baud)
pio run --target upload && pio device monitor  # Build + Upload + Monitor

# Cleaning
pio run --target clean     # Clean build artifacts

# Debugging
pio device monitor --filter esp32_exception_decoder  # Monitor with filtered output
pio device monitor -b 115200  # Monitor with custom baud rate
pio device list            # List connected devices
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

### External Dependencies

Managed via PlatformIO (platformio.ini lib_deps):
- lovyan03/LovyanGFX@^1.1.0 - Modern display driver library (replaces TFT_eSPI)
- bblanchon/ArduinoJson@^6.21.0 - JSON parsing
- FT6X36 touch driver from GitHub (strange-v/FT6X36.git)

LovyanGFX is configured via build_flags in platformio.ini - no User_Setup.h file needed.

### Internal Libraries

Project-specific libraries are stored in the `libs/` directory. Currently empty - will be populated as the project grows with custom components like:
- Bitcoin API client wrappers
- Display management utilities
- Custom UI components

See the [Internal Libraries Directory](#internal-libraries-directory-libs) section for details on creating custom libraries.

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

## Internal Libraries Directory (libs/)

**Location:** `/Users/lps/Solutions/bemind-embeded/btc-dashboard/libs`

The `libs/` directory is PlatformIO's standard location for project-specific internal libraries. Currently empty - custom libraries will be added here for:
- Custom display drivers or wrappers
- Bitcoin protocol utilities
- Hardware abstraction layers
- Reusable UI components
- Custom data structures

### Library Structure

Each library should follow PlatformIO's library structure:
```
libs/
└── MyLibrary/
    ├── library.json          # Library metadata (optional)
    ├── src/
    │   ├── MyLibrary.h      # Header file
    │   └── MyLibrary.cpp    # Implementation
    └── examples/            # Example sketches (optional)
```

### When to Create Internal Libraries

Create a library in `libs/` when:
- Code is reusable across multiple files/screens
- Functionality is self-contained and testable
- You want to separate concerns (e.g., API client, display manager)
- Code can be versioned and maintained independently

### External vs Internal Libraries

- **External libraries** (defined in `platformio.ini` `lib_deps`):
  - LovyanGFX - Display driver
  - ArduinoJson - JSON parsing
  - FT6X36 - Touch controller

- **Internal libraries** (in `libs/` directory):
  - Project-specific code
  - Custom hardware interfaces
  - Application-specific utilities

PlatformIO automatically includes libraries from `libs/` - no configuration needed.

### Best Practices

- Keep library names descriptive (e.g., `BitcoinAPI`, `DisplayManager`)
- Include header guards in all `.h` files
- Document public APIs with comments
- Add examples for complex libraries
- Consider adding `library.json` for metadata

## Scripts Directory

**Location:** `/Users/lps/Solutions/bemind-embeded/btc-dashboard/scripts`

The scripts directory is reserved for build automation, deployment, and maintenance scripts. Currently empty - scripts will be added as needed for:
- Automated firmware builds
- Release packaging
- Hardware testing utilities
- Configuration management
- Batch operations

When adding scripts:
- Use clear, descriptive filenames (e.g., `build-release.sh`, `test-hardware.sh`)
- Include usage comments at the top of each script
- Make scripts executable: `chmod +x scripts/script-name.sh`
- Document in CLAUDE.md and README.md

## Releases Directory

**Location:** `/Users/lps/Solutions/bemind-embeded/btc-dashboard/releases`

The releases directory stores pre-compiled firmware binaries for easy deployment. Currently empty - releases will be created for:
- Stable version releases
- Beta/testing builds
- Hardware-specific configurations

Release naming convention:
- Format: `btc-dashboard-v{version}-{variant}.bin`
- Example: `btc-dashboard-v1.0.0-sc01plus.bin`
- Include release notes in `releases/CHANGELOG.md`

To create a release:
1. Build firmware: `make build`
2. Copy binary from `.pio/build/sc01_plus/firmware.bin`
3. Rename with version: `btc-dashboard-v{version}.bin`
4. Place in `releases/` directory
5. Update `releases/CHANGELOG.md` with changes

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
