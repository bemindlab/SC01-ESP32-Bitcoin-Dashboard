# CSV Data Export Feature

## Overview

The Bitcoin Dashboard now includes comprehensive CSV-based historical data logging for all Bitcoin metrics. This feature automatically logs price, block, and mempool data to the SD card in CSV format, with configurable retention policies and serial console export capabilities.

## Logged Data Types

### 1. Price Data (BTC/USD and BTC/EUR)

**File Format:** `/logs/data/btc_price_YYYY-MM-DD.csv`

**Columns:**
- `timestamp` - ISO 8601 timestamp with milliseconds
- `price_usd` - Bitcoin price in USD (2 decimal places)
- `price_eur` - Bitcoin price in EUR (2 decimal places)

**Update Frequency:** Every 30 seconds (when price updates successfully)

**Retention Policy:** 90 days

**Example:**
```csv
timestamp,price_usd,price_eur
2025-11-29 12:34:56.123,95420.50,89234.12
2025-11-29 12:35:26.456,95421.00,89235.00
```

### 2. Block Data

**File Format:** `/logs/data/btc_blocks_YYYY-MM-DD.csv`

**Columns:**
- `timestamp` - ISO 8601 timestamp with milliseconds
- `block_height` - Bitcoin block height
- `tx_count` - Number of transactions in block
- `block_timestamp` - Unix timestamp when block was mined

**Update Frequency:** Only when a NEW block is detected (event-driven)

**Retention Policy:** Indefinite (low volume, historical value)

**Example:**
```csv
timestamp,block_height,tx_count,block_timestamp
2025-11-29 12:45:23.789,870123,2847,1732884323
2025-11-29 13:02:15.234,870124,3012,1732885335
```

### 3. Mempool Data

**File Format:** `/logs/data/btc_mempool_YYYY-MM-DD.csv`

**Columns:**
- `timestamp` - ISO 8601 timestamp with milliseconds
- `tx_count` - Number of pending transactions
- `size_mb` - Mempool size in megabytes (2 decimal places)

**Update Frequency:** Every 5 minutes

**Retention Policy:** 30 days

**Example:**
```csv
timestamp,tx_count,size_mb
2025-11-29 12:30:00.123,15234,87.45
2025-11-29 12:35:00.456,15312,88.12
```

## Implementation Details

### Automatic Logging

Data is automatically logged to CSV files when:

1. **Price Updates** - Every successful price fetch (30 second interval)
   - Triggered in `BTCDashboardScreen::fetchPrice()`
   - Calls `sdLogger.logPrice(usd, eur)`

2. **New Block Detection** - Only when block height changes
   - Triggered in `BTCDashboardScreen::fetchBlocks()`
   - Compares `newBlockHeight` with `lastLoggedBlockHeight`
   - Calls `sdLogger.logBlock(height, txCount, timestamp)`

3. **Mempool Snapshots** - Every 5 minutes
   - Triggered in `BTCDashboardScreen::update()`
   - Uses `MEMPOOL_LOG_INTERVAL` (300000ms)
   - Calls `sdLogger.logMempool(count, sizeMB)`

### File Management

**Daily Rotation:**
- New CSV files created automatically each day at midnight
- Filename includes date: `btc_price_2025-11-29.csv`
- Headers automatically written to new files

**Retention Policy:**
- Runs via `sdLogger.cleanup()` function
- Deletes files older than retention period
- Price data: 90 days
- Mempool data: 30 days
- Block data: Never deleted (kept indefinitely)

**CSV Format:**
- Standard RFC 4180 CSV format
- Headers on first line
- Comma-separated values
- No quotes unless necessary
- Newline-terminated rows

## Serial Commands

### Export Data to Serial Console

**Command:** `EXPORT_DATA[=TYPE]`

**Usage:**
```
EXPORT_DATA              # Export all data types
EXPORT_DATA=PRICE        # Export only price data
EXPORT_DATA=BLOCKS       # Export only block data
EXPORT_DATA=MEMPOOL      # Export only mempool data
```

**Output Format:**
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
# Using screen
screen -L /dev/ttyUSB0 115200
# Then type: EXPORT_DATA=PRICE
# Output saved to screenlog.0

# Using PlatformIO
pio device monitor > export.csv
# Then type: EXPORT_DATA=PRICE
# Ctrl+C to stop
```

### Manual Cleanup

**Command:** `CLEANUP_CSV`

Manually triggers retention policy cleanup:
- Deletes price data older than 90 days
- Deletes mempool data older than 30 days
- Keeps all block data

**Example:**
```
CLEANUP_CSV
```

**Output:**
```
=== SD Card Cleanup Starting ===
Deleted old CSV: btc_price_2025-08-15.csv (106 days old)
Deleted old CSV: btc_price_2025-08-16.csv (105 days old)
Cleanup: Deleted 2 old CSV files (btc_price_)
=== SD Card Cleanup Complete ===
```

## Data Analysis

### Python Example

```python
import pandas as pd
import matplotlib.pyplot as plt

# Load price data
df = pd.read_csv('btc_price_2025-11-29.csv')
df['timestamp'] = pd.to_datetime(df['timestamp'])
df.set_index('timestamp', inplace=True)

# Plot price over time
plt.figure(figsize=(12, 6))
plt.plot(df.index, df['price_usd'], label='USD')
plt.plot(df.index, df['price_eur'], label='EUR')
plt.xlabel('Time')
plt.ylabel('Price')
plt.title('Bitcoin Price - November 29, 2025')
plt.legend()
plt.grid(True)
plt.show()

# Calculate statistics
print(f"Average BTC/USD: ${df['price_usd'].mean():.2f}")
print(f"Min BTC/USD: ${df['price_usd'].min():.2f}")
print(f"Max BTC/USD: ${df['price_usd'].max():.2f}")
```

### Excel Analysis

1. Export data: `EXPORT_DATA=PRICE`
2. Copy output to text file
3. Remove header/footer lines (keep only CSV content)
4. Open in Excel as CSV
5. Use Excel charts and pivot tables for analysis

### Google Sheets

1. Export data via serial command
2. Copy CSV content
3. Import into Google Sheets (File → Import → Paste)
4. Use built-in charts and formulas

## Storage Requirements

### Approximate File Sizes

**Price Data:**
- ~60 bytes per row (including timestamp)
- 2,880 rows per day (30 second interval)
- ~170 KB per day
- ~15 MB per 90 days (retention period)

**Block Data:**
- ~80 bytes per row
- ~144 rows per day (one block every ~10 minutes)
- ~11 KB per day
- ~4 MB per year

**Mempool Data:**
- ~70 bytes per row
- 288 rows per day (5 minute interval)
- ~20 KB per day
- ~600 KB per 30 days (retention period)

**Total Storage (with retention):**
- ~16 MB for all data types
- Negligible for modern SD cards (typically 8-32 GB)

## Configuration

### Update Intervals

Defined in `src/screens/BTCDashboardScreen.h`:

```cpp
#define PRICE_INTERVAL 30000      // 30 sec - API fetch + CSV log
#define BLOCK_INTERVAL 60000      // 60 sec - API fetch
#define MEMPOOL_INTERVAL 30000    // 30 sec - API fetch
#define MEMPOOL_LOG_INTERVAL 300000  // 5 min - CSV log only
```

### Retention Policies

Defined in `src/utils/SDLogger.cpp` → `cleanup()`:

```cpp
cleanupOldCSVFiles("btc_price_", 90);    // Price: 90 days
cleanupOldCSVFiles("btc_mempool_", 30);  // Mempool: 30 days
// Block data: kept indefinitely
```

## Troubleshooting

### No CSV Files Created

**Check SD card status:**
```
CHECK_SD_CARD
```

**Verify logging is enabled:**
```
STATUS
```

**Check if SD card has space:**
```
CHECK_SD_CARD
# Look for "Free Space" value
```

### Missing Data in CSV

**Verify WiFi connection:**
- Dashboard must be connected to WiFi to fetch data
- Check WiFi status in serial output

**Check log level:**
```
LOG_LEVEL=DEBUG
# Then watch for "[CSV]" debug messages
```

**Verify SD card not removed:**
```
CHECK_SD_CARD
# Look for "Card Present: Yes"
```

### Export Command Not Working

**Check SD card status:**
```
CHECK_SD_CARD
```

**Verify files exist:**
- Files are created in `/logs/data/` directory
- Filenames include current date
- Export only shows files that exist

**Increase baud rate for large exports:**
- Default: 115200 baud
- Large exports may take time
- Consider exporting one type at a time

## Technical Architecture

### Code Flow

```
BTCDashboardScreen::update()
  ├─> fetchPrice() [every 30s]
  │   └─> sdLogger.logPrice(usd, eur)
  │       └─> Write to /logs/data/btc_price_YYYY-MM-DD.csv
  │
  ├─> fetchBlocks() [every 60s]
  │   └─> if (newBlockHeight != lastLoggedBlockHeight)
  │       └─> sdLogger.logBlock(height, count, time)
  │           └─> Write to /logs/data/btc_blocks_YYYY-MM-DD.csv
  │
  └─> [every 5 min]
      └─> sdLogger.logMempool(count, size)
          └─> Write to /logs/data/btc_mempool_YYYY-MM-DD.csv
```

### File Management Flow

```
Daily Rotation (midnight):
  └─> getCurrentDate() returns new date
      └─> New CSV files created automatically
      └─> Headers written to new files

Cleanup (manual or scheduled):
  └─> cleanup()
      ├─> cleanupOldCSVFiles("btc_price_", 90)
      ├─> cleanupOldCSVFiles("btc_mempool_", 30)
      └─> Block files never deleted
```

## Future Enhancements

**Potential Improvements:**

1. **Automatic Cleanup Scheduling**
   - Run cleanup daily at midnight
   - Triggered by date rotation

2. **Fee Rate Logging**
   - Add CSV logging for fast/medium/slow fees
   - Similar to mempool data

3. **Compressed Archives**
   - Compress old CSV files to save space
   - GZIP format for easy decompression

4. **FTP/HTTP Upload**
   - Automatic upload to server
   - Cloud backup of historical data

5. **On-Device Analysis**
   - Calculate statistics (avg, min, max)
   - Display trends on dashboard

6. **Export Filtering**
   - Date range exports: `EXPORT_DATA=PRICE,2025-11-01,2025-11-30`
   - Last N hours: `EXPORT_DATA=PRICE,24h`

## Related Documentation

- [SD Card Logging](logging.md) - General SD card logging features
- [Serial Commands](../guides/serial-commands.md) - All available serial commands
- [API Integration](../features/api-integration.md) - mempool.space API details
