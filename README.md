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

- **Board:** Panlee SC01 Plus
  - ESP32-S3 microcontroller
  - 16MB Flash
  - OPI PSRAM
  - 3.5" IPS Touch Display (320x480, ST7796 driver)
  - Capacitive Touch (FT6X36)

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
   pio run
   ```

4. **Upload to device**
   ```bash
   pio run --target upload
   ```

5. **Monitor serial output** (optional)
   ```bash
   pio device monitor
   ```

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
│   └── lv_conf.h         # LVGL configuration
├── platformio.ini        # PlatformIO configuration
├── README.md             # This file
└── CLAUDE.md             # Claude Code documentation
```

## Dependencies

- [LVGL](https://lvgl.io/) v8.3.0 - Graphics library
- [TFT_eSPI](https://github.com/Bodmer/TFT_eSPI) v2.5.0 - Display driver
- [ArduinoJson](https://arduinojson.org/) v6.21.0 - JSON parsing
- [FT6X36](https://github.com/strange-v/FT6X36) - Touch driver

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
