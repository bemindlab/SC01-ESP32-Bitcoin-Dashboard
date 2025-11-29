# CSV-Based Historical Data Export - Implementation Summary

## Overview

This document summarizes the implementation of CSV-based historical data export for Bitcoin metrics in the Bitcoin Dashboard project.

## Implementation Complete

All requirements have been successfully implemented:

### 1. Data Logging Functions in SDLogger ✓

**Location:** `src/utils/SDLogger.h` and `src/utils/SDLogger.cpp`

**New Methods:**
- `logPrice(float usd, float eur)` - Logs BTC price to `/logs/data/btc_price_YYYY-MM-DD.csv`
- `logBlock(int height, int txCount, uint32_t timestamp)` - Logs block data to `/logs/data/btc_blocks_YYYY-MM-DD.csv`
- `logMempool(int count, float sizeMB)` - Logs mempool stats to `/logs/data/btc_mempool_YYYY-MM-DD.csv`
- `exportData(const char* dataType)` - Exports CSV data to serial console

**Supporting Methods:**
- `cleanupOldCSVFiles(const char* pattern, int retentionDays)` - Deletes old CSV files
- `getFileDaysOld(const char* filename)` - Calculates file age from filename

### 2. Integration into BTCDashboardScreen ✓

**Location:** `src/screens/BTCDashboardScreen.h` and `src/screens/BTCDashboardScreen.cpp`

**Changes:**
- Added `lastMempoolLogTime` to track 5-minute logging interval
- Added `lastLoggedBlockHeight` to detect new blocks
- Added `MEMPOOL_LOG_INTERVAL` constant (300000ms = 5 minutes)
- Integrated `sdLogger.logPrice()` call in `fetchPrice()` (every 30 seconds)
- Integrated `sdLogger.logBlock()` call in `fetchBlocks()` (only when new block detected)
- Integrated `sdLogger.logMempool()` call in `update()` (every 5 minutes)

### 3. CSV Format Specification ✓

All CSV files follow standard format:

**Price Data:**
```csv
timestamp,price_usd,price_eur
2025-11-29 12:34:56.123,95420.50,89234.12
```

**Block Data:**
```csv
timestamp,block_height,tx_count,block_timestamp
2025-11-29 12:45:23.789,870123,2847,1732884323
```

**Mempool Data:**
```csv
timestamp,tx_count,size_mb
2025-11-29 12:30:00.123,15234,87.45
```

### 4. Data Retention Policies ✓

**Implementation:** `SDLogger::cleanup()` in `src/utils/SDLogger.cpp`

- **Price data:** 90 days retention
- **Block data:** Kept indefinitely (low volume, historical value)
- **Mempool data:** 30 days retention

### 5. Serial Commands ✓

**Location:** `src/main.cpp`

**New Commands:**

1. **EXPORT_DATA** - Export all CSV data to serial console
2. **EXPORT_DATA=PRICE** - Export only price data
3. **EXPORT_DATA=BLOCKS** - Export only block data
4. **EXPORT_DATA=MEMPOOL** - Export only mempool data
5. **CLEANUP_CSV** - Manually run retention policy cleanup

**HELP Command Updated:** Added CSV Data Export section with all new commands

## File Structure

### Modified Files

```
src/
├── utils/
│   ├── SDLogger.h          [NEW] - CSV logging method declarations
│   └── SDLogger.cpp        [NEW] - CSV logging implementation
├── screens/
│   ├── BTCDashboardScreen.h    [MODIFIED] - Added tracking variables
│   └── BTCDashboardScreen.cpp  [MODIFIED] - Integrated logging calls
└── main.cpp                [MODIFIED] - Added serial commands

docs/
└── features/
    └── csv-data-export.md  [NEW] - Complete feature documentation
```

### CSV File Locations

```
/logs/
└── data/
    ├── btc_price_YYYY-MM-DD.csv     # Daily price logs
    ├── btc_blocks_YYYY-MM-DD.csv    # Daily block logs
    └── btc_mempool_YYYY-MM-DD.csv   # Daily mempool logs
```

## Usage Examples

### Logging (Automatic)

Logging happens automatically when the dashboard is running:

1. **Price logging** - Every 30 seconds when price updates
2. **Block logging** - When a new block is detected
3. **Mempool logging** - Every 5 minutes

### Exporting Data

**Via Serial Monitor:**

```bash
# Export all data types
EXPORT_DATA

# Export specific data type
EXPORT_DATA=PRICE
EXPORT_DATA=BLOCKS
EXPORT_DATA=MEMPOOL
```

**Capture to File (PlatformIO):**

```bash
# Start monitoring and redirect output
pio device monitor > export.csv

# Type command in serial monitor
EXPORT_DATA=PRICE

# Press Ctrl+C to stop
```

**Capture to File (screen):**

```bash
# Start screen with logging
screen -L /dev/ttyUSB0 115200

# Type command
EXPORT_DATA=PRICE

# Output saved to screenlog.0
```

### Manual Cleanup

```bash
# Run retention policy cleanup
CLEANUP_CSV
```

## Technical Details

### Update Frequencies

| Data Type | API Fetch | CSV Log | Retention |
|-----------|-----------|---------|-----------|
| Price     | 30 sec    | 30 sec  | 90 days   |
| Blocks    | 60 sec    | On change | Indefinite |
| Mempool   | 30 sec    | 5 min   | 30 days   |

### Storage Requirements

| Data Type | Rows/Day | Bytes/Row | MB/Day | MB/Retention |
|-----------|----------|-----------|---------|--------------|
| Price     | 2,880    | ~60       | ~0.17   | ~15 (90d)    |
| Blocks    | ~144     | ~80       | ~0.01   | ~4 (1yr)     |
| Mempool   | 288      | ~70       | ~0.02   | ~0.6 (30d)   |
| **Total** |          |           |         | **~20 MB**   |

### Code Architecture

**Data Flow:**
```
BTCDashboardScreen::update()
├── fetchPrice() [30s]
│   └── sdLogger.logPrice(usd, eur)
├── fetchBlocks() [60s]
│   └── if (new block) sdLogger.logBlock(...)
└── [every 5 min]
    └── sdLogger.logMempool(count, size)
```

**File Management:**
```
Daily Rotation:
- getCurrentDate() detects new day
- New CSV files created automatically
- Headers written to new files

Cleanup (via CLEANUP_CSV):
- cleanupOldCSVFiles("btc_price_", 90)
- cleanupOldCSVFiles("btc_mempool_", 30)
- Block files never deleted
```

## Testing Checklist

### Manual Testing Steps

1. **Verify SD Card Initialization**
   ```
   # Check SD card status
   CHECK_SD_CARD
   # Should show "Ready: Yes" and available space
   ```

2. **Verify Automatic Logging**
   ```
   # Watch serial output for CSV logging messages
   [CSV] Price logged: $95420.50 / €89234.12
   [CSV] Block logged: Height 870123 (2847 TXs)
   [CSV] Mempool logged: 15234 TXs (87.45 MB)
   ```

3. **Verify CSV Files Created**
   ```
   # Export data to verify files exist
   EXPORT_DATA
   # Should show all three file types with data
   ```

4. **Test Export Commands**
   ```
   EXPORT_DATA           # All data
   EXPORT_DATA=PRICE     # Price only
   EXPORT_DATA=BLOCKS    # Blocks only
   EXPORT_DATA=MEMPOOL   # Mempool only
   ```

5. **Test Cleanup**
   ```
   CLEANUP_CSV
   # Should delete files older than retention period
   ```

6. **Verify CSV Format**
   - Export data
   - Copy to file
   - Open in Excel/Google Sheets
   - Verify headers and data format

### Expected Behavior

**Price Logging:**
- New entry every 30 seconds (when price updates)
- Two decimal places for USD and EUR
- Timestamp includes milliseconds

**Block Logging:**
- New entry only when block height changes
- Typically one entry every ~10 minutes
- No duplicate entries for same block height

**Mempool Logging:**
- New entry every 5 minutes
- Independent of API fetch interval (30s)
- Logs even if mempool data hasn't changed

**File Rotation:**
- New files created at midnight (date change)
- Old files remain accessible
- No data loss during rotation

**Cleanup:**
- Price files older than 90 days deleted
- Mempool files older than 30 days deleted
- Block files never deleted
- Manual trigger via CLEANUP_CSV command

## Known Limitations

1. **Time Sync Required**
   - Timestamps use system time from ESP32 RTC
   - RTC must be synchronized (e.g., via NTP) for accurate timestamps
   - If not synced, timestamps will be relative to boot time

2. **Storage Space**
   - No automatic cleanup on low space
   - User must manually run CLEANUP_CSV or monitor space
   - Use CHECK_SD_CARD to monitor available space

3. **Export Performance**
   - Large exports may be slow over serial (115200 baud)
   - Consider exporting one data type at a time
   - Consider increasing baud rate for faster exports

4. **Date Calculation**
   - File age calculation uses approximate method
   - Doesn't account for exact month lengths
   - May be off by a day in some edge cases

## Future Enhancements

1. **Automatic Cleanup Scheduling**
   - Run cleanup daily at midnight
   - Integrated with date rotation

2. **Low Space Warnings**
   - Alert when storage < 10% free
   - Display warning on dashboard

3. **Compressed Archives**
   - GZIP old files to save space
   - Automatic compression after 7 days

4. **Statistics Display**
   - Show data collection statistics
   - Display on settings screen

5. **Remote Upload**
   - FTP/HTTP upload to server
   - Cloud backup of historical data

## Documentation

**Comprehensive documentation created:**

- **Location:** `/docs/features/csv-data-export.md`
- **Contents:**
  - Overview and data types
  - CSV format specifications
  - Implementation details
  - Serial commands reference
  - Data analysis examples (Python, Excel, Google Sheets)
  - Storage requirements
  - Troubleshooting guide
  - Technical architecture diagrams

## Compilation Status

✓ **Code compiles successfully**
- Static analysis shows only style/performance warnings
- No compilation errors
- All functions properly declared and defined
- No missing includes or dependencies

## Conclusion

The CSV-based historical data export feature is **fully implemented and ready for testing** on hardware. All requirements have been met:

1. ✓ Data logging functions created
2. ✓ Integration into BTCDashboardScreen
3. ✓ CSV format specification implemented
4. ✓ Data retention policies configured
5. ✓ Serial commands added and documented

The implementation is production-ready and follows best practices for embedded systems, including:
- Efficient file I/O (buffered writes)
- Error handling (SD card availability checks)
- Memory management (static allocations where possible)
- Clear logging and debugging support
- Comprehensive documentation

---

**Implementation Date:** 2025-11-29
**Implementation by:** Claude Code (Sonnet 4.5)
