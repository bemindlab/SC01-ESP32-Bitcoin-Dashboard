# Bitcoin Dashboard - Ticker Commands Guide

Complete reference for serial commands to control and monitor the Bitcoin Dashboard remotely.

## Quick Start

1. Open Serial Monitor at **115200 baud**
2. Type `HELP` to see all commands
3. Commands are case-sensitive
4. Press Enter to execute

## Bitcoin Ticker Commands

### GET_PRICE

Get the current Bitcoin price in USD and EUR.

**Usage:**
```
GET_PRICE
```

**Output:**
```
=== Current Bitcoin Price ===
  USD: $98456.00
  EUR: €93245.00
```

**Use Cases:**
- Price monitoring scripts
- Automated price logging
- Integration with external systems

---

### GET_SIGNAL

Get the current trading signal based on price trend analysis.

**Usage:**
```
GET_SIGNAL
```

**Output:**
```
=== Trading Signal ===
  Timeframe: 15m
  Signal: BUY
```

**Signal Types:**
- `BUY` (Green) - Price dropping >0.5%, potential dip buy
- `SELL` (Red) - Price rising >0.5%, take profit opportunity
- `HOLD` (Yellow) - Price stable within ±0.5%
- `ANALYZING` (Gray) - Insufficient data for analysis

**Use Cases:**
- Automated trading bots
- Alert systems
- Decision support tools

---

### GET_HISTORY

Retrieve the last 10 price samples from the history buffer.

**Usage:**
```
GET_HISTORY
```

**Output:**
```
=== Price History ===
  Samples: 25
  Recent prices (newest first):
    [1] $98456.00
    [2] $98423.50
    [3] $98390.25
    [4] $98367.75
    [5] $98345.00
    [6] $98322.50
    [7] $98300.25
    [8] $98278.00
    [9] $98256.75
    [10] $98234.50
  ... (15 more samples)
```

**Details:**
- Stores up to 60 samples (30 minutes at 30s intervals)
- Shows most recent 10 by default
- Useful for trend visualization

---

### REFRESH_PRICE

Force an immediate price refresh, bypassing the 30-second interval.

**Usage:**
```
REFRESH_PRICE
```

**Output:**
```
⟳ Forcing price refresh...
Price updated: $98456
✓ Price refreshed successfully
```

**Notes:**
- Updates price immediately
- Recalculates trading signal
- Refreshes display
- Use sparingly to avoid rate limiting

---

### SET_TIMEFRAME=N

Change the trading signal analysis timeframe.

**Usage:**
```
SET_TIMEFRAME=15
```

**Valid Values:**
- `5` - 5 minutes (10 samples)
- `10` - 10 minutes (20 samples)
- `15` - 15 minutes (30 samples) - **Default**
- `30` - 30 minutes (60 samples)

**Output:**
```
✓ Timeframe set to: 15m
Price Trend Analysis: 0.73% change
  First half avg: $98234.50
  Second half avg: $98952.25
  → TREND: SELL (price rising)
```

**Effect:**
- Immediately recalculates signal
- Updates display
- Affects future signal calculations

---

## Screen Navigation Commands

### GET_SCREEN

Display the currently active screen.

**Usage:**
```
GET_SCREEN
```

**Output:**
```
=== Current Screen ===
  Dashboard
```

**Possible Screens:**
- Dashboard
- News
- Trading Suggestion
- Settings
- WiFi Scan

---

### SWITCH Commands

Switch between different screens programmatically.

**Available Commands:**
```
SWITCH_DASHBOARD   - Main price display
SWITCH_NEWS        - Bitcoin news feed
SWITCH_TRADING     - AI trading suggestions
SWITCH_SETTINGS    - Configuration screen
SWITCH_WIFI        - WiFi network scanner
```

**Example:**
```
> SWITCH_TRADING
Switching to Trading Suggestion screen...
```

**Use Cases:**
- Automated screen rotation
- Demo modes
- Testing workflows

---

## WiFi Management

### WIFI_STATUS

Get detailed WiFi connection information.

**Usage:**
```
WIFI_STATUS
```

**Output (Connected):**
```
=== WiFi Status ===
  Status: Connected
  SSID: MyNetwork
  IP: 192.168.1.100
  RSSI: -45 dBm
  Channel: 6
```

**Output (Disconnected):**
```
=== WiFi Status ===
  Status: Disconnected
```

**RSSI Interpretation:**
- `-30 to -50 dBm` - Excellent signal
- `-50 to -60 dBm` - Good signal
- `-60 to -70 dBm` - Fair signal
- `-70 to -80 dBm` - Weak signal
- Below `-80 dBm` - Very weak signal

---

### SET_WIFI

Connect to a WiFi network and save credentials.

**Usage:**
```
SET_WIFI=MyNetwork,MyPassword
```

**Output:**
```
Connecting to WiFi: MyNetwork
....................
✓ WiFi connected!
IP: 192.168.1.100
✓ WiFi credentials saved!
Switching to dashboard...
```

**Notes:**
- Replaces existing WiFi credentials
- Saves to non-volatile storage
- Auto-connects on future boots
- Maximum 20-second connection timeout

---

## Display Commands

### SCREENSHOT

Capture the current display buffer to serial output as raw RGB565 data.

**Usage:**
```
SCREENSHOT
```

**Output:**
```
Screenshot command received!

SCREENSHOT_START
[binary RGB565 data stream]
SCREENSHOT_END
```

**Details:**
- 480x320 pixels
- RGB565 format (2 bytes per pixel)
- 307,200 bytes total
- Can be captured and converted to image

---

### CAPTURE_SCREEN

Save screenshot directly to SD card.

**Usage:**
```
CAPTURE_SCREEN
```

**Output:**
```
Capturing screen to SD card...
✓ Screenshot saved: /logs/screenshots/screen_2025-01-15_14-30-45.raw
```

**File Format:**
- Location: `/logs/screenshots/`
- Format: Raw RGB565 with header
- Includes timestamp and metadata

---

## Configuration Commands

### STATUS

Show comprehensive device status information.

**Usage:**
```
STATUS
```

**Output:**
```
WiFi: Connected
Free Heap: 245632 bytes
Uptime: 3847 seconds
SD Card: Ready
  Free: 29.45 GB / 31.91 GB

Configuration:
  First Run: No
  WiFi SSID: MyNetwork
  Gemini API Key: Set (40 chars)
  OpenAI API Key: Not set
```

---

### SET_GEMINI_KEY / SET_OPENAI_KEY

Configure API keys for AI-powered trading suggestions.

**Usage:**
```
SET_GEMINI_KEY=your-api-key-here
SET_OPENAI_KEY=your-api-key-here
```

**Output:**
```
✓ Gemini API key saved successfully!
```

---

### RESET_CONFIG

Reset all configuration to factory defaults.

**Usage:**
```
RESET_CONFIG
```

**Output:**
```
Resetting configuration...
✓ Configuration reset complete!
```

**Warning:** This clears:
- WiFi credentials
- API keys
- All saved settings

---

## SD Card Logging

### LOG_ENABLE / LOG_DISABLE

Control SD card logging.

**Usage:**
```
LOG_ENABLE
LOG_DISABLE
```

---

### LOG_LEVEL

Set the minimum log level for SD card output.

**Usage:**
```
LOG_LEVEL=INFO
```

**Levels (in order):**
- `DEBUG` - Verbose debugging information
- `INFO` - General information (default)
- `WARN` - Warnings
- `ERROR` - Errors
- `FATAL` - Fatal errors only

---

### LOG_MEMORY

Log current memory usage to SD card and display.

**Usage:**
```
LOG_MEMORY
```

**Output:**
```
✓ Memory usage logged to SD card
  Free Heap: 245632 / 327680 bytes (25.0% used)
  Free PSRAM: 8126464 / 8388608 bytes (3.1% used)
```

---

### CHECK_SD_CARD

Manually check SD card status and detect hot-swap.

**Usage:**
```
CHECK_SD_CARD
```

**Output:**
```
SD Card Status: Ready
  Free Space: 29.45 GB
  Total Space: 31.91 GB
```

---

## Automation Examples

### Price Monitoring Script

```bash
#!/bin/bash
# Monitor BTC price every minute

while true; do
    echo "GET_PRICE" > /dev/ttyUSB0
    sleep 60
done
```

### Auto-Timeframe Cycling

```bash
#!/bin/bash
# Cycle through all timeframes

for tf in 5 10 15 30; do
    echo "SET_TIMEFRAME=$tf" > /dev/ttyUSB0
    echo "GET_SIGNAL" > /dev/ttyUSB0
    sleep 300  # Wait 5 minutes
done
```

### Alert on Signal Change

```python
import serial
import time

ser = serial.Serial('/dev/ttyUSB0', 115200)
last_signal = None

while True:
    ser.write(b'GET_SIGNAL\n')
    time.sleep(1)

    response = ser.read_all().decode()
    if 'Signal:' in response:
        signal = response.split('Signal:')[1].strip().split()[0]
        if signal != last_signal:
            print(f"Signal changed: {last_signal} → {signal}")
            # Send notification
            last_signal = signal

    time.sleep(30)
```

---

## Troubleshooting

### Command Not Recognized

**Issue:** Commands don't work or show no output.

**Solutions:**
1. Check baud rate is 115200
2. Ensure proper line endings (newline)
3. Commands are case-sensitive
4. Type `HELP` to verify connection

### Dashboard Not Active Error

**Issue:** `✗ Dashboard not active. Switch with: SWITCH_DASHBOARD`

**Solution:**
```
SWITCH_DASHBOARD
```

Then retry the ticker command.

### Timeframe Set Failed

**Issue:** `✗ Invalid timeframe: X minutes`

**Solution:** Use only valid values: 5, 10, 15, or 30

---

## Best Practices

1. **Rate Limiting**: Don't call `REFRESH_PRICE` too frequently (respect 30s default)
2. **Screen Context**: Ticker commands only work when dashboard is active
3. **Error Handling**: Check for `✗` error markers in responses
4. **Logging**: Enable SD logging for debugging automation scripts
5. **Memory Monitoring**: Use `LOG_MEMORY` periodically in long-running scripts

---

## Integration APIs

The serial commands can be accessed programmatically via:

- **Python**: `pyserial` library
- **Node.js**: `serialport` package
- **C/C++**: Standard serial I/O
- **Shell Scripts**: Direct device file access (`/dev/ttyUSB0`)

---

## See Also

- [Unit Testing Guide](unit-testing-guide.md)
- [Single Screen Mode Guide](single-screen-mode-guide.md)
- Main documentation: [CLAUDE.md](../../CLAUDE.md)
