# SD Card Logging Feature Plan

## Overview

Add SD card logging functionality to the Bitcoin Dashboard to persistently store system logs, API data, error traces, and diagnostic information under a `/logs` directory on the SD card. This enables long-term monitoring, debugging, and data analysis without relying on serial console access.

**Target Version:** v1.1.0
**Priority:** Medium
**Complexity:** Medium-High
**Dependencies:** SD card library (ESP32 built-in), FAT filesystem support

---

## Feature Goals

1. **Persistent Logging** - Store all system events to SD card for later review
2. **Structured Log Files** - Organized directory structure with rotating log files
3. **Diagnostic Support** - Enable post-mortem debugging and long-term stability analysis
4. **Data Export** - Historical data logging for Bitcoin metrics (price, blocks, fees)
5. **Configurable Verbosity** - Log levels (DEBUG, INFO, WARN, ERROR, FATAL)
6. **Memory Efficient** - Minimize RAM usage with buffered writes
7. **No Performance Impact** - Asynchronous logging that doesn't block UI

---

## Hardware Requirements

### SD Card Interface

**Panlee SC01 Plus SD Card Pins (Verify in hardware documentation):**
- **CS (Chip Select):** GPIO ?? (to be determined)
- **MOSI (Data Out):** GPIO ??
- **MISO (Data In):** GPIO ??
- **SCK (Clock):** GPIO ??

**Note:** The SC01 Plus may or may not have an SD card slot. This needs hardware verification before implementation.

**Alternatives if no SD card:**
- Use SPIFFS/LittleFS on internal flash (limited to ~4MB)
- Add external SD card module via SPI
- Use external EEPROM for critical logs only

### Storage Capacity Planning

| Log Type | Est. Size/Hour | Daily Size | 7-Day Size | 30-Day Size |
|----------|---------------|------------|------------|-------------|
| System Logs (INFO) | 50 KB | 1.2 MB | 8.4 MB | 36 MB |
| Debug Logs (DEBUG) | 500 KB | 12 MB | 84 MB | 360 MB |
| API Data Logs | 100 KB | 2.4 MB | 16.8 MB | 72 MB |
| Error Traces | 10 KB | 240 KB | 1.7 MB | 7.2 MB |
| **Total (INFO + API + Errors)** | **160 KB** | **3.84 MB** | **26.9 MB** | **115.2 MB** |

**Recommended SD Card:** 1GB minimum, 4GB+ recommended for long-term logging

---

## Directory Structure

```
/logs/
├── system/
│   ├── boot_2025-11-28_123045.log      # Boot/startup logs
│   ├── system_2025-11-28.log           # Daily system logs (rotating)
│   └── system_2025-11-27.log
├── api/
│   ├── mempool_2025-11-28.log          # mempool.space API calls
│   ├── gemini_2025-11-28.log           # Gemini API calls
│   └── openai_2025-11-28.log           # OpenAI API calls
├── data/
│   ├── btc_price_2025-11-28.csv        # Historical price data
│   ├── btc_blocks_2025-11-28.csv       # Block data
│   └── btc_mempool_2025-11-28.csv      # Mempool statistics
├── errors/
│   ├── crash_2025-11-28_143022.log     # Crash dumps with stack traces
│   └── error_2025-11-28.log            # Error-level events
└── debug/
    └── debug_2025-11-28.log            # Debug-level logs (optional)
```

---

## Log File Formats

### System Logs (Text Format)

```
[2025-11-28 12:30:45.123] [INFO] WiFi connected: 192.168.1.100
[2025-11-28 12:30:46.456] [INFO] NTP sync successful
[2025-11-28 12:30:50.789] [INFO] BTC price updated: $95,420.50
[2025-11-28 12:31:00.012] [WARN] Mempool API timeout (10s exceeded)
[2025-11-28 12:31:05.345] [ERROR] JSON parsing failed: invalid block data
```

**Format:** `[timestamp] [level] message`

### API Logs (JSON Lines Format)

```jsonl
{"timestamp":"2025-11-28T12:30:50Z","endpoint":"/api/v1/prices","status":200,"duration_ms":245,"response_size":128}
{"timestamp":"2025-11-28T12:31:00Z","endpoint":"/api/mempool","status":500,"duration_ms":10000,"error":"timeout"}
```

### Data Logs (CSV Format)

**BTC Price Log:**
```csv
timestamp,price_usd,price_eur,source
2025-11-28T12:30:50Z,95420.50,89234.12,mempool.space
2025-11-28T12:31:20Z,95435.75,89248.30,mempool.space
```

**Block Data Log:**
```csv
timestamp,height,tx_count,size_bytes,weight,timestamp
2025-11-28T12:30:50Z,870542,3245,1987654,3954321,1732792250
```

---

## Implementation Plan

### Phase 1: SD Card Library Integration

**Tasks:**
1. ✅ Verify SC01 Plus hardware has SD card slot
2. Add SD library dependency to platformio.ini
3. Create SDLogger class in `src/utils/SDLogger.h`
4. Implement SD card initialization with error handling
5. Test SD card mount/unmount on boot
6. Add SD card status to STATUS serial command

**Files to Create:**
- `src/utils/SDLogger.h` - SD logging interface
- `src/utils/SDLogger.cpp` - SD logging implementation
- `test/native/test_sdlogger/` - Unit tests for SD logger

**platformio.ini Changes:**
```ini
lib_deps =
    lovyan03/LovyanGFX@^1.1.0
    bblanchon/ArduinoJson@^6.21.0
    https://github.com/strange-v/FT6X36.git
    SD  # ESP32 built-in SD library
```

### Phase 2: Core Logging Infrastructure

**Tasks:**
1. Implement log levels enum (DEBUG, INFO, WARN, ERROR, FATAL)
2. Create buffered write system to minimize SD writes
3. Implement log rotation (daily, size-based)
4. Add timestamp formatting (requires NTP sync)
5. Create log directory structure on first boot
6. Implement log retention policy (auto-delete old logs)

**Key Features:**
- **Buffered Writes:** 4KB RAM buffer, flush every 30 seconds or on ERROR+
- **Log Rotation:** New file daily at midnight, max 100MB per file
- **Retention:** Keep last 30 days of logs, auto-delete older files
- **Fail-Safe:** If SD full, disable logging (don't crash)

### Phase 3: System Event Logging

**Tasks:**
1. Add logging to boot sequence (WiFi connect, display init, config load)
2. Log all configuration changes (API keys set, WiFi credentials updated)
3. Log screen navigation events (Dashboard → Settings, etc.)
4. Log touch events (optional, DEBUG level only)
5. Log memory usage every 5 minutes (INFO level)
6. Log watchdog resets and exceptions

**Integration Points:**
- `main.cpp:setup()` - Boot logs
- `Config.cpp` - Configuration change logs
- `ScreenManager.cpp` - Navigation logs
- `main.cpp:loop()` - Periodic memory logs

### Phase 4: API Call Logging

**Tasks:**
1. Create API interceptor for all HTTP requests
2. Log API endpoint, status code, duration, response size
3. Log API errors with full error messages
4. Log rate limiting events
5. Create separate log files per API (mempool, Gemini, OpenAI)

**Example API Log Entry:**
```cpp
sdLogger.logAPI("mempool.space", "/api/v1/prices", 200, 245, 128);
sdLogger.logAPIError("mempool.space", "/api/mempool", 500, "timeout after 10s");
```

### Phase 5: Data Export & Historical Logs

**Tasks:**
1. Log BTC price every time it updates (CSV format)
2. Log block data on each new block (CSV format)
3. Log mempool statistics every 5 minutes (CSV format)
4. Create data export function (copy CSV to USB via serial?)
5. Add data visualization support (import CSV to Excel/Python)

**Data Retention:**
- Price data: Keep 90 days (5-minute intervals = ~26K records = 1.3MB)
- Block data: Keep all blocks (low volume, ~144/day = 5MB/year)
- Mempool data: Keep 30 days (5-minute intervals = 8.6K records = 430KB)

### Phase 6: Error & Crash Logging

**Tasks:**
1. Implement exception handler that writes to SD before reboot
2. Log crash dumps with stack traces
3. Log memory allocation failures
4. Log WiFi disconnections and reconnect attempts
5. Log JSON parsing errors with raw response snippets
6. Create crash recovery UI notification ("Recovered from crash - logs saved")

**Crash Log Example:**
```
[2025-11-28 14:30:22.456] [FATAL] Exception detected
Stack Trace:
  0x400d1234: setup() at main.cpp:95
  0x400d5678: loop() at main.cpp:150
  0x400dabcd: lv_task_handler() at lvgl.c:1234
Heap Free: 45678 bytes
PSRAM Free: 2345678 bytes
Uptime: 3 hours 45 minutes
Last API Call: mempool.space /api/mempool (10s ago)
```

### Phase 7: Configuration & UI Integration

**Tasks:**
1. Add SD logging enable/disable setting to Config
2. Add log level configuration (DEBUG/INFO/WARN/ERROR)
3. Add SD card status indicator to Settings screen
4. Create log viewer screen (optional, advanced feature)
5. Add serial commands: `LOG_LEVEL=DEBUG`, `SHOW_LOGS`, `CLEAR_LOGS`
6. Add "Export Logs" function via serial (dump logs to console)

**Settings Screen Updates:**
- SD Card Status: ✓ Mounted (7.2GB free) / ✗ Not detected
- Logging Enabled: [Toggle]
- Log Level: [Dropdown: DEBUG/INFO/WARN/ERROR]
- Log Retention: [7 days / 30 days / 90 days]

### Phase 8: Testing & Validation

**Tasks:**
1. Unit tests for SDLogger class (mock SD library)
2. Test log rotation at midnight
3. Test SD card removal/insertion while running
4. Test with full SD card (graceful degradation)
5. Test crash recovery and dump capture
6. Verify log file integrity after power loss
7. Performance testing (ensure no UI lag during logging)
8. Long-term stability test (7+ days with logging enabled)

---

## Technical Implementation Details

### SDLogger Class Interface

```cpp
// src/utils/SDLogger.h
#ifndef SDLOGGER_H
#define SDLOGGER_H

#include <SD.h>
#include <Arduino.h>

enum LogLevel {
    LOG_DEBUG = 0,
    LOG_INFO = 1,
    LOG_WARN = 2,
    LOG_ERROR = 3,
    LOG_FATAL = 4
};

class SDLogger {
public:
    SDLogger();
    ~SDLogger();

    // Initialization
    bool begin(int csPin);
    bool isReady();

    // Core logging
    void log(LogLevel level, const char* message);
    void logf(LogLevel level, const char* format, ...);

    // Specialized logging
    void logAPI(const char* service, const char* endpoint, int status, long duration_ms, size_t response_size);
    void logAPIError(const char* service, const char* endpoint, int status, const char* error);
    void logData(const char* filename, const char* csvLine);
    void logCrash(const char* stackTrace);

    // Configuration
    void setLogLevel(LogLevel level);
    void setBufferFlushInterval(unsigned long ms);
    void setRetentionDays(int days);

    // Maintenance
    void flush();  // Force write buffer to SD
    void rotate(); // Create new log file (called daily)
    void cleanup(); // Delete old logs per retention policy

    // Status
    size_t getFreeSpace();
    size_t getLogSize();
    int getLogFileCount();

private:
    File currentLogFile;
    char logBuffer[4096];
    size_t bufferPos;
    LogLevel currentLevel;
    unsigned long lastFlush;
    unsigned long flushInterval;
    int retentionDays;

    void writeBuffer();
    const char* getLevelString(LogLevel level);
    String getTimestamp();
    String formatLogLine(LogLevel level, const char* message);
};

extern SDLogger sdLogger;

// Convenience macros
#define LOG_DEBUG(msg) sdLogger.log(LOG_DEBUG, msg)
#define LOG_INFO(msg) sdLogger.log(LOG_INFO, msg)
#define LOG_WARN(msg) sdLogger.log(LOG_WARN, msg)
#define LOG_ERROR(msg) sdLogger.log(LOG_ERROR, msg)
#define LOG_FATAL(msg) sdLogger.log(LOG_FATAL, msg)

#endif
```

### Usage Examples

```cpp
// In main.cpp setup()
if (!sdLogger.begin(SD_CS_PIN)) {
    Serial.println("SD card initialization failed - logging disabled");
} else {
    sdLogger.setLogLevel(LOG_INFO);
    sdLogger.setRetentionDays(30);
    LOG_INFO("Bitcoin Dashboard started");
}

// In API calls
unsigned long startTime = millis();
int httpCode = http.GET();
unsigned long duration = millis() - startTime;
if (httpCode == 200) {
    sdLogger.logAPI("mempool.space", "/api/v1/prices", httpCode, duration, http.getSize());
} else {
    sdLogger.logAPIError("mempool.space", "/api/v1/prices", httpCode, "HTTP request failed");
}

// In data updates
char csvLine[256];
snprintf(csvLine, sizeof(csvLine), "%s,%.2f,%.2f,mempool.space",
         sdLogger.getTimestamp().c_str(), btcData.priceUSD, btcData.priceEUR);
sdLogger.logData("/logs/data/btc_price.csv", csvLine);

// In exception handler
void onCrash() {
    char crashLog[1024];
    snprintf(crashLog, sizeof(crashLog),
             "Stack: 0x%08X\nHeap: %d bytes\nUptime: %lu seconds",
             (uint32_t)__builtin_return_address(0), ESP.getFreeHeap(), millis() / 1000);
    sdLogger.logCrash(crashLog);
    sdLogger.flush(); // Ensure written before reboot
}
```

---

## Memory & Performance Impact

### RAM Usage
- **Log Buffer:** 4KB (configurable)
- **File Handle:** ~100 bytes
- **Timestamp String:** ~32 bytes
- **Total:** ~4.2KB additional RAM (0.13% of 320KB)

### Flash Usage
- **SDLogger Library:** ~15-20KB
- **SD Library:** ~30KB (ESP32 built-in)
- **Total:** ~50KB additional flash (0.3% of 16MB)

### Performance
- **Buffered Writes:** No UI blocking, writes happen in background
- **Flush Frequency:** Every 30 seconds or on ERROR+ (configurable)
- **CPU Overhead:** <1% (minimal sprintf and string operations)
- **SD Write Speed:** ~1-2ms per flush (4KB buffer)

---

## Configuration Options

### Config.h Additions

```cpp
// SD logging settings
bool sdLoggingEnabled = true;
int sdLogLevel = LOG_INFO; // 0=DEBUG, 1=INFO, 2=WARN, 3=ERROR, 4=FATAL
int sdRetentionDays = 30;
bool sdLogAPIData = true;
bool sdLogPriceData = true;
```

### Serial Commands

```
SET_LOG_LEVEL=DEBUG     # Set log verbosity (DEBUG/INFO/WARN/ERROR/FATAL)
GET_LOG_LEVEL           # Show current log level
SD_STATUS               # Show SD card status and log statistics
FLUSH_LOGS              # Force flush buffer to SD
CLEAR_LOGS              # Delete all log files
EXPORT_LOGS             # Dump recent logs to serial console
```

---

## Error Handling & Edge Cases

### SD Card Failure Scenarios

1. **No SD Card Detected:**
   - Disable logging silently
   - Log to Serial only
   - Show "SD: Not detected" in Settings screen

2. **SD Card Full:**
   - Stop logging new data
   - Keep system running normally
   - Show warning in Settings: "SD: Full (0 bytes free)"
   - Auto-cleanup old logs if retention policy enabled

3. **SD Card Removed During Operation:**
   - Detect on next flush attempt
   - Disable logging
   - Show "SD: Removed" in Settings
   - Auto-resume if card re-inserted

4. **SD Card Write Error:**
   - Retry once
   - If retry fails, disable logging
   - Log error to Serial
   - Continue normal operation

5. **Corrupted Filesystem:**
   - Attempt to reformat (user confirmation required)
   - If reformat fails, disable SD logging
   - System continues without SD

### Fail-Safe Design Principles

- **Never crash due to SD errors** - Always degrade gracefully
- **SD logging is optional** - Core dashboard works without it
- **Buffer flushes on critical errors** - Don't lose important data
- **Auto-recovery** - Re-enable logging if SD becomes available

---

## Testing Strategy

### Unit Tests

```cpp
// test/native/test_sdlogger/test_sdlogger.cpp
#include <unity.h>
#include "utils/SDLogger.h"
#include "mocks/MockSD.h"

void test_logger_initialization() {
    MockSD mockSD;
    SDLogger logger;
    TEST_ASSERT_TRUE(logger.begin(SD_CS_PIN));
    TEST_ASSERT_TRUE(logger.isReady());
}

void test_log_level_filtering() {
    SDLogger logger;
    logger.setLogLevel(LOG_WARN);
    logger.log(LOG_INFO, "This should be filtered");
    logger.log(LOG_ERROR, "This should be logged");
    // Verify only ERROR was written to mock file
}

void test_log_rotation() {
    SDLogger logger;
    // Simulate 24 hours passing
    logger.rotate();
    // Verify new log file created
}

void test_buffer_flush() {
    SDLogger logger;
    for (int i = 0; i < 100; i++) {
        logger.logf(LOG_INFO, "Test message %d", i);
    }
    // Verify buffer flushed when full
}
```

### Hardware Integration Tests

1. **SD Card Stress Test:**
   - Write 1GB of logs continuously
   - Verify no memory leaks
   - Verify filesystem integrity

2. **Power Loss Test:**
   - Write logs continuously
   - Pull power randomly
   - Verify no corruption, last flush recovered

3. **Performance Test:**
   - Log at DEBUG level for 1 hour
   - Measure UI responsiveness
   - Verify <1% CPU overhead

4. **Long-Term Stability Test:**
   - Run for 7+ days with logging enabled
   - Verify log rotation works
   - Verify retention policy deletes old logs
   - Check for memory leaks

---

## Documentation Updates Required

1. **README.md:**
   - Add SD card logging section
   - Document serial commands for log management

2. **docs/guides/debugging-guide.md:**
   - Add section on retrieving logs from SD card
   - Document log file formats

3. **docs/configuration-system-guide.md:**
   - Document SD logging configuration options

4. **docs/features/sd-card-logging-plan.md:**
   - This document (feature specification)

5. **docs/hardware-setup.md (new):**
   - Document SD card slot location (if available)
   - Pin assignments for SD interface
   - SD card formatting requirements (FAT32, exFAT)

---

## Future Enhancements (v2.0+)

1. **Log Viewer Screen (LVGL UI):**
   - Browse logs on device
   - Filter by date/level
   - Touch to scroll through logs

2. **Remote Log Upload:**
   - MQTT publish of ERROR/FATAL logs
   - HTTP POST logs to remote server
   - Email critical errors via SMTP

3. **Log Analytics:**
   - On-device log analysis (parse for patterns)
   - Uptime statistics display
   - API success rate charts

4. **Compression:**
   - GZIP old log files to save space
   - Automatic compression of logs >7 days old

5. **CSV Export via USB:**
   - USB Mass Storage mode
   - Expose SD card as USB drive
   - Easy data retrieval without removing SD card

---

## Success Criteria

**MVP (Minimum Viable Product) - v1.1.0:**
- ✅ SD card initialization on boot
- ✅ System logs (INFO level) written to `/logs/system/`
- ✅ API call logs written to `/logs/api/`
- ✅ Error logs written to `/logs/errors/`
- ✅ Daily log rotation working
- ✅ 30-day retention policy enforced
- ✅ SD card status in Settings screen
- ✅ Serial commands for log management
- ✅ No performance degradation (<1% CPU overhead)

**Full Feature Set - v1.2.0:**
- ✅ Historical data logging (CSV format)
- ✅ Crash dumps with stack traces
- ✅ Configurable log levels
- ✅ Export logs via serial
- ✅ Auto-cleanup of old logs
- ✅ 7-day stability testing passed

**Advanced Features - v2.0.0:**
- Log viewer UI screen
- Remote log upload (MQTT/HTTP)
- USB Mass Storage mode
- Log compression

---

## Risk Assessment

| Risk | Likelihood | Impact | Mitigation |
|------|-----------|--------|------------|
| SC01 Plus has no SD slot | High | High | Verify hardware first, use SPIFFS fallback |
| SD card failures in production | Medium | Low | Graceful degradation, continue without logging |
| Performance impact on UI | Low | Medium | Buffered writes, async logging |
| SD card full during operation | Medium | Low | Auto-cleanup, user notification |
| Log data privacy concerns | Low | Medium | Document what's logged, no sensitive data |
| Corrupted SD filesystem | Medium | Low | Auto-reformat option, fail-safe design |

---

## Dependencies & Prerequisites

**Hardware:**
- ✅ ESP32-S3 with SPI interface (available)
- ⏳ SD card slot on SC01 Plus (to be verified)
- ⏳ SD card (1GB+ recommended)

**Software:**
- ✅ ESP32 SD library (built-in)
- ✅ FAT filesystem support (built-in)
- ⏳ NTP time sync (for timestamps) - currently not implemented
- ⏳ RTC support (optional, for timestamps without WiFi)

**Prerequisites:**
1. Verify SC01 Plus has SD card slot (check hardware documentation)
2. Document SD card pin assignments
3. Implement NTP time synchronization (for accurate timestamps)
4. Test SD library compatibility with ESP32-S3

---

## Implementation Timeline

**Sprint 1 (Week 1-2): Research & Foundation**
- Verify SC01 Plus SD card hardware capabilities
- Create SDLogger class skeleton
- Implement basic SD initialization
- Add unit tests

**Sprint 2 (Week 3-4): Core Logging**
- Implement buffered writes
- Add log levels and filtering
- Create log directory structure
- Test on hardware

**Sprint 3 (Week 5-6): Integration**
- Add logging to main.cpp (boot, loop)
- Add logging to API calls
- Add logging to screen navigation
- Test logging overhead

**Sprint 4 (Week 7-8): Advanced Features**
- Implement log rotation
- Add retention policy
- Create crash dump handler
- Test long-term stability

**Sprint 5 (Week 9-10): Polish & Testing**
- Add Settings screen UI
- Implement serial commands
- Documentation updates
- Final testing and bug fixes

**Total Estimated Time:** 10 weeks (part-time development)

---

## Conclusion

SD card logging will significantly enhance the Bitcoin Dashboard's debugging capabilities, long-term stability monitoring, and data export functionality. The implementation prioritizes:

1. **Reliability** - Fail-safe design that never crashes
2. **Performance** - <1% overhead, no UI blocking
3. **Usability** - Clear log formats, easy export
4. **Maintainability** - Auto-rotation, auto-cleanup

**Next Steps:**
1. ✅ Create this feature plan document
2. ⏳ Verify SC01 Plus SD card hardware
3. ⏳ Update development-plan.md with SD logging phase
4. ⏳ Begin implementation in Sprint 1

**Approval Required:** Hardware verification before starting implementation
