# Bitcoin Dashboard for Panlee SC01 Plus

[![PlatformIO CI](https://img.shields.io/badge/PlatformIO-passing-brightgreen)](https://platformio.org/)
[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)

Real-time Bitcoin dashboard running on ESP32-S3 with 3.5" IPS touch display. Displays live Bitcoin data from mempool.space API without requiring an API key.

![Bitcoin Dashboard](https://img.shields.io/badge/BTC-Dashboard-orange)

## Features

### Core Dashboard
- 💰 **Live BTC Price** - Real-time updates (USD/EUR) from mempool.space
- 📈 **24h Change** - Price change tracking
- 🧱 **Block Height** - Latest block information
- 📊 **Mempool Count** - Pending transactions
- ⛽ **Fast Fee** - Current fee rate (sat/vB)
- 📶 **WiFi Signal** - Network signal strength

### Display Features
- 📱 **Simple Card Layout** - 6 data cards in 2-column grid
- 📜 **Vertical Scroll** - Drag to scroll through all cards
- 🔄 **Screen Rotation** - Tap top-right corner to rotate (0°/90°/180°/270°)
- 🎨 **High Contrast Colors** - White text on dark cards for easy reading
- ⚡ **Real-time Updates** - Auto-refresh every 30 seconds

### System Features
- ⚙️ **Configuration System** - NVRAM storage for WiFi credentials
- 🔄 **Auto-Reconnect** - Automatic WiFi reconnection on network loss
- 📸 **Screenshot Capture** - Debug via serial screenshot transmission
- 🔧 **Serial Commands** - Configure device via serial monitor (115200 baud)
- 🌐 **WiFi Scan** - Touch-based network selection (on first boot)

## Hardware Requirements

- **Board:** Panlee SC01 Plus (WT32-SC01 Plus)
  - **MCU:** ESP32-S3-WROVER-N16R2
  - **Flash:** 16MB SPI Flash
  - **PSRAM:** 2MB PSRAM (OPI)
  - **Display:** 3.5" IPS TFT LCD, 320x480 resolution, ST7796 driver
  - **Touch:** FT6X36 capacitive touch controller (I2C)
  - **Connectivity:** Wi-Fi 802.11 b/g/n (2.4GHz), Bluetooth 5.0 LE
  - **USB:** USB-C port for programming and power
  - **Dimensions:** Development board with integrated display

> **📄 Full Hardware Specifications:** See [docs/WT32-SC01_Plus_ESP32-S3_Datasheet.pdf](docs/WT32-SC01_Plus_ESP32-S3_Datasheet.pdf) for complete technical details

## Pin Configuration

| Function | GPIO |
|----------|------|
| TFT MOSI | 13 |
| TFT SCLK | 14 |
| TFT CS | 15 |
| TFT DC | 21 |
| TFT RST | 22 |
| TFT BL | 23 |
| Touch SDA | 18 |
| Touch SCL | 19 |

## Getting Started

### Prerequisites

- [PlatformIO](https://platformio.org/) installed (VS Code extension or CLI)
- WiFi network (2.4GHz only - ESP32-S3 doesn't support 5GHz)

### Installation

1. **Clone the repository**
   ```bash
   git clone git@github.com:bemindlab/SC01-ESP32-Bitcoin-Dashboard.git
   cd SC01-ESP32-Bitcoin-Dashboard
   ```

2. **Configure WiFi credentials**

   **Option A: Using Configuration Template (Recommended)**
   ```bash
   # Copy the template
   cp configs/keys.txt.template .tmp/keys.txt

   # Edit .tmp/keys.txt and add your credentials:
   # SET_WIFI=YourNetworkSSID,YourNetworkPassword

   # Apply configuration via serial monitor after first boot
   # Or use: make patch-keys (if implemented)
   ```

   **Option B: Via Serial Monitor (After First Boot)**
   ```
   # Connect to serial monitor (115200 baud)
   SET_WIFI=YourNetworkSSID,YourNetworkPassword
   ```

   **Option C: Edit Config.cpp (Legacy Method)**
   ```cpp
   // Edit src/Config.cpp if needed for hardcoded defaults
   // Note: NVRAM configuration is preferred
   ```

3. **Build the project**

   **Standard Build (Multi-Screen Mode):**
   ```bash
   # Using Make (recommended)
   make build

   # Or using PlatformIO directly
   pio run
   ```

   **Single Screen Mode (Dashboard Only - Reduced Memory):**
   ```bash
   # Using Make
   make build-single

   # Or using PlatformIO directly
   pio run -e sc01_plus_single
   ```

   Single Screen Mode:
   - Dashboard-only (excludes WiFi, Settings, News, Trading screens)
   - Saves ~51KB flash memory
   - No swipe navigation
   - Configure via serial commands only
   - Ideal for simple, production deployments

4. **Upload to device**
   ```bash
   # Using Make
   make upload

   # Or using PlatformIO directly
   pio run --target upload
   ```

5. **Monitor serial output** (optional)
   ```bash
   # Using Make
   make monitor

   # Or using PlatformIO directly
   pio device monitor
   ```

For a complete list of available commands, run `make help` or see the [Development Commands](#development-commands) section.

## Configuration

### WiFi Setup

The device stores WiFi credentials in NVRAM (non-volatile memory). Three configuration methods are available:

1. **Configuration Template** (Recommended for initial setup)
   - Copy `configs/keys.txt.template` to `.tmp/keys.txt`
   - Edit the file with your WiFi credentials and API keys
   - Use serial monitor to send commands after first boot

2. **Serial Monitor** (Recommended for updates)
   ```
   SET_WIFI=YourNetworkSSID,YourNetworkPassword
   STATUS  # Verify configuration
   ```

3. **Settings Screen** (Touch interface)
   - Navigate to Settings from the main dashboard
   - Follow on-screen instructions to configure WiFi

### API Keys (Optional)

For AI-powered features (Bitcoin News and Trading Suggestions):

```bash
# Gemini API (for Trading Suggestions)
SET_GEMINI_KEY=AIzaSy...

# OpenAI API (for Bitcoin News)
SET_OPENAI_KEY=sk-proj-...
```

Get your API keys:
- **Gemini:** https://makersuite.google.com/app/apikey
- **OpenAI:** https://platform.openai.com/api-keys

See `docs/configuration-system-guide.md` for complete configuration documentation.

## Usage

Once powered on, the dashboard will:
1. Connect to WiFi using stored credentials
2. Fetch initial Bitcoin data from mempool.space
3. Display real-time information across multiple screens
4. Auto-update data at configured intervals

### Navigation

- **Main Dashboard** - BTC price, blocks, mempool, fees
- **WiFi Icon** (tap) - Network selection and management
- **Settings Icon** (tap) - Configuration and preferences
- **News** - Bitcoin news with AI summaries (requires OpenAI key)
- **Trading** - AI-powered trading insights (requires Gemini key)

### Update Intervals

Default update intervals (configurable via serial or Settings screen):
- **Price:** 30 seconds
- **Block Info:** 60 seconds
- **Mempool & Fees:** 30 seconds

Change intervals via serial monitor:
```
SET_PRICE_INTERVAL=60000    # 60 seconds
SET_BLOCK_INTERVAL=120000   # 2 minutes
```

## Customization

### Change Update Intervals

Use serial commands (no code changes needed):
```bash
SET_PRICE_INTERVAL=60000     # Price updates every 60 seconds
SET_BLOCK_INTERVAL=120000    # Block updates every 2 minutes
SET_MEMPOOL_INTERVAL=45000   # Mempool updates every 45 seconds
STATUS                       # Verify your changes
```

Or edit via Settings screen (touch interface).

### Add More Data

The [mempool.space API](https://mempool.space/docs/api) provides additional endpoints:
- `/api/v1/difficulty-adjustment` - Difficulty adjustment data
- `/api/v1/lightning/statistics/latest` - Lightning Network stats
- `/api/address/{address}` - Address information
- `/api/tx/{txid}` - Transaction details

## Device Limitations & Considerations

### Current Hardware Utilization (v1.0.0-beta)

**Flash Memory:**
- **Used:** ~1.03 MB (1,031,961 bytes)
- **Total:** 16 MB
- **Available:** ~15 MB (93.6% free)
- **Status:** ✅ Plenty of room for additional features

**RAM (SRAM):**
- **Used:** ~48.4 KB (48,444 bytes)
- **Total:** 320 KB
- **Available:** ~272 KB (85.2% free)
- **Status:** ✅ Sufficient headroom for operations

**PSRAM (External):**
- **Total:** 2 MB
- **Usage:** Display buffers, LVGL graphics
- **Status:** ✅ Adequate for current UI complexity

### Practical Limitations

**Network & API:**
- ⚠️ **WiFi:** 2.4GHz only (ESP32-S3 limitation, no 5GHz support)
- ⚠️ **API Rate Limits:** mempool.space may throttle excessive requests
- ⚠️ **AI API Costs:** OpenAI/Gemini usage incurs costs (~$5-10/month at 5-min refresh)
- ⚠️ **HTTP Timeout:** 10-second timeout per request (may fail on slow networks)
- ⚠️ **Concurrent Connections:** Limited to 1 HTTP connection at a time

**Display & UI:**
- ⚠️ **Screen Size:** 480x320 pixels limits information density
- ⚠️ **Touch Accuracy:** Capacitive touch may require recalibration
- ⚠️ **Scrolling:** Large text may require scrolling (implemented)
- ⚠️ **Animations:** Limited smooth animations due to refresh rate

**Power & Stability:**
- ⚠️ **Power Source:** USB-powered only (no battery support yet)
- ⚠️ **Long-term Stability:** Not tested beyond 24-48 hours continuous operation
- ⚠️ **millis() Rollover:** Potential issue after 49 days of continuous operation
- ⚠️ **Crash Recovery:** No watchdog timer implemented yet (on roadmap)

**Development State:**
- ⚠️ **Beta Software:** v1.0.0-beta - features complete but stability testing ongoing
- ⚠️ **Field Testing:** Limited to single development unit
- ⚠️ **Error Handling:** Basic error recovery, not exhaustive
- ⚠️ **Security:** WiFi/API keys stored in NVRAM (plaintext, not encrypted)

### Estimated Capacity

**Additional Features Possible:**
- ✅ **2-3 more screens** without performance degradation
- ✅ **5-10 more API integrations** with current RAM
- ✅ **Historical charts** (limited data points due to RAM)
- ✅ **Notification system** (Telegram/email)
- ⚠️ **Video/animation** (limited by PSRAM and CPU)
- ⚠️ **Complex visualizations** (may impact responsiveness)

**NOT Recommended:**
- ❌ **Bitcoin full node** (insufficient storage and RAM)
- ❌ **Mining operations** (ESP32 not designed for mining)
- ❌ **Wallet/signing** (security concerns, no secure element)
- ❌ **Real-time streaming** (network bandwidth and processing limits)
- ❌ **Multiple simultaneous API calls** (connection limits)

### Performance Expectations

**Typical Response Times:**
- **WiFi Connect:** 2-5 seconds
- **API Request:** 1-3 seconds (network dependent)
- **AI Analysis:** 3-10 seconds (OpenAI/Gemini processing)
- **Screen Navigation:** <100ms (instant feel)
- **Touch Response:** <50ms (immediate feedback)

**Update Frequencies:**
- **Price:** 30 seconds (configurable 10s-5min)
- **Blocks:** 60 seconds (configurable 30s-10min)
- **Mempool:** 30 seconds (configurable 10s-5min)
- **Trading AI:** 5 minutes (configurable 1-60min, cost consideration)
- **News:** Manual refresh (API cost consideration)

### Recommendations for Optimal Operation

1. **WiFi Signal:** Maintain strong 2.4GHz signal (RSSI > -70 dBm)
2. **API Keys:** Use separate development keys with rate limits
3. **Update Intervals:** Balance freshness vs. API costs/limits
4. **Power Supply:** Use quality USB power adapter (5V 2A minimum)
5. **Monitoring:** Check serial output periodically for errors
6. **Memory:** Monitor RAM usage if adding custom features
7. **Testing:** Test new features in isolation before deployment

### Future Improvements (Roadmap)

**Planned for v1.0.0 Stable:**
- Watchdog timer for automatic crash recovery
- millis() rollover handling (49-day bug fix)
- Extended stability testing (48+ hours)
- Memory leak detection and prevention

**Planned for v2.0.0:**
- Flash encryption for secure key storage
- OTA firmware updates
- Web configuration interface
- MQTT monitoring for remote status

See `docs/development/development-plan.md` for complete roadmap.

## Development Commands

### Using Make (Recommended)

The project includes a Makefile for convenient shortcuts:

```bash
# Build and Upload
make build          # Build the project
make upload         # Upload firmware to device
make all            # Build and upload in one command
make flash          # Upload and monitor serial output

# Monitoring
make monitor        # Open serial monitor (115200 baud)
make debug-monitor  # Monitor with exception decoder for debugging

# Maintenance
make clean          # Clean build artifacts
make devices        # List connected serial devices
make update         # Update PlatformIO core and libraries

# Development
make erase          # Erase flash memory (useful for troubleshooting)
make check          # Check environment and configuration
make libs           # Show installed library dependencies
make info           # Show platform information
```

Run `make help` to see all available commands.

### Using PlatformIO CLI

You can also use PlatformIO commands directly:

```bash
# Build and flash
pio run                              # Build the project
pio run --target upload              # Upload firmware to device
pio device monitor                   # Open serial monitor

# Debugging
pio device monitor --filter esp32_exception_decoder  # Decode crash dumps
pio device list                      # List connected devices

# Maintenance
pio run --target clean               # Clean build files
pio lib update                       # Update libraries
```

## API Data Source

This project uses the free [mempool.space API](https://mempool.space) with no authentication required.

**Rate Limiting:** Be mindful of request frequency to avoid rate limiting. Default intervals are conservative.

## Troubleshooting

### Display Not Working
- Verify GPIO pins match your hardware in `platformio.ini`
- Try different rotation values in `src/main.cpp`:
  ```cpp
  tft.setRotation(0-3);  // Try values 0, 1, 2, or 3
  ```

### WiFi Connection Failed
- Ensure you're using a 2.4GHz network (ESP32-S3 doesn't support 5GHz)
- Double-check SSID and password in `src/main.cpp`
- Monitor serial output for connection errors

### Data Not Updating
- Check serial monitor for HTTP request errors
- Verify internet connectivity
- mempool.space may have rate limits - try increasing update intervals

## Project Structure

```
btc-dashboard/
├── src/
│   ├── main.cpp          # Main application code
│   ├── lv_conf.h         # LVGL configuration
│   ├── DisplayConfig.h   # Display configuration
│   └── screens/          # Screen implementations
├── libs/                 # Internal/custom libraries
│   └── (empty - project-specific libraries)
├── docs/                 # Documentation directory
│   ├── README.md         # Documentation index
│   ├── WT32-SC01_Plus_ESP32-S3_Datasheet.pdf  # Hardware datasheet
│   ├── debugging-guide.md        # Debugging procedures
│   ├── development-plan.md       # Development roadmap
│   ├── manual-upload-guide.md    # Manual upload instructions
│   └── ui-improvements.md        # UI enhancement roadmap
├── scripts/              # Build and automation scripts
│   └── (empty - reserved for future scripts)
├── releases/             # Pre-compiled firmware binaries
│   └── (empty - releases will be added here)
├── platformio.ini        # PlatformIO configuration
├── Makefile              # Build automation shortcuts
├── README.md             # This file
└── CLAUDE.md             # Claude Code documentation
```

### Directory Purposes

- **src/** - Main application source code
- **libs/** - Internal project-specific libraries (auto-included by PlatformIO)
- **docs/** - All documentation files (guides, datasheets, plans)
- **scripts/** - Build automation, testing, and deployment scripts
- **releases/** - Versioned firmware binaries for distribution
- **.claude/** - Claude Code configuration (agents, hooks)

## Dependencies

### External Libraries

These libraries are automatically downloaded by PlatformIO (defined in `platformio.ini`):

- [LovyanGFX](https://github.com/lovyan03/LovyanGFX) v1.1.0 - Modern graphics and display driver
- [ArduinoJson](https://arduinojson.org/) v6.21.0 - JSON parsing for API responses
- [FT6X36](https://github.com/strange-v/FT6X36) - Capacitive touch controller driver

### Internal Libraries

Project-specific libraries in the `libs/` directory (currently empty). Future internal libraries may include:
- Bitcoin API client wrappers
- Display management utilities
- Custom UI components
- Hardware abstraction layers

Internal libraries are automatically discovered and included by PlatformIO - no additional configuration needed.

## Releases

Pre-compiled firmware binaries will be available in the `releases/` directory for easy deployment without requiring a build environment.

### Creating a Release

To create a firmware release:

1. Build the project:
   ```bash
   make build
   ```

2. Locate the compiled binary:
   ```
   .pio/build/sc01_plus/firmware.bin
   ```

3. Copy and rename with version:
   ```bash
   cp .pio/build/sc01_plus/firmware.bin releases/btc-dashboard-v1.0.0-sc01plus.bin
   ```

4. Document changes in `releases/CHANGELOG.md`

### Flashing Pre-compiled Firmware

To flash a pre-compiled binary:

```bash
# Using esptool.py
esptool.py --chip esp32s3 --port /dev/cu.usbserial-* write_flash 0x0 releases/btc-dashboard-v*.bin

# Or using PlatformIO
pio run --target upload --upload-port /dev/cu.usbserial-*
```

## Contributing

Contributions are welcome! Please feel free to submit a Pull Request.

## License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.

## Acknowledgments

- Bitcoin data provided by [mempool.space](https://mempool.space)
- Built with [PlatformIO](https://platformio.org/)
- UI powered by [LVGL](https://lvgl.io/)

## Support

For issues and questions:
- Open an [issue](https://github.com/bemindlab/SC01-ESP32-Bitcoin-Dashboard/issues)
- Check [mempool.space API docs](https://mempool.space/docs/api)

---

**Note:** This project is for educational purposes. Always verify Bitcoin data from multiple sources for critical decisions.
