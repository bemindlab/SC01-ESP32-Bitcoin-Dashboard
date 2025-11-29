# Bitcoin Dashboard for Panlee SC01 Plus

[![PlatformIO CI](https://img.shields.io/badge/PlatformIO-passing-brightgreen)](https://platformio.org/)
[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)

Real-time Bitcoin dashboard running on ESP32-S3 with 3.5" IPS touch display. Features smooth 120 FPS vertical scrolling, AI-powered trading signals, and comprehensive Bitcoin metrics from mempool.space API.

![Bitcoin Dashboard](https://img.shields.io/badge/BTC-Dashboard-orange)

## Features

### Main Dashboard (Unified Screen)
- 💰 **BTC Price (USD)** - Real-time price updates every 30 seconds
- 💶 **BTC Price (EUR)** - Euro pricing
- 🧱 **Block Height** - Latest block number
- 📊 **Mempool Count** - Pending transactions
- ⛽ **Fee Rates** - Fast/Medium/Slow sat/vB
- 📶 **WiFi Signal** - Network RSSI strength
- 🤖 **DCA Signal (AI)** - Gemini AI recommendation (BUY/SELL/WAIT)
- 📈 **Trading Signal (AI)** - 15m-1h timeframe analysis (BUY/SELL/HOLD)

### Display Features
- 📜 **Smooth Vertical Scrolling** - 120 FPS with 8ms frame time
- 🎨 **Bitcoin Orange Theme** - Orange header (#F7931A) with white text
- 🃏 **8 Information Cards** - 4 rows, 2 columns layout
- ⚡ **Optimized Rendering** - Batch operations with GPU clipping
- 🔄 **Screen Rotation** - Tap top-right corner (0°/90°/180°/270°)
- 📏 **412px Scroll Range** - Seamless card navigation

### AI-Powered Analysis
- 🧠 **Gemini AI Integration** - Trading signals and DCA recommendations
- 🔄 **5-Minute Updates** - Balanced between freshness and API costs
- 🎯 **Color-Coded Cards** - Visual feedback (Green=BUY, Red=SELL, Yellow/Gray=HOLD/WAIT)

### System Features
- ⚙️ **NVRAM Configuration** - Persistent WiFi credentials and API keys
- 🔄 **Auto-Reconnect** - Automatic WiFi recovery
- 📸 **Screenshot Capture** - Serial debugging with visual output
- 🔧 **Serial Commands** - Full device control via UART (115200 baud)
- 💾 **SD Card Logging** - Optional data logging to microSD
- 🛡️ **Crash Handler** - Automatic exception logging and recovery
- 📊 **Memory Monitoring** - RAM/Flash usage tracking

## Hardware Requirements

- **Board:** Panlee SC01 Plus (WT32-SC01 Plus)
  - **MCU:** ESP32-S3-WROVER-N16R2
  - **Flash:** 16MB SPI Flash
  - **PSRAM:** 2MB PSRAM (OPI)
  - **Display:** 3.5" IPS TFT LCD, 320x480 resolution, ST7796 driver
  - **Touch:** FT6X36 capacitive touch controller (I2C)
  - **Storage:** MicroSD card slot (optional, for logging)
  - **Connectivity:** Wi-Fi 802.11 b/g/n (2.4GHz), Bluetooth 5.0 LE
  - **USB:** USB-C port for programming and power
  - **Dimensions:** Development board with integrated display

> **📄 Full Hardware Specifications:** See [docs/WT32-SC01_Plus_ESP32-S3_Datasheet.pdf](docs/WT32-SC01_Plus_ESP32-S3_Datasheet.pdf) for complete technical details

## Pin Configuration

### Display Interface (Parallel 8-bit)

| Function | GPIO | Description |
|----------|------|-------------|
| **Data Bus** | | |
| D0 | 9 | Data bit 0 |
| D1 | 46 | Data bit 1 |
| D2 | 3 | Data bit 2 |
| D3 | 8 | Data bit 3 |
| D4 | 18 | Data bit 4 |
| D5 | 17 | Data bit 5 |
| D6 | 16 | Data bit 6 |
| D7 | 15 | Data bit 7 |
| **Control Signals** | | |
| WR (Write) | 47 | Write strobe |
| RS (Register Select) | 0 | Command/Data select |
| RST (Reset) | 4 | Display reset |
| BL (Backlight) | 45 | PWM backlight control |

### Touch Interface (I2C)

| Function | GPIO | Description |
|----------|------|-------------|
| SDA | 18 | I2C data line |
| SCL | 19 | I2C clock line |
| INT | 7 | Touch interrupt (FT6X36) |

### SD Card Interface (SPI)

| Function | GPIO | Description |
|----------|------|-------------|
| CS (Chip Select) | 41 | SD card select |
| MOSI (Data Out) | 40 | SPI data output |
| MISO (Data In) | 38 | SPI data input |
| CLK (Clock) | 39 | SPI clock |

### Power and USB

| Function | GPIO | Description |
|----------|------|-------------|
| USB D+ | 20 | USB-Serial/JTAG |
| USB D- | 19 | USB-Serial/JTAG |
| 5V | - | USB-C power input |
| 3.3V | - | Regulated output for peripherals |
| GND | - | Ground reference |

**Note:** All GPIO assignments are hardware-defined in `src/DisplayConfig.h` and `src/utils/SDLogger.h`. Do not modify unless using custom hardware.

### Available GPIOs (Unused)

The following GPIOs are **available for custom expansion** (sensors, actuators, additional peripherals):

| GPIO | Notes | Recommended Use |
|------|-------|-----------------|
| **1** | Available | General I/O |
| **2** | Available | General I/O, ADC1_CH1 |
| **5** | Available | General I/O |
| **6** | Available | General I/O |
| **10** | Available | General I/O |
| **11** | Available | General I/O |
| **12** | Available | General I/O, ADC2_CH1 |
| **13** | Available | General I/O, ADC2_CH2 |
| **14** | Available | General I/O, ADC2_CH3 |
| **21** | Available | General I/O |
| **42** | Available | General I/O (MTMS) |
| **43** | Available | General I/O (MTCK) |
| **44** | Available | General I/O (MTDO) |
| **48** | Available | General I/O (RGB LED on some boards) |

**Total Available:** 14 GPIOs

**Usage Examples:**
- **Temperature Sensor:** GPIO 2 (DHT22, DS18B20)
- **Relay Control:** GPIO 5, 6 (for switching devices)
- **Additional I2C:** GPIO 10 (SDA), GPIO 11 (SCL) - for extra sensors
- **Additional SPI:** GPIO 12 (MISO), GPIO 13 (MOSI), GPIO 14 (CLK), GPIO 21 (CS)
- **Status LEDs:** GPIO 42, 43, 44
- **RGB LED:** GPIO 48 (if present on board)

**Reserved/Unavailable GPIOs:**
- **26-37:** Internal Flash/PSRAM (not exposed)
- **0, 3, 45, 46:** Strapping pins (used by display)
- **19, 20:** USB D-/D+ (Serial/JTAG)

**Caution:**
- Some GPIOs have bootstrap/strapping requirements - consult ESP32-S3 datasheet
- GPIO 0 is a strapping pin (already used by display RS)
- ADC2 (GPIO 12-14) cannot be used when WiFi is active

## Getting Started

### Prerequisites

- [PlatformIO](https://platformio.org/) installed (VS Code extension or CLI)
- WiFi network (2.4GHz only - ESP32-S3 doesn't support 5GHz)
- Optional: Gemini API key for AI features ([Get Key](https://makersuite.google.com/app/apikey))

### Installation

1. **Clone the repository**
   ```bash
   git clone git@github.com:bemindlab/SC01-ESP32-Bitcoin-Dashboard.git
   cd SC01-ESP32-Bitcoin-Dashboard
   ```

2. **Build the project**
   ```bash
   # Using Make (recommended)
   make build

   # Or using PlatformIO directly
   pio run
   ```

3. **Upload to device**
   ```bash
   # Using Make
   make flash

   # Or using PlatformIO directly
   pio run --target upload && pio device monitor
   ```

4. **Configure WiFi (via serial monitor at 115200 baud)**
   ```
   SET_WIFI=YourNetworkSSID,YourNetworkPassword
   STATUS  # Verify configuration
   ```

5. **Optional: Configure Gemini AI for trading signals**
   ```
   SET_GEMINI_KEY=AIzaSy...
   STATUS  # Verify API key stored
   ```

For a complete list of available commands, run `make help`.

## Configuration

### WiFi Setup

The device stores credentials in NVRAM (non-volatile memory):

```bash
# Connect to serial monitor (115200 baud)
SET_WIFI=YourNetworkSSID,YourNetworkPassword
STATUS  # Verify configuration saved
```

### Gemini API Key (Optional - for AI features)

```bash
SET_GEMINI_KEY=AIzaSy...
STATUS  # Verify key stored
```

Get your Gemini API key: https://makersuite.google.com/app/apikey

### Update Intervals

Customize data refresh rates:

```bash
SET_PRICE_INTERVAL=30000    # Price updates (milliseconds)
SET_BLOCK_INTERVAL=60000    # Block updates
SET_MEMPOOL_INTERVAL=30000  # Mempool updates
SET_AI_INTERVAL=300000      # AI signal updates (5 minutes)
STATUS                      # Verify changes
```

### Serial Commands Reference

| Command | Description |
|---------|-------------|
| `SET_WIFI=ssid,password` | Configure WiFi credentials |
| `SET_GEMINI_KEY=key` | Set Gemini API key |
| `SET_PRICE_INTERVAL=ms` | Price update interval |
| `SET_BLOCK_INTERVAL=ms` | Block update interval |
| `SET_MEMPOOL_INTERVAL=ms` | Mempool update interval |
| `STATUS` | Show current configuration |
| `SCREENSHOT` | Capture and send screen via serial |
| `CHECK_SD_CARD` | SD card status |
| `LOG_ENABLE` | Enable SD logging |
| `LOG_DISABLE` | Disable SD logging |
| `FORMAT_SD_CARD` | Format SD card (WARNING: deletes data!) |

See `docs/guides/serial-commands-reference.md` for complete command documentation.

## Usage

### Screen Interaction

- **Scroll:** Swipe up/down to navigate through 8 cards (412px scroll range)
- **Rotate:** Tap top-right corner for 90° rotation (0°/90°/180°/270°)
- **Data Updates:** Automatic every 30-60 seconds
- **AI Signals:** Updated every 5 minutes (configurable)

### Card Layout (4 Rows × 2 Columns)

```
┌─────────────────┬─────────────────┐
│ BTC Price (USD) │ BTC Price (EUR) │  Row 1
├─────────────────┼─────────────────┤
│ Block Height    │ Mempool Count   │  Row 2
├─────────────────┼─────────────────┤
│ Fast Fee        │ WiFi Signal     │  Row 3
├─────────────────┼─────────────────┤
│ DCA Signal (AI) │ Trading (AI)    │  Row 4
└─────────────────┴─────────────────┘
```

**Scroll to see all 8 cards** - Smooth 120 FPS vertical scrolling

### AI Signal Colors

**DCA Recommendation:**
- 🟢 Green = BUY
- 🔴 Red = SELL
- 🟡 Yellow = WAIT

**Trading Signal (15m-1h):**
- 🟢 Green = BUY
- 🔴 Red = SELL
- ⚪ Gray = HOLD

## Multi-Agent Development System

The project includes a **tmux-based multi-agent development system** for parallel workflows:

```bash
# Start multi-agent session
make agents

# Navigate between agents
Ctrl-b 0  # Coordinator
Ctrl-b 1  # ESP32 Hardware Specialist
Ctrl-b 2  # UI/UX Designer
Ctrl-b 3  # Bitcoin API Specialist
Ctrl-b 4  # Embedded Debugger
Ctrl-b 5  # Build Specialist
Ctrl-b 6  # Global Monitor

# Detach (session keeps running)
Ctrl-b d

# Reattach
make agents-attach

# Stop session
make agents-stop
```

### Agent Communication

Each agent can communicate with others:

```bash
agent_log "Starting task X"              # Log to own log
agent_broadcast "Milestone completed"    # Broadcast to all
agent_send target_agent "Data ready"     # Send to specific agent
agent_read                               # Read recent messages
```

See `CLAUDE.md` and `docs/guides/multi-agent-development.md` for complete workflows.

## Development Commands

### Using Make (Recommended)

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
make clean          # Clean build artifacts
make devices        # List connected devices
make update         # Update PlatformIO and libraries

# Development
make erase          # Erase flash memory
make check          # Check configuration
make libs           # Show library dependencies
make info           # Show platform information

# Multi-Agent Development
make agents         # Start multi-agent tmux session
make agents-attach  # Attach to running session
make agents-stop    # Stop session
make agents-status  # Show status
make agents-logs    # View communication logs

# SD Card
make sd-format      # Format SD card (WARNING: deletes data!)
make sd-status      # Check SD status
make sd-enable      # Enable logging
make sd-disable     # Disable logging
```

Run `make help` to see all available commands.

## Performance Specifications

### Current Metrics (v2.0.0)

**Memory Usage:**
- RAM: 16.3% (53,404 bytes / 320 KB)
- Flash: 16.5% (1,078,861 bytes / 16 MB)
- PSRAM: 2 MB available for display buffers

**Rendering Performance:**
- Scroll FPS: 110-120 FPS (target: 120 FPS)
- Frame Time: 8-9ms (120 FPS = 8.33ms target)
- Touch Response: <50ms (immediate feedback)
- Screen Rotation: <100ms

**Network Performance:**
- WiFi Connect: 2-5 seconds
- API Request: 1-3 seconds (network dependent)
- AI Analysis: 3-10 seconds (Gemini processing)
- Update Intervals: 30s (price), 60s (blocks), 5min (AI)

### Optimization Techniques

1. **120 FPS Scrolling:**
   - 8ms frame time limit
   - Batch rendering (`startWrite`/`endWrite`)
   - GPU clipping regions
   - Simplified card rendering (no shadows/rounded corners)

2. **Memory Efficiency:**
   - Single display buffer (no double buffering)
   - Static allocation (no dynamic malloc during runtime)
   - Optimized data structures

3. **API Rate Limiting:**
   - Conservative update intervals
   - HTTP timeout: 10 seconds
   - Graceful failure handling

## Architecture

### Core Components

```
src/
├── main.cpp                 # Entry point, setup, serial commands
├── DisplayConfig.h          # LovyanGFX pin configuration
├── Config.cpp/h             # NVRAM configuration system
├── api/
│   ├── BTCData.h            # Bitcoin data structures
│   ├── GeminiClient.cpp/h   # Gemini AI integration
│   └── OpenAIClient.cpp/h   # OpenAI integration (future)
├── screens/
│   ├── MainScreen.cpp/h     # Unified dashboard screen
│   ├── ScreenManager.cpp/h  # Screen lifecycle management
│   └── WiFiScanScreen.cpp/h # WiFi network selection
└── utils/
    ├── CrashHandler.cpp/h   # Exception logging
    └── SDLogger.cpp/h       # SD card data logging
```

### Data Flow

```
┌─────────────┐    HTTP     ┌──────────────┐    JSON    ┌──────────┐
│ mempool.space│ ─────────> │ MainScreen   │ ─────────> │ BTCData  │
│ API          │             │ fetch*()     │            │ struct   │
└─────────────┘             └──────────────┘            └──────────┘
                                    │                          │
                                    │ updateUI()               │
                                    ▼                          ▼
                            ┌──────────────┐            ┌──────────┐
                            │ drawContent()│ ◄───reads──│ btcData  │
                            └──────────────┘            └──────────┘
                                    │
                                    │ LGFX draw calls
                                    ▼
                            ┌──────────────┐
                            │ ST7796 LCD   │
                            │ 320x480      │
                            └──────────────┘
```

## API Data Source

This project uses the free [mempool.space API](https://mempool.space) with no authentication required.

**Endpoints Used:**
- `/api/v1/prices` - BTC price in USD/EUR
- `/api/v1/blocks` - Latest block information
- `/api/mempool` - Mempool statistics
- `/api/v1/fees/recommended` - Fee rate recommendations

**Rate Limiting:** Be mindful of request frequency. Default intervals are conservative (30-60 seconds).

## Troubleshooting

### Display Issues

- **Blank Screen:** Check power supply (5V 2A minimum via USB-C)
- **Wrong Orientation:** Use rotation button (tap top-right corner)
- **Touch Not Working:** Verify I2C pins (SDA=18, SCL=19, INT=7)
- **Garbled Display:** Hardware issue - check parallel bus connections

### WiFi Connection

- **Connection Failed:** Ensure 2.4GHz network (ESP32-S3 doesn't support 5GHz)
- **Credentials Error:** Use `SET_WIFI=ssid,password` via serial (115200 baud)
- **Weak Signal:** Check RSSI in WiFi card (target: >-70 dBm)

### Data Not Updating

- **Check Serial Monitor:** Look for HTTP errors or timeouts
- **Network Issues:** Verify internet connectivity
- **API Rate Limiting:** Increase update intervals if mempool.space throttles requests

### AI Signals Not Working

- **No Gemini Key:** Configure with `SET_GEMINI_KEY=your_key`
- **API Quota:** Check Gemini API usage limits (free tier: 60 requests/minute)
- **Network Timeout:** AI requests take 3-10 seconds, may timeout on slow networks

### SD Card Logging

- **Card Not Detected:** Check formatting (must be FAT32)
- **Logging Disabled:** Use `LOG_ENABLE` command
- **Check Status:** Use `CHECK_SD_CARD` command
- **Format Card:** Use `FORMAT_SD_CARD` (WARNING: deletes all data!)

See `docs/guides/debugging.md` for detailed troubleshooting procedures.

## Project Structure

```
btc-dashboard/
├── src/
│   ├── main.cpp             # Main application code
│   ├── DisplayConfig.h      # Display pin configuration
│   ├── Config.cpp/h         # NVRAM configuration system
│   ├── api/                 # API clients and data structures
│   ├── screens/             # Screen implementations
│   └── utils/               # Utilities (SD logging, crash handler)
├── docs/
│   ├── guides/              # User guides and tutorials
│   ├── features/            # Feature specifications
│   ├── development/         # Development roadmap
│   └── testing/             # Testing documentation
├── scripts/
│   ├── tmux_agents.sh       # Multi-agent session manager
│   ├── monitor_serial.py    # Serial monitoring tools
│   └── capture_debug_screens.py  # Screenshot capture
├── .claude/
│   ├── agents/              # Specialized agent definitions
│   └── hooks/               # Git hooks for automation
├── platformio.ini           # PlatformIO configuration
├── Makefile                 # Build automation
├── README.md                # This file
└── CLAUDE.md                # Multi-agent development guide
```

## Dependencies

### External Libraries (Auto-installed by PlatformIO)

- [LovyanGFX](https://github.com/lovyan03/LovyanGFX) v1.1.0 - Modern graphics and display driver
- [ArduinoJson](https://arduinojson.org/) v6.21.0 - JSON parsing for API responses
- [FT6X36](https://github.com/strange-v/FT6X36) - Capacitive touch controller driver

### Internal Components

Project-specific code in `src/`:
- Bitcoin API clients (GeminiClient, OpenAIClient)
- Screen management system
- NVRAM configuration
- SD card logging
- Crash handler

## Device Limitations

### Network
- ⚠️ WiFi: 2.4GHz only (no 5GHz support)
- ⚠️ API Rate Limits: mempool.space may throttle requests
- ⚠️ AI API Costs: ~$5-10/month at 5-minute refresh

### Display & UI
- ⚠️ Screen Size: 480x320 pixels limits information density
- ⚠️ Touch Accuracy: May require careful tapping on small elements

### Power & Stability
- ⚠️ Power Source: USB-powered only (no battery)
- ⚠️ millis() Rollover: Potential issue after 49 days uptime
- ⚠️ Long-term Stability: Crash handler mitigates but not exhaustively tested

### Security
- ⚠️ Plaintext Storage: WiFi/API keys in NVRAM (not encrypted)
- ⚠️ No Secure Element: Not suitable for wallet/signing operations

## Contributing

Contributions welcome! Please:
1. Fork the repository
2. Create a feature branch
3. Test thoroughly on actual hardware
4. Submit a Pull Request

See `CLAUDE.md` for multi-agent development workflows.

## License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.

## Acknowledgments

- Bitcoin data: [mempool.space](https://mempool.space) API
- AI analysis: [Google Gemini](https://ai.google.dev/) API
- Built with: [PlatformIO](https://platformio.org/)
- Graphics: [LovyanGFX](https://github.com/lovyan03/LovyanGFX)

## Support

For issues and questions:
- Open an [issue](https://github.com/bemindlab/SC01-ESP32-Bitcoin-Dashboard/issues)
- Check [mempool.space API docs](https://mempool.space/docs/api)
- Read [multi-agent development guide](CLAUDE.md)

---

**Note:** This project is for educational purposes. Always verify Bitcoin data from multiple sources for critical decisions.
