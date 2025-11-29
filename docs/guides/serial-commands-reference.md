# Serial Commands Reference

Quick reference for all available serial commands in the Bitcoin Dashboard.

## Display Commands

### SCREENSHOT
Captures the current display buffer and sends it over serial as raw RGB565 data.

**Usage:**
```
SCREENSHOT
```

**Output:**
- Binary RGB565 pixel data (480x320 pixels = 307,200 bytes)
- Use capture script to save as PNG/BMP

---

## Device Status Commands

### STATUS
Shows current device status including WiFi, memory, and configuration.

**Usage:**
```
STATUS
```

**Output:**
```
WiFi: Connected
Free Heap: 234560 bytes
Uptime: 3600 seconds
[Configuration details...]
```

---

## SD Card Commands

### CHECK_SD_CARD
Displays detailed SD card diagnostics including status, space, and pin configuration.

**Usage:**
```
CHECK_SD_CARD
```

**Output:**
```
=== SD Card Status ===
Logger Ready: Yes
Logger Enabled: Yes
Card Present: Yes
Status: Active
Free Space: 14.23 GB
Total Space: 15.92 GB
```

### REINIT_SD
Attempts to reinitialize the SD card (useful for hot-swap recovery).

**Usage:**
```
REINIT_SD
```

**Output:**
```
=== Reinitializing SD Card ===
✓ SD card reinitialized successfully
```

### FORMAT_SD_CARD
Formats the SD card by deleting all files and directories.

**WARNING:** This deletes ALL data on the card!

**Usage:**
```
FORMAT_SD_CARD
```

**Output:**
```
========================================
⚠️  WARNING: SD CARD FORMAT
========================================
This will DELETE ALL DATA on the SD card!
[Deletion process...]
✓ SD card formatted successfully
```

### LOG_ENABLE
Enables SD card logging.

**Usage:**
```
LOG_ENABLE
```

### LOG_DISABLE
Disables SD card logging (useful to save power or prevent writes).

**Usage:**
```
LOG_DISABLE
```

### LOG_FLUSH
Forces the log buffer to be written to SD card immediately.

**Usage:**
```
LOG_FLUSH
```

### LOG_LEVEL
Sets the minimum log level for system logging.

**Usage:**
```
LOG_LEVEL=DEBUG
LOG_LEVEL=INFO
LOG_LEVEL=WARN
LOG_LEVEL=ERROR
LOG_LEVEL=FATAL
```

**Levels:**
- **DEBUG** - All messages (verbose)
- **INFO** - Informational and above
- **WARN** - Warnings and above
- **ERROR** - Errors and fatal only
- **FATAL** - Fatal errors only

### LOG_MEMORY
Logs current memory usage to SD card and CSV file.

**Usage:**
```
LOG_MEMORY
```

**Output:**
```
✓ Memory usage logged to SD card
```

**Logged Data:**
- Heap usage (used/total/percentage)
- PSRAM usage (used/total/percentage)
- Timestamp
- Saved to `/logs/data/memory_usage.csv`

---

## CSV Data Export Commands

### EXPORT_DATA
Exports historical Bitcoin data from CSV files to serial console.

**Usage:**
```
EXPORT_DATA              # Export all data types
EXPORT_DATA=PRICE        # Export only price data
EXPORT_DATA=BLOCKS       # Export only block data
EXPORT_DATA=MEMPOOL      # Export only mempool data
```

**Output:**
```
=== EXPORT START: PRICE ===

--- FILE: btc_price_2025-11-29.csv ---
timestamp,price_usd,price_eur
2025-11-29 12:34:56.123,95420.50,89234.12
2025-11-29 12:35:26.456,95421.00,89235.00
...

=== EXPORT END: PRICE ===
Files exported: 1
Total lines: 156
```

**Capture to File:**
```bash
# Using PlatformIO
pio device monitor > export.csv
# Then type: EXPORT_DATA=PRICE
# Press Ctrl+C to stop

# Using screen
screen -L /dev/ttyUSB0 115200
# Then type: EXPORT_DATA=PRICE
# Output saved to screenlog.0
```

### CLEANUP_CSV
Manually runs the CSV retention policy to delete old files.

**Usage:**
```
CLEANUP_CSV
```

**Output:**
```
=== SD Card Cleanup Starting ===
Deleted old CSV: btc_price_2025-08-15.csv (106 days old)
Cleanup: Deleted 2 old CSV files (btc_price_)
=== SD Card Cleanup Complete ===
```

**Retention Policies:**
- Price data: 90 days
- Mempool data: 30 days
- Block data: Never deleted (kept indefinitely)

---

## Configuration Commands

### SET_GEMINI_KEY
Sets the Gemini API key for AI-powered news analysis.

**Usage:**
```
SET_GEMINI_KEY=your-api-key-here
```

**Output:**
```
✓ Gemini API key saved successfully!
```

**Notes:**
- API key stored in NVRAM (persistent)
- Used for News screen analysis
- Get free API key: https://aistudio.google.com/app/apikey

### SET_OPENAI_KEY
Sets the OpenAI API key for alternative AI analysis.

**Usage:**
```
SET_OPENAI_KEY=your-api-key-here
```

**Output:**
```
✓ OpenAI API key saved successfully!
```

### SET_WIFI
Configures WiFi credentials (SINGLE_SCREEN_MODE only).

**Usage:**
```
SET_WIFI=YourSSID,YourPassword
```

**Notes:**
- Only available in SINGLE_SCREEN_MODE builds
- Credentials stored in NVRAM
- Device will auto-connect on next boot

### RESET_CONFIG
Resets all configuration to factory defaults.

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
- API keys (Gemini, OpenAI)
- WiFi credentials
- All user preferences
- Does NOT delete SD card data

---

## Help Command

### HELP
Shows list of all available commands with descriptions.

**Usage:**
```
HELP
```

**Output:**
```
=== Available Commands ===

[Display]
  SCREENSHOT         - Capture display buffer

[Device Status]
  STATUS             - Show device status

[Configuration]
  SET_GEMINI_KEY=xxx - Set Gemini API key
  [...]
```

---

## Command Tips

### General Usage

1. **Serial Monitor Settings:**
   - Baud rate: 115200
   - Line ending: Newline (NL) or CR+LF
   - Commands are case-sensitive

2. **PlatformIO Monitor:**
   ```bash
   pio device monitor -b 115200
   ```

3. **Screen Tool:**
   ```bash
   screen /dev/ttyUSB0 115200
   ```

4. **Minicom:**
   ```bash
   minicom -D /dev/ttyUSB0 -b 115200
   ```

### Capturing Output

**Save to File (Linux/Mac):**
```bash
pio device monitor > output.txt
# Type command
# Press Ctrl+C to stop
```

**Save Screenshots:**
```bash
# Use provided Python script
python scripts/capture_screenshot.py
```

**Save CSV Exports:**
```bash
# Redirect output
pio device monitor > export.csv

# Type command
EXPORT_DATA=PRICE

# Stop capture
Ctrl+C

# Clean up file (remove non-CSV lines)
grep -E "^[0-9]|^timestamp" export.csv > clean_export.csv
```

### Automation

**Automated Data Export (cron job):**
```bash
#!/bin/bash
# export_btc_data.sh
echo "EXPORT_DATA=PRICE" > /dev/ttyUSB0
sleep 5
cat /dev/ttyUSB0 > /data/btc_export_$(date +%Y%m%d).csv
```

**Scheduled Cleanup:**
```bash
# Every day at 3 AM
0 3 * * * echo "CLEANUP_CSV" > /dev/ttyUSB0
```

---

## Troubleshooting Commands

### Device Not Responding

1. Check serial connection:
   ```bash
   ls /dev/ttyUSB*  # Linux
   ls /dev/tty.*    # Mac
   ```

2. Try simple command:
   ```
   STATUS
   ```

3. Check baud rate (must be 115200)

### SD Card Issues

1. Check SD card status:
   ```
   CHECK_SD_CARD
   ```

2. Try reinitialization:
   ```
   REINIT_SD
   ```

3. If all else fails:
   ```
   FORMAT_SD_CARD
   ```

### WiFi Issues

1. Check current status:
   ```
   STATUS
   ```

2. In SINGLE_SCREEN_MODE, reconfigure:
   ```
   SET_WIFI=YourSSID,YourPassword
   ```

3. Restart device after setting WiFi

### Memory Issues

1. Check current memory:
   ```
   LOG_MEMORY
   STATUS
   ```

2. Look for memory leaks:
   ```
   LOG_LEVEL=DEBUG
   # Watch serial output for allocation messages
   ```

---

## Command Reference Table

| Command | Category | Arguments | Output | Notes |
|---------|----------|-----------|--------|-------|
| SCREENSHOT | Display | None | Binary data | Use capture script |
| STATUS | Status | None | Text | Shows all status info |
| CHECK_SD_CARD | SD Card | None | Text | Detailed diagnostics |
| REINIT_SD | SD Card | None | Text | Hot-swap recovery |
| FORMAT_SD_CARD | SD Card | None | Text | **DELETES ALL DATA** |
| LOG_ENABLE | SD Card | None | Text | Enable logging |
| LOG_DISABLE | SD Card | None | Text | Disable logging |
| LOG_FLUSH | SD Card | None | Text | Force buffer write |
| LOG_LEVEL | SD Card | DEBUG/INFO/WARN/ERROR/FATAL | Text | Set min level |
| LOG_MEMORY | SD Card | None | Text | Log memory stats |
| EXPORT_DATA | CSV Export | [=TYPE] | CSV data | TYPE: PRICE/BLOCKS/MEMPOOL/ALL |
| CLEANUP_CSV | CSV Export | None | Text | Delete old CSV files |
| SET_GEMINI_KEY | Config | =key | Text | Persistent in NVRAM |
| SET_OPENAI_KEY | Config | =key | Text | Persistent in NVRAM |
| SET_WIFI | Config | =SSID,Pass | Text | SINGLE_SCREEN_MODE only |
| RESET_CONFIG | Config | None | Text | Factory reset |
| HELP | Help | None | Text | Show all commands |

---

## Related Documentation

- [CSV Data Export Guide](../features/csv-data-export.md)
- [SD Card Logging](../features/logging.md)
- [Configuration Guide](../guides/configuration.md)
- [Debugging Guide](../guides/debugging-guide.md)
