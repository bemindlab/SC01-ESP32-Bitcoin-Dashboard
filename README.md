# Bitcoin Dashboard for Panlee SC01 Plus

[![PlatformIO CI](https://img.shields.io/badge/PlatformIO-passing-brightgreen)](https://platformio.org/)
[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)

Real-time Bitcoin dashboard running on ESP32-S3 with 3.5" IPS touch display. Displays live Bitcoin data from mempool.space API without requiring an API key.

![Bitcoin Dashboard](https://img.shields.io/badge/BTC-Dashboard-orange)

## Features

- 💰 **Live BTC Price** - Updates every 30 seconds (USD/EUR)
- 🧱 **Block Information** - Latest block height and transaction count
- 📊 **Mempool Statistics** - Pending transactions and mempool size
- ⛽ **Fee Rates** - Fast/Medium/Slow recommendations (sat/vB)

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

   Edit `src/main.cpp` and update your WiFi settings:
   ```cpp
   const char* WIFI_SSID = "YOUR_WIFI_SSID";
   const char* WIFI_PASSWORD = "YOUR_WIFI_PASSWORD";
   ```

3. **Build the project**
   ```bash
   # Using Make (recommended)
   make build

   # Or using PlatformIO directly
   pio run
   ```

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

## Usage

Once powered on, the dashboard will:
1. Connect to WiFi (up to 15 seconds timeout)
2. Fetch initial Bitcoin data from mempool.space
3. Display real-time information across four cards
4. Auto-update data at configured intervals

### Update Intervals

Default update intervals (configurable in `src/main.cpp`):
- **Price:** 30 seconds
- **Block Info:** 60 seconds
- **Mempool & Fees:** 30 seconds

## Customization

### Change Update Intervals

Edit the constants in `src/main.cpp`:
```cpp
const unsigned long PRICE_INTERVAL = 30000;    // 30 sec
const unsigned long BLOCK_INTERVAL = 60000;    // 1 min
const unsigned long MEMPOOL_INTERVAL = 30000;  // 30 sec
```

### Add More Data

The [mempool.space API](https://mempool.space/docs/api) provides additional endpoints:
- `/api/v1/difficulty-adjustment` - Difficulty adjustment data
- `/api/v1/lightning/statistics/latest` - Lightning Network stats
- `/api/address/{address}` - Address information
- `/api/tx/{txid}` - Transaction details

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
