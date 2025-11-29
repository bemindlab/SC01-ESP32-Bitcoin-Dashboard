# Testing Crash Handler - Quick Guide

## Safe Testing (No Crash Required)

### 1. Verify Build
```bash
make build
```

Expected: Build succeeds with no errors

### 2. Check Serial Output on Boot
```bash
make monitor
```

Look for:
```
=== Crash Handler Initialization ===
Reset Reason: Power-on reset
Crash Handler initialized

=== Initializing Watchdog Timer (30 seconds) ===
✓ Watchdog timer enabled
```

### 3. Test LAST_CRASH Command
In Serial Monitor, type:
```
LAST_CRASH
```

Expected (no crashes):
```
=== Last Crash Information ===
No crash detected since last power-on reset
```

### 4. Check Settings Screen
1. Navigate to Settings (tap gear icon)
2. Look for "Crash Information" option
3. Should show green "NONE" status
4. Tap to view details - should say "No crashes detected"

## Crash Testing (OPTIONAL - Simulates Real Crash)

**⚠️ WARNING:** This will crash your device and create a reboot loop if not handled correctly!

### Method 1: Null Pointer Crash Test

1. Add test code to `src/main.cpp` in `loop()`:

```cpp
void loop() {
    crashHandler.feedWatchdog();
    
    // === TEST CODE - REMOVE AFTER TESTING ===
    static bool testDone = false;
    if (Serial.available() && !testDone) {
        String cmd = Serial.readStringUntil('\n');
        cmd.trim();
        if (cmd == "TEST_CRASH") {
            testDone = true;  // Prevent crash loop
            Serial.println("⚠️  Triggering test crash in 3 seconds...");
            Serial.println("Device will crash and reboot");
            delay(3000);
            
            // Force null pointer crash
            int* ptr = nullptr;
            *ptr = 42;  // CRASH HERE
        }
    }
    // === END TEST CODE ===
    
    processSerialCommand();
    // ... rest of loop ...
}
```

2. Build and upload:
```bash
make flash
```

3. Open Serial Monitor and type:
```
TEST_CRASH
```

4. Expected sequence:
```
⚠️  Triggering test crash in 3 seconds...
Device will crash and reboot

Guru Meditation Error: Core 0 panic'ed (LoadProhibited)
... stack trace ...
Rebooting...

=== Crash Handler Initialization ===
⚠️  CRASH RECOVERY DETECTED
Crash Count: 1
Last Crash: Exception/panic reset
Last Screen: Dashboard
✓ Crash dump saved: /logs/errors/crash_...log
```

5. Verify crash log on SD card:
```bash
# Remove SD card and check on computer
ls /logs/errors/
cat /logs/errors/crash_*.log
```

6. Use LAST_CRASH command:
```
LAST_CRASH

=== Last Crash Information ===
Crash Count: 1
Last Crash Reason: Exception/panic reset
Last Screen: Dashboard
Last API Call: (none or last API called)
Uptime at crash: X seconds
Watchdog timeout: NO
```

7. Check Settings screen:
   - "Crash Information" should show red "DETECTED"
   - Tap to see crash details

8. **IMPORTANT:** Remove test code after testing!

### Method 2: Watchdog Timeout Test

1. Add test code to `src/main.cpp`:

```cpp
void loop() {
    crashHandler.feedWatchdog();
    
    // === TEST CODE - REMOVE AFTER TESTING ===
    static bool testDone = false;
    if (Serial.available() && !testDone) {
        String cmd = Serial.readStringUntil('\n');
        cmd.trim();
        if (cmd == "TEST_WATCHDOG") {
            testDone = true;  // Prevent crash loop
            Serial.println("⚠️  Blocking main loop - watchdog will trigger in 30s");
            Serial.println("Device will reset after watchdog timeout");
            
            // Block forever - no watchdog feeding
            while(true) {
                delay(1000);
                Serial.print(".");
            }
        }
    }
    // === END TEST CODE ===
    
    processSerialCommand();
    // ... rest of loop ...
}
```

2. Build and upload:
```bash
make flash
```

3. Type in Serial Monitor:
```
TEST_WATCHDOG
```

4. Expected sequence:
```
⚠️  Blocking main loop - watchdog will trigger in 30s
Device will reset after watchdog timeout
.....................(30 seconds of dots)
Task watchdog got triggered...
Rebooting...

=== Crash Handler Initialization ===
⚠️  WATCHDOG TIMEOUT DETECTED
✓ Watchdog timeout logged: /logs/errors/watchdog_...log
```

5. Check watchdog log on SD card

6. **IMPORTANT:** Remove test code after testing!

## Production Testing (No Intentional Crashes)

### Monitor in Production
1. Check crash count periodically:
```
STATUS
```

2. If crashes detected, investigate:
```
LAST_CRASH
```

3. Review crash logs on SD card:
```bash
ls /logs/errors/
cat /logs/errors/crash_*.log
```

4. Settings screen shows visual indicator

### Clear Crash Info
Crash counter resets on power-on reset (not software reset):
1. Unplug power
2. Wait 5 seconds
3. Plug back in
4. Crash count should reset

## Troubleshooting Tests

### Test 1: Watchdog Working?
Stop feeding watchdog:
```cpp
// Comment out this line temporarily:
// crashHandler.feedWatchdog();
```

Expected: Watchdog triggers after 30 seconds

### Test 2: SD Card Logging?
1. Trigger test crash
2. Remove SD card
3. Check for crash log files
4. Read log content

### Test 3: Settings Screen?
1. Navigate to Settings
2. Tap "Crash Information"
3. Verify display matches LAST_CRASH output

## Testing Checklist

- [ ] Build succeeds without errors
- [ ] Serial output shows crash handler init
- [ ] Watchdog timer initializes successfully
- [ ] LAST_CRASH command responds (no crashes initially)
- [ ] Settings screen shows "Crash Information" option
- [ ] Crash status shows green "NONE" initially
- [ ] (Optional) Test crash creates crash log on SD card
- [ ] (Optional) Test crash shows in Settings screen
- [ ] (Optional) LAST_CRASH shows crash details after test
- [ ] (Optional) Watchdog timeout test works
- [ ] Test code removed before production use

## Production Deployment Checklist

- [ ] Remove ALL test crash code
- [ ] Verify watchdog timeout setting (30s recommended)
- [ ] SD card has sufficient space for crash logs
- [ ] Serial logging level appropriate (INFO or WARN)
- [ ] Settings screen accessible to users
- [ ] Documentation provided to users
- [ ] Crash log review process established

## Expected Crash Log Location

```
/logs/errors/crash_YYYY-MM-DD_HHMMSS.log
/logs/errors/watchdog_YYYY-MM-DD_HHMMSS.log
```

Example:
```
/logs/errors/crash_2025-11-29_143022-456.log
/logs/errors/watchdog_2025-11-29_143522-123.log
```

## Clean Test Environment

To start fresh testing:
1. Format SD card (or delete /logs/errors/)
2. Power cycle device (unplug power, not reset button)
3. Verify crash count is 0: `LAST_CRASH`
4. Verify Settings shows green "NONE"

## Success Criteria

✅ System boots without errors
✅ Watchdog initializes and runs
✅ No false crash detections
✅ Crash detection works (if tested)
✅ Crash logs created on SD card
✅ Settings screen displays crash info
✅ Serial commands work correctly
✅ No performance degradation
✅ Memory usage acceptable

## Questions?

See full documentation:
- `/docs/CRASH_HANDLER_IMPLEMENTATION.md`
- `/CRASH_HANDLER_SUMMARY.md`
