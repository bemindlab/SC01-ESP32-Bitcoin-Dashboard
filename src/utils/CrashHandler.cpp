#include "CrashHandler.h"
#include "SDLogger.h"
#include <esp_system.h>

// Global instance
CrashHandler crashHandler;

// RTC memory initialization (persists across software resets)
RTC_DATA_ATTR bool CrashHandler::rtc_hasCrashed = false;
RTC_DATA_ATTR uint32_t CrashHandler::rtc_crashCount = 0;
RTC_DATA_ATTR uint32_t CrashHandler::rtc_lastCrashTime = 0;
RTC_DATA_ATTR uint32_t CrashHandler::rtc_lastCrashUptime = 0;
RTC_DATA_ATTR char CrashHandler::rtc_lastCrashReason[64] = {0};
RTC_DATA_ATTR char CrashHandler::rtc_lastScreen[32] = {0};
RTC_DATA_ATTR char CrashHandler::rtc_lastAPICall[128] = {0};
RTC_DATA_ATTR uint32_t CrashHandler::rtc_lastAPITime = 0;
RTC_DATA_ATTR bool CrashHandler::rtc_watchdogTimeout = false;

CrashHandler::CrashHandler() {
    watchdogEnabled = false;
    watchdogTimeout = 30;
    memset(&crashInfo, 0, sizeof(crashInfo));
    memset(currentScreen, 0, sizeof(currentScreen));
    memset(lastAPIService, 0, sizeof(lastAPIService));
    memset(lastAPIEndpoint, 0, sizeof(lastAPIEndpoint));
    lastAPICallTime = 0;
}

void CrashHandler::begin() {
    Serial.println("\n=== Crash Handler Initialization ===");

    // Load crash info from RTC memory (survives software resets)
    loadCrashInfoFromRTC();

    // Get reset reason
    esp_reset_reason_t reason = esp_reset_reason();
    const char* resetReasonStr = getResetReason();

    Serial.printf("Reset Reason: %s\n", resetReasonStr);

    // Check if this was a crash recovery
    if (crashInfo.hasCrashed) {
        Serial.println("\n⚠️  CRASH RECOVERY DETECTED");
        Serial.printf("Crash Count: %u\n", crashInfo.crashCount);
        Serial.printf("Last Crash: %s\n", crashInfo.lastCrashReason);
        Serial.printf("Last Screen: %s\n", crashInfo.lastScreen);
        Serial.printf("Last API: %s\n", crashInfo.lastAPICall);
        Serial.printf("Uptime at crash: %u seconds\n", crashInfo.lastCrashUptime / 1000);

        // Log crash dump if SD card is available
        delay(1000); // Give SD card time to initialize
        logCrashDump();

        // Clear crash flag after logging
        rtc_hasCrashed = false;
        crashInfo.hasCrashed = false;
    }

    // Check for watchdog timeout
    if (reason == ESP_RST_TASK_WDT || reason == ESP_RST_WDT) {
        Serial.println("⚠️  WATCHDOG TIMEOUT DETECTED");
        rtc_watchdogTimeout = true;
        crashInfo.watchdogTimeout = true;
        logWatchdogTimeout();
    }

    Serial.println("Crash Handler initialized");
}

void CrashHandler::initWatchdog(uint32_t timeoutSeconds) {
    watchdogTimeout = timeoutSeconds;

    Serial.printf("\n=== Initializing Watchdog Timer (%u seconds) ===\n", timeoutSeconds);

    // Initialize Task Watchdog Timer (older ESP-IDF API)
    esp_err_t err = esp_task_wdt_init(timeoutSeconds, true); // timeout in seconds, panic on timeout
    if (err == ESP_OK) {
        // Subscribe current task to watchdog
        err = esp_task_wdt_add(NULL);
        if (err == ESP_OK) {
            watchdogEnabled = true;
            Serial.println("✓ Watchdog timer enabled");
            sdLogger.logf(LOG_INFO, "Watchdog timer initialized: %u second timeout", timeoutSeconds);
        } else {
            Serial.printf("✗ Failed to subscribe to watchdog: %d\n", err);
        }
    } else {
        Serial.printf("✗ Watchdog initialization failed: %d\n", err);
    }
}

void CrashHandler::feedWatchdog() {
    if (watchdogEnabled) {
        esp_task_wdt_reset();
    }
}

void CrashHandler::disableWatchdog() {
    if (watchdogEnabled) {
        esp_task_wdt_delete(NULL);
        // Note: esp_task_wdt_deinit() may not be available in all ESP-IDF versions
        watchdogEnabled = false;
        Serial.println("Watchdog timer disabled");
        sdLogger.log(LOG_INFO, "Watchdog timer disabled");
    }
}

bool CrashHandler::wasResetByCrash() {
    esp_reset_reason_t reason = esp_reset_reason();
    return (reason == ESP_RST_PANIC ||
            reason == ESP_RST_INT_WDT ||
            reason == ESP_RST_TASK_WDT ||
            reason == ESP_RST_WDT);
}

bool CrashHandler::wasResetByWatchdog() {
    esp_reset_reason_t reason = esp_reset_reason();
    return (reason == ESP_RST_TASK_WDT || reason == ESP_RST_WDT);
}

const char* CrashHandler::getResetReason() {
    esp_reset_reason_t reason = esp_reset_reason();

    switch (reason) {
        case ESP_RST_UNKNOWN:   return "Unknown reset";
        case ESP_RST_POWERON:   return "Power-on reset";
        case ESP_RST_EXT:       return "External reset";
        case ESP_RST_SW:        return "Software reset";
        case ESP_RST_PANIC:     return "Exception/panic reset";
        case ESP_RST_INT_WDT:   return "Interrupt watchdog reset";
        case ESP_RST_TASK_WDT:  return "Task watchdog reset";
        case ESP_RST_WDT:       return "Watchdog reset";
        case ESP_RST_DEEPSLEEP: return "Deep sleep reset";
        case ESP_RST_BROWNOUT:  return "Brownout reset";
        case ESP_RST_SDIO:      return "SDIO reset";
        default:                return "Other reset";
    }
}

CrashInfo CrashHandler::getCrashInfo() {
    return crashInfo;
}

void CrashHandler::setCurrentScreen(const char* screenName) {
    strncpy(currentScreen, screenName, sizeof(currentScreen) - 1);
    currentScreen[sizeof(currentScreen) - 1] = '\0';

    // Save to RTC memory for crash context
    strncpy(rtc_lastScreen, screenName, sizeof(rtc_lastScreen) - 1);
    rtc_lastScreen[sizeof(rtc_lastScreen) - 1] = '\0';
}

void CrashHandler::setLastAPICall(const char* service, const char* endpoint) {
    strncpy(lastAPIService, service, sizeof(lastAPIService) - 1);
    lastAPIService[sizeof(lastAPIService) - 1] = '\0';

    strncpy(lastAPIEndpoint, endpoint, sizeof(lastAPIEndpoint) - 1);
    lastAPIEndpoint[sizeof(lastAPIEndpoint) - 1] = '\0';

    lastAPICallTime = millis();

    // Save to RTC memory
    snprintf(rtc_lastAPICall, sizeof(rtc_lastAPICall), "%s %s", service, endpoint);
    rtc_lastAPITime = millis();
}

void CrashHandler::logCrashDump() {
    if (!sdLogger.isReady()) {
        Serial.println("⚠️  SD card not ready, crash dump not saved");
        return;
    }

    // Create detailed crash dump
    String timestamp = sdLogger.getTimestamp();
    String crashLog = "/logs/errors/crash_" + timestamp + ".log";
    crashLog.replace(" ", "_");
    crashLog.replace(":", "-");
    crashLog.replace(".", "-");

    File logFile = SD.open(crashLog.c_str(), FILE_WRITE);
    if (!logFile) {
        Serial.printf("✗ Failed to create crash log: %s\n", crashLog.c_str());
        return;
    }

    // Write crash dump
    logFile.println("=== CRASH DUMP ===");
    logFile.printf("Timestamp: %s\n", timestamp.c_str());
    logFile.printf("Uptime at crash: %u seconds (%uh %um %us)\n",
                   crashInfo.lastCrashUptime / 1000,
                   (crashInfo.lastCrashUptime / 1000) / 3600,
                   ((crashInfo.lastCrashUptime / 1000) % 3600) / 60,
                   (crashInfo.lastCrashUptime / 1000) % 60);
    logFile.printf("Crash Count (lifetime): %u\n", crashInfo.crashCount);
    logFile.println();

    // Exception info
    logFile.println("--- Exception Info ---");
    logFile.printf("Exception Type: %s\n", crashInfo.lastCrashReason);
    logFile.printf("Reset Reason: %s\n", getResetReason());
    logFile.println();

    // Memory state at crash
    logFile.println("--- Memory State ---");
    logFile.printf("Free Heap: %u bytes\n", ESP.getFreeHeap());
    logFile.printf("Total Heap: %u bytes (%.1f%% used)\n",
                   ESP.getHeapSize(),
                   ((ESP.getHeapSize() - ESP.getFreeHeap()) * 100.0) / ESP.getHeapSize());
    logFile.printf("Free PSRAM: %u bytes\n", ESP.getFreePsram());
    logFile.printf("Total PSRAM: %u bytes (%.1f%% used)\n",
                   ESP.getPsramSize(),
                   ((ESP.getPsramSize() - ESP.getFreePsram()) * 100.0) / ESP.getPsramSize());
    logFile.printf("Largest Free Heap Block: %u bytes\n", ESP.getMaxAllocHeap());
    logFile.printf("Min Free Heap (since boot): %u bytes\n", ESP.getMinFreeHeap());
    logFile.println();

    // WiFi status
    logFile.println("--- Network State ---");
    logFile.printf("WiFi Status: %s\n", getWiFiStatus().c_str());
    if (WiFi.status() == WL_CONNECTED) {
        logFile.printf("IP Address: %s\n", WiFi.localIP().toString().c_str());
        logFile.printf("RSSI: %d dBm\n", WiFi.RSSI());
    }
    logFile.println();

    // Application context
    logFile.println("--- Application Context ---");
    logFile.printf("Last Screen: %s\n", crashInfo.lastScreen);
    logFile.printf("Last API Call: %s\n", crashInfo.lastAPICall);
    if (crashInfo.lastAPITime > 0) {
        uint32_t apiAgo = (crashInfo.lastCrashUptime - crashInfo.lastAPITime) / 1000;
        logFile.printf("Time since last API: %u seconds ago\n", apiAgo);
    }
    logFile.println();

    // CPU info
    logFile.println("--- CPU State ---");
    logFile.printf("CPU Frequency: %u MHz\n", ESP.getCpuFreqMHz());
    logFile.printf("Flash Size: %u bytes\n", ESP.getFlashChipSize());
    logFile.printf("Flash Speed: %u MHz\n", ESP.getFlashChipSpeed() / 1000000);
    logFile.printf("Sketch Size: %u bytes\n", ESP.getSketchSize());
    logFile.printf("Free Sketch Space: %u bytes\n", ESP.getFreeSketchSpace());
    logFile.println();

    // Recommendations
    logFile.println("--- Troubleshooting ---");
    logFile.println("Common causes:");
    logFile.println("  - Null pointer dereference");
    logFile.println("  - Stack overflow (deep recursion)");
    logFile.println("  - Heap fragmentation / out of memory");
    logFile.println("  - Double free / use after free");
    logFile.println("  - Array bounds violation");
    logFile.println();
    logFile.println("Next steps:");
    logFile.println("  1. Check Serial Monitor for exception decoder output");
    logFile.println("  2. Review last API call and screen for issues");
    logFile.println("  3. Check memory usage patterns before crash");
    logFile.println("  4. Enable core dump for detailed stack trace");
    logFile.println("  5. Add debug logging around suspected code");
    logFile.println();

    logFile.println("=== END CRASH DUMP ===");
    logFile.close();

    Serial.printf("✓ Crash dump saved: %s\n", crashLog.c_str());
    sdLogger.logf(LOG_FATAL, "Crash dump created: %s", crashLog.c_str());
}

void CrashHandler::logWatchdogTimeout() {
    if (!sdLogger.isReady()) {
        Serial.println("⚠️  SD card not ready, watchdog timeout not logged");
        return;
    }

    String timestamp = sdLogger.getTimestamp();
    String crashLog = "/logs/errors/watchdog_" + timestamp + ".log";
    crashLog.replace(" ", "_");
    crashLog.replace(":", "-");
    crashLog.replace(".", "-");

    File logFile = SD.open(crashLog.c_str(), FILE_WRITE);
    if (!logFile) {
        Serial.printf("✗ Failed to create watchdog log: %s\n", crashLog.c_str());
        return;
    }

    logFile.println("=== WATCHDOG TIMEOUT CRASH ===");
    logFile.printf("Timestamp: %s\n", timestamp.c_str());
    logFile.printf("Watchdog Timeout: %u seconds\n", watchdogTimeout);
    logFile.printf("Uptime: %u seconds\n", millis() / 1000);
    logFile.println();

    logFile.println("--- Memory State ---");
    logFile.printf("Free Heap: %u bytes (Total: %u bytes)\n",
                   ESP.getFreeHeap(), ESP.getHeapSize());
    logFile.printf("Free PSRAM: %u bytes (Total: %u bytes)\n",
                   ESP.getFreePsram(), ESP.getPsramSize());
    logFile.printf("Largest Free Heap Block: %u bytes\n", ESP.getMaxAllocHeap());
    logFile.println();

    logFile.println("--- CPU State ---");
    logFile.printf("CPU Frequency: %u MHz\n", ESP.getCpuFreqMHz());
    logFile.printf("Flash Size: %u bytes\n", ESP.getFlashChipSize());
    logFile.printf("Flash Speed: %u MHz\n", ESP.getFlashChipSpeed() / 1000000);
    logFile.println();

    logFile.println("--- Application Context ---");
    logFile.printf("Last Screen: %s\n", rtc_lastScreen);
    logFile.printf("Last API Call: %s\n", rtc_lastAPICall);
    logFile.println();

    logFile.println("--- Likely Cause ---");
    logFile.printf("Main loop blocked for >%u seconds\n", watchdogTimeout);
    logFile.println("Possible causes:");
    logFile.println("  - Long SD card write operations");
    logFile.println("  - Blocking HTTP requests without timeout");
    logFile.println("  - Infinite loops without yield()");
    logFile.println("  - Mutex deadlocks");
    logFile.println("  - Excessive delay() calls");
    logFile.println();

    logFile.println("--- Recommended Actions ---");
    logFile.println("1. Review recent code changes");
    logFile.println("2. Check serial output for stuck operations");
    logFile.println("3. Verify all HTTP requests have timeouts");
    logFile.println("4. Ensure SD writes are non-blocking");
    logFile.println("5. Add vTaskDelay() / yield() to long-running loops");
    logFile.println("6. Check for infinite while loops");
    logFile.println();

    logFile.println("=== END WATCHDOG TIMEOUT CRASH ===");
    logFile.close();

    Serial.printf("✓ Watchdog timeout logged: %s\n", crashLog.c_str());
    sdLogger.logf(LOG_FATAL, "Watchdog timeout crash: %s", crashLog.c_str());
}

String CrashHandler::getLastCrashLog() {
    if (!crashInfo.hasCrashed) {
        return "No recent crash detected";
    }

    String info = "=== Last Crash ===\n";
    info += "Reason: " + String(crashInfo.lastCrashReason) + "\n";
    info += "Screen: " + String(crashInfo.lastScreen) + "\n";
    info += "API: " + String(crashInfo.lastAPICall) + "\n";
    info += "Uptime: " + String(crashInfo.lastCrashUptime / 1000) + "s\n";
    info += "Count: " + String(crashInfo.crashCount) + "\n";

    return info;
}

void CrashHandler::loadCrashInfoFromRTC() {
    crashInfo.hasCrashed = rtc_hasCrashed;
    crashInfo.crashCount = rtc_crashCount;
    crashInfo.lastCrashTime = rtc_lastCrashTime;
    crashInfo.lastCrashUptime = rtc_lastCrashUptime;
    crashInfo.watchdogTimeout = rtc_watchdogTimeout;

    strncpy(crashInfo.lastCrashReason, rtc_lastCrashReason, sizeof(crashInfo.lastCrashReason) - 1);
    strncpy(crashInfo.lastScreen, rtc_lastScreen, sizeof(crashInfo.lastScreen) - 1);
    strncpy(crashInfo.lastAPICall, rtc_lastAPICall, sizeof(crashInfo.lastAPICall) - 1);
    crashInfo.lastAPITime = rtc_lastAPITime;
}

void CrashHandler::saveCrashInfoToRTC() {
    rtc_hasCrashed = true;
    rtc_crashCount++;
    rtc_lastCrashTime = millis();
    rtc_lastCrashUptime = millis();

    strncpy(rtc_lastCrashReason, getResetReason(), sizeof(rtc_lastCrashReason) - 1);
    strncpy(rtc_lastScreen, currentScreen, sizeof(rtc_lastScreen) - 1);

    snprintf(rtc_lastAPICall, sizeof(rtc_lastAPICall), "%s %s",
             lastAPIService, lastAPIEndpoint);
    rtc_lastAPITime = lastAPICallTime;
}

void CrashHandler::clearCrashInfo() {
    memset(&crashInfo, 0, sizeof(crashInfo));
    rtc_hasCrashed = false;
}

String CrashHandler::getWiFiStatus() {
    switch (WiFi.status()) {
        case WL_CONNECTED:       return "Connected";
        case WL_NO_SHIELD:       return "No shield";
        case WL_IDLE_STATUS:     return "Idle";
        case WL_NO_SSID_AVAIL:   return "No SSID available";
        case WL_SCAN_COMPLETED:  return "Scan completed";
        case WL_CONNECT_FAILED:  return "Connect failed";
        case WL_CONNECTION_LOST: return "Connection lost";
        case WL_DISCONNECTED:    return "Disconnected";
        default:                 return "Unknown";
    }
}

void CrashHandler::panicHandler(void* arg) {
    // This is called during panic - keep it minimal
    // Just mark crash flag in RTC memory
    rtc_hasCrashed = true;
    rtc_crashCount++;
    rtc_lastCrashUptime = millis();

    // Save crash reason
    strncpy(rtc_lastCrashReason, "Exception/Panic", sizeof(rtc_lastCrashReason) - 1);
}
