# SD Card Format Guide

**Version:** 1.3.0
**Feature:** On-Device SD Card Formatting
**Status:** ✅ Implemented

## Overview

The BTC Dashboard includes on-device SD card formatting capability via serial commands. This feature allows complete erasure and re-initialization of the SD card without removing it from the device.

## ⚠️ WARNING

**FORMATTING THE SD CARD IS A DESTRUCTIVE OPERATION THAT CANNOT BE UNDONE!**

All data on the SD card will be permanently deleted, including:
- All log files (system, API, boot logs)
- All data files (memory usage, API response data)
- All screenshots
- All crash dumps
- Custom files/directories

**There is no way to recover data after formatting!**

## When to Format the SD Card

Format the SD card when:
- SD card filesystem is corrupted
- SD card has errors preventing normal operation
- You want to start with a clean slate
- Preparing the SD card for a fresh deployment
- Troubleshooting persistent SD card issues

## How to Format the SD Card

### Prerequisites

1. Device connected via USB serial port
2. Serial monitor open at 115200 baud
3. Device powered on and running

### Step-by-Step Procedure

#### Step 1: Issue Format Command

In the serial monitor, type:
```
FORMAT_SD_CARD
```

You will see a warning message:
```
⚠️  ⚠️  ⚠️  DANGER ZONE ⚠️  ⚠️  ⚠️
This will DELETE ALL DATA on the SD card!
Including all logs, screenshots, and data files.

To proceed, type: FORMAT_SD_CARD_CONFIRM
To cancel, type anything else or wait 30 seconds.
```

#### Step 2: Confirm or Cancel

**To proceed with formatting:**
```
FORMAT_SD_CARD_CONFIRM
```

**To cancel:**
- Type anything else (e.g., `CANCEL`)
- Or simply wait (no timeout enforcement, but recommended to cancel explicitly)

#### Step 3: Format Execution

If confirmed, you'll see:
```
🔥 FORMATTING SD CARD - THIS CANNOT BE UNDONE! 🔥

⚠️  WARNING: SD CARD FORMAT
This will DELETE ALL DATA on the SD card!
Flushing pending writes...
Closing log files...
Re-initializing SD card...
Deleting all files and directories...
  Deleting: /logs/system/...
  Deleting: /logs/api/...
  [... progress messages ...]
Creating log directory structure...
  Creating: /logs
  Creating: /logs/system
  Creating: /logs/api
  Creating: /logs/data
  Creating: /logs/screenshots
  Creating: /logs/crashes

✓ SD card formatted successfully!
✓ Log directory structure recreated
✓ SD logging re-enabled

SD card is ready for use.
```

#### Step 4: Verification

Verify the format was successful:
```
STATUS
```

You should see:
```
SD Card: Ready
  Free: [nearly full capacity] GB / [total capacity] GB
```

## What Happens During Format

### 1. Safety Checks
- Confirmation required via two-step command
- Clear warnings displayed to user

### 2. Pre-Format Cleanup
```cpp
// Flush any pending log writes
flush();

// Close all open log files
closeLogFile();

// Unmount SD card
SD.end();
```

### 3. SD Card Re-initialization
```cpp
// Re-mount SD card with SPI configuration
SD.begin(SD_CS_PIN, SPI, 25000000, "/sd", 5, false);
```

### 4. Recursive File Deletion
The `deleteRecursive()` method performs depth-first traversal:
- Opens root directory
- Recursively processes all files and subdirectories
- Deletes files first, then directories
- Provides progress feedback via Serial

### 5. Directory Recreation
The `ensureDirectories()` method recreates the standard log structure:
```
/logs/
├── system/      # System log files
├── api/         # API request/response logs
├── data/        # Data files (memory usage, etc.)
├── screenshots/ # Screen captures
└── crashes/     # Crash dumps
```

### 6. Logger Re-initialization
```cpp
ready = true;          // Mark logger as ready
cardPresent = true;    // Mark card as present
logf(LOG_WARN, "SD card formatted - all previous data deleted");
```

## Implementation Details

### Files Modified

**`src/utils/SDLogger.h`** (line 59):
```cpp
bool formatCard(); // Format SD card (WARNING: Deletes all data)
```

**`src/utils/SDLogger.cpp`** (lines 575-688):
- `formatCard()` - Main format function
- `deleteRecursive()` - Recursive file/directory deletion

**`src/main.cpp`** (lines 332-352):
- Two-step confirmation command handling
- User-friendly warnings and feedback

### Serial Commands

| Command | Description |
|---------|-------------|
| `FORMAT_SD_CARD` | Display warning and request confirmation |
| `FORMAT_SD_CARD_CONFIRM` | Execute the format operation |

### Safety Features

1. **Two-Step Confirmation**
   - Initial warning command
   - Separate confirmation command required
   - Clear indication of destructive nature

2. **Explicit Warnings**
   - Multiple warning messages
   - Clear list of what will be deleted
   - Visual indicators (⚠️ emoji)

3. **Graceful Handling**
   - Flushes pending writes before format
   - Properly closes all files
   - Re-initializes filesystem correctly

4. **Status Reporting**
   - Progress messages during deletion
   - Clear success/failure indication
   - Verification instructions

## Error Handling

### Format Failure Scenarios

**SD Card Not Present:**
```
✗ SD card format FAILED!
Check SD card connection and try again.
```

**SD Card Re-initialization Failure:**
- SD.begin() fails after SD.end()
- Returns false immediately
- User must check hardware connection

**Partial Deletion Failure:**
- Some files may fail to delete
- Format continues despite individual file errors
- Check serial output for specific errors

### Recovery from Failed Format

If format fails:

1. **Check SD Card Connection**
   - Verify SD card is properly inserted
   - Check for physical damage
   - Try removing and reinserting

2. **Retry Format**
   - Issue `FORMAT_SD_CARD` command again
   - May succeed on second attempt

3. **Manual Format**
   - Remove SD card from device
   - Format on computer (FAT32)
   - Reinsert into device
   - Issue `CHECK_SD_CARD` command

4. **Replace SD Card**
   - If persistent failures, SD card may be defective
   - Use quality SD card (Class 10 or better)

## Testing Recommendations

### Pre-Format Testing

Before deploying format feature:

1. **Test with Non-Critical Data**
   - Use SD card with test data only
   - Verify format completes successfully
   - Verify directory recreation

2. **Test Confirmation Flow**
   - Verify `FORMAT_SD_CARD` shows warning
   - Verify typing wrong confirmation cancels
   - Verify `FORMAT_SD_CARD_CONFIRM` executes

3. **Test Post-Format Logging**
   - Issue format command
   - Verify logging continues after format
   - Check new log files are created

### Post-Format Verification

After format operation:

1. **Check Directory Structure**
   ```
   STATUS  # Should show SD card ready with full capacity
   ```

2. **Verify Logging Works**
   ```
   LOG_MEMORY  # Should create new memory usage log
   ```

3. **Verify File Creation**
   ```
   CAPTURE_SCREEN  # Should create screenshot file
   ```

## Performance Considerations

### Format Duration

Format time depends on:
- SD card size
- Number of files/directories
- SD card speed class

**Typical Duration:**
- Empty card: 2-5 seconds
- Card with 100 log files: 10-30 seconds
- Card with 1000+ files: 1-3 minutes

### Watchdog Considerations

**IMPORTANT:** The format operation can take longer than the watchdog timeout (15 seconds) if many files exist.

**Current Implementation:**
- No watchdog reset during format
- May trigger watchdog timeout on large formats

**Recommended Enhancement** (future):
Add watchdog resets to `deleteRecursive()`:

```cpp
bool SDLogger::deleteRecursive(File dir, const char* path) {
    File file = dir.openNextFile();
    int fileCount = 0;
    while (file) {
        // Reset watchdog every 10 files
        if (++fileCount % 10 == 0) {
            esp_task_wdt_reset();
        }
        // ... rest of deletion logic
    }
}
```

## Troubleshooting

### Problem: Format command not recognized

**Symptoms:**
- Typing `FORMAT_SD_CARD` does nothing
- No warning message appears

**Solution:**
- Check firmware version (must be v1.3.0+)
- Re-upload firmware if needed
- Type `HELP` to verify command is listed

### Problem: Format confirmation not working

**Symptoms:**
- `FORMAT_SD_CARD_CONFIRM` shows error or does nothing
- Format doesn't execute

**Solution:**
- Must issue `FORMAT_SD_CARD` first
- Commands are case-sensitive
- Type exact command: `FORMAT_SD_CARD_CONFIRM`

### Problem: Format takes too long / watchdog timeout

**Symptoms:**
- Format starts but device resets
- "WATCHDOG TIMEOUT" message during format

**Solution:**
- SD card has too many files (>500)
- Remove SD card and format on computer
- Or wait for v1.3.1+ with watchdog resets

### Problem: Directory structure not recreated

**Symptoms:**
- Format succeeds but directories missing
- Logging fails after format

**Solution:**
- Issue `CHECK_SD_CARD` command
- Manually create directories via computer
- Report bug if persistent

## Integration with Other Features

### Hot-Swap Detection

After format:
- SD card is automatically re-detected
- `cardPresent` flag is set to true
- No manual `CHECK_SD_CARD` needed

### Memory Logging

After format:
- Memory logging continues normally
- New `memory_usage.csv` file created
- Previous data is lost

### Crash Dumps

After format:
- Crash dump functionality restored
- `/logs/crashes/` directory ready
- Previous crash data lost

### API Logging

After format:
- API logging continues normally
- New API log files created
- Previous API logs lost

## Best Practices

1. **Always Backup Important Data**
   - Never format without backing up critical logs
   - Use `CAPTURE_SCREEN` to save screenshots first
   - Transfer log files to computer before format

2. **Format During Maintenance Windows**
   - Don't format during active monitoring
   - Schedule format during low-activity periods
   - Coordinate with other system maintenance

3. **Verify Before and After**
   - Run `STATUS` before format to see current state
   - Run `STATUS` after format to verify success
   - Test logging immediately after format

4. **Document Format Operations**
   - Note date/time of format in external log
   - Record reason for format
   - Track format frequency for hardware assessment

5. **Monitor SD Card Health**
   - Frequent format needs may indicate card failure
   - Replace SD card if format needed monthly
   - Use quality SD cards (SanDisk, Samsung, etc.)

## Security Considerations

### Data Erasure

**Important:** The format operation does NOT perform secure erasure:
- Files are unlinked but data may remain on card
- Forensic recovery may be possible
- Not suitable for secure data destruction

For secure erasure:
- Remove SD card from device
- Use secure erase tool on computer (e.g., `dd`, `shred`)
- Or physically destroy SD card

### Access Control

Format command is accessible via serial port:
- Anyone with serial access can format
- No password protection
- Physical access = format capability

Consider:
- Limiting physical access to device
- Disabling serial commands in production
- Adding password protection (future enhancement)

## Future Enhancements

Potential improvements for future versions:

1. **Watchdog-Safe Format**
   - Add `esp_task_wdt_reset()` calls during delete
   - Handle large file counts gracefully

2. **Password Protection**
   - Require password for format confirmation
   - Configurable via `globalConfig`

3. **Backup Before Format**
   - Automatic backup to WiFi/HTTP server
   - Optional backup prompt

4. **Partial Format**
   - Format only specific directories
   - Preserve certain log types
   - Selective cleanup commands

5. **Format Scheduling**
   - Automatic format on schedule
   - Cleanup old files instead of full format
   - Configurable retention policies

## Related Documentation

- [SD Card Logging Guide](SD_LOGGER_GUIDE.md) - General SD logging documentation
- [v1.2.0 Release Notes](V1.2.0_RELEASE_NOTES.md) - Enhanced error handling features
- [Watchdog Implementation](WATCHDOG_IMPLEMENTATION.md) - Watchdog timer details

## Summary

The SD card format feature provides a safe, on-device method to completely erase and reinitialize the SD card. The two-step confirmation process and clear warnings help prevent accidental data loss. Always backup important data before formatting, and verify the operation succeeded using the `STATUS` command.

**Remember:** Format is permanent and irreversible. Use with caution!
