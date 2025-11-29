# SD Card Commands Guide

Quick reference for SD card management commands on the Bitcoin Dashboard.

**Status:** ✅ Fully Implemented (v1.4.0)

All SD card commands are now integrated into `main.cpp` and accessible via serial monitor or convenient Make commands.

## Make Commands (Recommended)

### Format SD Card
```bash
make sd-format
```
**WARNING:** This will delete ALL data on the SD card!
- Prompts for confirmation before formatting
- Connects to device and sends FORMAT_SD_CARD command
- Displays format progress and results

### Check SD Card Status
```bash
make sd-status
```
Shows:
- Card present: Yes/No
- Status: Ready / Not Available
- Free space in GB
- Total space in GB
- Number of log files
- Logging enabled/disabled

### Enable/Disable Logging
```bash
make sd-enable      # Enable SD card logging
make sd-disable     # Disable SD card logging
```

### Flush Log Buffer
```bash
make sd-flush       # Force write buffered logs to SD card
```

### Log Memory Usage
```bash
make sd-memory      # Capture current heap/PSRAM usage to SD
```

## Serial Commands (Manual)

You can also send commands directly via serial monitor:

### Check SD Card Status
```
CHECK_SD_CARD
```
Displays detailed SD card information including:
- Card present: Yes/No
- Free space in GB
- Total space in GB
- Hot-swap detection status

### Format SD Card
```
FORMAT_SD_CARD
```
**WARNING:** Deletes all data!
- Formats the SD card
- Creates fresh log directory structure
- Resets all logging files

### SD Logging Commands
```
LOG_ENABLE          - Enable SD card logging
LOG_DISABLE         - Disable SD card logging
LOG_FLUSH           - Force flush log buffer to SD
LOG_LEVEL=DEBUG     - Set log level (DEBUG/INFO/WARN/ERROR/FATAL)
LOG_MEMORY          - Log current memory usage to SD
```

## Python Scripts (Advanced)

### Format SD Card Script
```bash
python3 scripts/sd_format.py /dev/cu.usbmodem1101
```
- Interactive confirmation prompt
- Sends FORMAT_SD_CARD command
- Displays device response

### Generic Command Script
```bash
python3 scripts/send_serial_command.py /dev/cu.usbmodem1101 "CHECK_SD_CARD"
```
Send any serial command and view response.

## Troubleshooting

### SD Card Not Detected
1. Check physical connection (card fully inserted)
2. Try removing and reinserting the SD card
3. Run `CHECK_SD_CARD` command to test hot-swap detection
4. Check serial output for initialization errors

### Format Fails
1. Remove SD card physically
2. Format using computer (FAT32 recommended)
3. Reinsert card
4. Run `CHECK_SD_CARD` to verify detection

### Logging Not Working
1. Check SD card is inserted: `CHECK_SD_CARD`
2. Enable logging: `LOG_ENABLE`
3. Check free space (need at least 10MB)
4. Try formatting: `make sd-format`

## Log File Locations

After formatting, the SD card will have this structure:
```
/logs/
  ├── app/          - Application logs (daily rotation)
  │   └── app_YYYY-MM-DD.log
  ├── api/          - API call logs
  │   └── api_YYYY-MM-DD.log
  ├── boot/         - Boot/startup logs
  │   └── boot_YYYY-MM-DD.log
  ├── data/         - Data snapshots (CSV)
  │   └── btc_data.csv
  └── errors/       - Error and crash logs
      ├── crashes_YYYY-MM-DD.log
      └── watchdog_YYYY-MM-DD.log
```

## Hot-Swap Detection

The Bitcoin Dashboard supports SD card hot-swapping:
- Card removal is detected within 5 seconds
- Card insertion automatically reinitializes logging
- Logging continues seamlessly after reinsertion
- No device restart required

## See Also

- [SD Card Logging Plan](../features/logging.md) - Detailed logging architecture
- [Debugging Guide](debugging.md) - Using logs for troubleshooting
- [Serial Commands](ticker-commands.md) - Full list of available commands
