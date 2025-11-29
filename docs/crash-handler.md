# Crash Handler Implementation Guide

## Overview

The Bitcoin Dashboard ESP32-S3 now includes a comprehensive crash detection, logging, and recovery system with watchdog timer support. This system captures detailed crash information, persists it across resets, and provides diagnostic tools for debugging.

## Features

### 1. Crash Detection and Logging
- Automatic detection of crashes and panics
- Persistent crash information stored in RTC memory (survives software resets)
- Detailed crash dumps saved to SD card
- Stack trace capture and memory state logging

### 2. Watchdog Timer
- 30-second hardware watchdog timer
- Automatic system reset if main loop hangs
- Watchdog timeout logging for diagnostics

### 3. Crash Recovery
- Boot-time crash detection
- Automatic crash dump creation on recovery
- Crash count tracking across reboots
- Last crash information display in Settings screen

### 4. Context Tracking
- Current screen name tracking
- Last API call tracking
- WiFi connection state
- Memory usage at crash time

## Architecture

### Core Components

#### 1. CrashHandler (`src/utils/CrashHandler.h/cpp`)
**Purpose:** Main crash detection and recovery system

**Key Methods:**
- `begin()` - Initialize crash handler, detect previous crashes
- `initWatchdog(timeout)` - Start watchdog timer
- `feedWatchdog()` - Reset watchdog timer (call in main loop)
- `setCurrentScreen(name)` - Track current screen for crash context
- `setLastAPICall(service, endpoint)` - Track API calls for crash context
- `logCrashDump()` - Write detailed crash report to SD card
- `getCrashInfo()` - Retrieve crash information structure

**RTC Memory Persistence:**
```cpp
RTC_DATA_ATTR bool rtc_hasCrashed;
RTC_DATA_ATTR uint32_t rtc_crashCount;
RTC_DATA_ATTR char rtc_lastCrashReason[64];
RTC_DATA_ATTR char rtc_lastScreen[32];
RTC_DATA_ATTR char rtc_lastAPICall[128];
```

These variables persist across software resets, allowing crash detection after reboot.

#### 2. SDLogger Enhancements
**New Methods:**
- `logCrash(stackTrace)` - Basic crash logging (already existed)
- `logWatchdogCrash()` - Watchdog timeout logging (already existed)

#### 3. Main Application Integration
**Bootstrap Sequence:**
```cpp
void setup() {
    Serial.begin(115200);
    crashHandler.begin();           // FIRST: Detect crashes
    // ... SD card, display, WiFi init ...
    crashHandler.initWatchdog(30);  // LAST: Enable watchdog
}

void loop() {
    crashHandler.feedWatchdog();    // FIRST: Reset watchdog
    // ... main loop code ...
}
```

#### 4. Settings Screen Integration
- New "Crash Information" menu option
- Displays crash count and last crash details
- Visual indicators (red for crashes, green for no crashes)
- Links to SD card crash logs

## Crash Dump Format

### File Location
`/logs/errors/crash_YYYY-MM-DD_HHMMSS.log`

### Example Crash Dump
```
=== CRASH DUMP ===
Timestamp: 2025-11-29 14:30:22.456
Uptime at crash: 3845 seconds (1h 4m 5s)
Crash Count (lifetime): 3

--- Exception Info ---
Exception Type: Task watchdog reset
Reset Reason: Task watchdog reset

--- Memory State ---
Free Heap: 45678 bytes
Total Heap: 327680 bytes (86.1% used)
Free PSRAM: 2345678 bytes
Total PSRAM: 8388608 bytes (72.0% used)
Largest Free Heap Block: 32768 bytes
Min Free Heap (since boot): 38912 bytes

--- Network State ---
WiFi Status: Connected
IP Address: 192.168.1.100
RSSI: -54 dBm

--- Application Context ---
Last Screen: Dashboard
Last API Call: mempool.space /api/v1/prices
Time since last API: 10 seconds ago

--- CPU State ---
CPU Frequency: 240 MHz
Flash Size: 16777216 bytes
Flash Speed: 80 MHz
Sketch Size: 1125313 bytes
Free Sketch Space: 5428287 bytes

--- Troubleshooting ---
Common causes:
  - Null pointer dereference
  - Stack overflow (deep recursion)
  - Heap fragmentation / out of memory
  - Double free / use after free
  - Array bounds violation

Next steps:
  1. Check Serial Monitor for exception decoder output
  2. Review last API call and screen for issues
  3. Check memory usage patterns before crash
  4. Enable core dump for detailed stack trace
  5. Add debug logging around suspected code

=== END CRASH DUMP ===
```

### Watchdog Timeout Log Format
`/logs/errors/watchdog_YYYY-MM-DD_HHMMSS.log`

```
=== WATCHDOG TIMEOUT CRASH ===
Timestamp: 2025-11-29 14:35:10.123
Watchdog Timeout: 30 seconds
Uptime: 4012 seconds

--- Memory State ---
Free Heap: 42345 bytes (Total: 327680 bytes)
Free PSRAM: 2198765 bytes (Total: 8388608 bytes)
Largest Free Heap Block: 28672 bytes

--- CPU State ---
CPU Frequency: 240 MHz
Flash Size: 16777216 bytes
Flash Speed: 80 MHz

--- Application Context ---
Last Screen: Dashboard
Last API Call: mempool.space /api/mempool

--- Likely Cause ---
Main loop blocked for >30 seconds
Possible causes:
  - Long SD card write operations
  - Blocking HTTP requests without timeout
  - Infinite loops without yield()
  - Mutex deadlocks
  - Excessive delay() calls

--- Recommended Actions ---
1. Review recent code changes
2. Check serial output for stuck operations
3. Verify all HTTP requests have timeouts
4. Ensure SD writes are non-blocking
5. Add vTaskDelay() / yield() to long-running loops
6. Check for infinite while loops

=== END WATCHDOG TIMEOUT CRASH ===
```

## Serial Commands

### LAST_CRASH
View the most recent crash information via serial console.

**Usage:**
```
LAST_CRASH
```

**Output:**
```
=== Last Crash Information ===
Crash Count: 2
Last Crash Reason: Task watchdog reset
Last Screen: Dashboard
Last API Call: mempool.space /api/v1/prices
Uptime at crash: 3845 seconds
Watchdog timeout: YES

Check /logs/errors/ directory for detailed crash dumps
```

**No Crash Output:**
```
=== Last Crash Information ===
No crash detected since last power-on reset
```

### STATUS (Enhanced)
The existing STATUS command now shows crash information:
```
STATUS

WiFi: Connected
Free Heap: 45678 bytes
Uptime: 1234 seconds
Crash Count: 0
```

## Settings Screen UI

### Crash Information Option
Located in Settings menu as third option:

```
┌─────────────────────────────────────┐
│ < Settings                          │
├─────────────────────────────────────┤
│ Gemini API Key          [SET]       │
│ WiFi Settings           [CONFIGURED]│
│ Crash Information       [NONE] ✓    │  <- Green if no crashes
│ Update Intervals        [EDIT]      │
│ Reset Configuration     [DANGER]    │
│ Back to Dashboard                   │
└─────────────────────────────────────┘
```

If crashes detected:
```
│ Crash Information       [DETECTED] ✗│  <- Red if crashes detected
```

### Crash Details Screen
Tapping "Crash Information" shows:

**No Crashes:**
```
┌─────────────────────────────────────┐
│ Crash Information                   │
│                                     │
│ ✓ No crashes detected               │
│   System running normally           │
└─────────────────────────────────────┘
```

**With Crashes:**
```
┌─────────────────────────────────────┐
│ Crash Information                   │
│                                     │
│ ✗ Crash Count: 3                    │
│                                     │
│ Last Crash:                         │
│   Reason: Task watchdog reset       │
│   Screen: Dashboard                 │
│   Last API: mempool.space /api/...  │
│   Uptime: 1h 4m 5s                  │
│   ✗ Watchdog Timeout!               │
│                                     │
│ Check SD card logs for details:     │
│   /logs/errors/crash_*.log          │
└─────────────────────────────────────┘
```

## Testing and Debugging

### Testing Crash Detection

#### 1. Force a Crash (Null Pointer Dereference)
Add test code to trigger crash:
```cpp
void loop() {
    crashHandler.feedWatchdog();

    // Test crash - remove after testing!
    if (Serial.available()) {
        String cmd = Serial.readStringUntil('\n');
        if (cmd == "TEST_CRASH") {
            int* ptr = nullptr;
            *ptr = 42;  // This will crash
        }
    }

    // ... rest of loop ...
}
```

Expected behavior:
1. System crashes and resets
2. On reboot, crash handler detects previous crash
3. Crash dump created in `/logs/errors/crash_*.log`
4. Settings screen shows "DETECTED" in red
5. Serial command `LAST_CRASH` displays crash info

#### 2. Test Watchdog Timeout
Add blocking code:
```cpp
void loop() {
    crashHandler.feedWatchdog();

    // Test watchdog - remove after testing!
    if (Serial.available()) {
        String cmd = Serial.readStringUntil('\n');
        if (cmd == "TEST_WATCHDOG") {
            Serial.println("Blocking loop - watchdog will trigger in 30s");
            while(true) {
                // Block forever - no watchdog feeding
                delay(1000);
            }
        }
    }

    // ... rest of loop ...
}
```

Expected behavior:
1. After 30 seconds, watchdog triggers system reset
2. On reboot, watchdog timeout detected
3. Watchdog log created in `/logs/errors/watchdog_*.log`
4. Crash info shows "Watchdog timeout: YES"

### Debugging Real Crashes

#### 1. Enable Exception Decoder
PlatformIO automatically decodes stack traces. View in Serial Monitor:
```
Guru Meditation Error: Core 0 panic'ed (LoadProhibited)
Memory access exception, not enough data to decode
```

#### 2. Check SD Card Logs
```bash
# View crash logs
ls -la /logs/errors/

# Read most recent crash
cat /logs/errors/crash_2025-11-29_143022-456.log
```

#### 3. Serial Monitor Analysis
Look for patterns before crash:
- Last successful operation
- Memory warnings (heap < 20%)
- API timeout messages
- SD card write errors

#### 4. Use LAST_CRASH Command
Quick crash info without removing SD card:
```
LAST_CRASH
```

## Configuration

### Watchdog Timeout
Adjust in `main.cpp`:
```cpp
// Default: 30 seconds
crashHandler.initWatchdog(30);

// For slower operations, increase timeout:
crashHandler.initWatchdog(60);  // 60 seconds
```

**Recommendations:**
- Development: 60 seconds (allows debugging)
- Production: 30 seconds (faster recovery)
- With slow SD cards: 45 seconds

### Crash Log Retention
Crash logs are stored indefinitely. Manually clean old logs:
```cpp
// Add to serial commands if needed
if (command == "CLEANUP_CRASH_LOGS") {
    // Delete crash logs older than 30 days
    // Implementation: iterate /logs/errors/ and delete old files
}
```

## Memory Overhead

### Flash Memory
- CrashHandler.cpp: ~8KB
- CrashHandler.h: ~2KB
- **Total:** ~10KB flash

### RAM Usage
- Static variables: ~512 bytes
- RTC memory: ~256 bytes (persists across resets)
- Stack during crash dump: ~2KB (temporary)
- **Total:** ~3KB RAM

### SD Card Space
- Crash log: ~1-2KB per crash
- Watchdog log: ~1KB per timeout
- With 100 crashes: ~200KB

## Best Practices

### 1. Feed Watchdog Regularly
```cpp
void loop() {
    crashHandler.feedWatchdog();  // ALWAYS FIRST
    // ... rest of code ...
}
```

### 2. Track Screen Context
```cpp
void ScreenManager::switchScreen(ScreenType type) {
    // ... screen switching code ...

    // Update crash context
    crashHandler.setCurrentScreen(getScreenName(type));
}
```

### 3. Track API Calls
```cpp
bool fetchPrice() {
    crashHandler.setLastAPICall("mempool.space", "/api/v1/prices");

    HTTPClient http;
    http.begin("https://mempool.space/api/v1/prices");
    // ... HTTP request ...
}
```

### 4. Handle Long Operations
For operations > 10 seconds:
```cpp
void longOperation() {
    for (int i = 0; i < 1000; i++) {
        // Do work...

        // Feed watchdog periodically
        if (i % 100 == 0) {
            crashHandler.feedWatchdog();
        }
    }
}
```

### 5. Disable Watchdog for Debugging
```cpp
void setup() {
    // ... initialization ...

#ifdef DEBUG_MODE
    Serial.println("DEBUG MODE: Watchdog disabled");
    // Don't initialize watchdog
#else
    crashHandler.initWatchdog(30);
#endif
}
```

## Troubleshooting

### "Watchdog timer disabled" message
**Cause:** Watchdog initialization failed

**Solutions:**
1. Check ESP-IDF version compatibility
2. Verify `esp_task_wdt_init()` API call
3. Ensure no other code initializes watchdog

### Crash logs not created
**Cause:** SD card not available during crash recovery

**Solutions:**
1. Ensure SD card is inserted
2. Check SD card initialization before crash handler
3. Verify `/logs/errors/` directory exists

### False crash detection
**Cause:** RTC memory not cleared after power cycle

**Solutions:**
1. RTC memory is intentionally persistent
2. Power-on reset clears RTC memory
3. Deep sleep resets preserve crash info (intended)

### High memory usage warnings
**Cause:** Crash handler uses buffers for logging

**Solutions:**
1. Normal: ~3KB RAM overhead
2. During crash dump: temporary 2KB spike
3. Consider reducing buffer sizes if critical

## Future Enhancements

### Planned Features
1. **Core Dump Support** - Full register and stack dumps
2. **Crash Analytics** - Track crash patterns over time
3. **Remote Crash Reporting** - Upload crashes to server
4. **Crash Replay** - Recreate conditions leading to crash
5. **Auto-Recovery Actions** - Safe mode, factory reset options

### Configuration Options
1. Configurable crash log retention
2. Crash severity levels
3. Auto-cleanup of old crash logs
4. Crash notification LED patterns

## Version History

### v1.0 (2025-11-29)
- Initial crash handler implementation
- RTC memory persistence
- Watchdog timer support
- SD card crash logging
- Settings screen integration
- LAST_CRASH serial command

## Support and Debugging

### Serial Monitor Output
Enable verbose logging:
```cpp
// In main.cpp setup()
sdLogger.setLogLevel(LOG_DEBUG);
```

### Common Error Messages
```
✗ Watchdog initialization failed: -1
  → Check ESP-IDF compatibility

⚠️ SD card not ready, crash dump not saved
  → Insert SD card and restart

✗ Failed to create crash log: /logs/errors/crash_*.log
  → SD card full or write-protected
```

### Contact and Reporting
For crash-related issues:
1. Capture full serial output
2. Save crash logs from SD card
3. Note system conditions (WiFi, memory, uptime)
4. Report in GitHub issues with logs attached
