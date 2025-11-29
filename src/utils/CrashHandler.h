#ifndef CRASH_HANDLER_H
#define CRASH_HANDLER_H

#include <Arduino.h>
#include <esp_system.h>
#include <esp_task_wdt.h>
#include <WiFi.h>

// Crash tracking structure
struct CrashInfo {
    bool hasCrashed;
    uint32_t crashCount;
    uint32_t lastCrashTime;
    uint32_t lastCrashUptime;
    char lastCrashReason[64];
    char lastScreen[32];
    char lastAPICall[128];
    uint32_t lastAPITime;
    bool watchdogTimeout;
};

class CrashHandler {
public:
    CrashHandler();

    // Initialization
    void begin();

    // Watchdog timer management
    void initWatchdog(uint32_t timeoutSeconds = 30);
    void feedWatchdog();
    void disableWatchdog();

    // Crash detection and recovery
    bool wasResetByCrash();
    bool wasResetByWatchdog();
    const char* getResetReason();
    CrashInfo getCrashInfo();

    // Crash logging
    void logCrashDump();
    void logWatchdogTimeout();
    String getLastCrashLog();

    // State tracking (for crash context)
    void setCurrentScreen(const char* screenName);
    void setLastAPICall(const char* service, const char* endpoint);

    // Exception handlers (called automatically by ESP32)
    static void panicHandler(void* arg);

private:
    CrashInfo crashInfo;
    bool watchdogEnabled;
    uint32_t watchdogTimeout;
    char currentScreen[32];
    char lastAPIService[64];
    char lastAPIEndpoint[64];
    uint32_t lastAPICallTime;

    // RTC memory for crash persistence across resets
    static RTC_DATA_ATTR bool rtc_hasCrashed;
    static RTC_DATA_ATTR uint32_t rtc_crashCount;
    static RTC_DATA_ATTR uint32_t rtc_lastCrashTime;
    static RTC_DATA_ATTR uint32_t rtc_lastCrashUptime;
    static RTC_DATA_ATTR char rtc_lastCrashReason[64];
    static RTC_DATA_ATTR char rtc_lastScreen[32];
    static RTC_DATA_ATTR char rtc_lastAPICall[128];
    static RTC_DATA_ATTR uint32_t rtc_lastAPITime;
    static RTC_DATA_ATTR bool rtc_watchdogTimeout;

    // Internal methods
    void loadCrashInfoFromRTC();
    void saveCrashInfoToRTC();
    void clearCrashInfo();
    String formatStackTrace();
    String getWiFiStatus();
    void dumpMemoryInfo();
};

// Global instance
extern CrashHandler crashHandler;

#endif // CRASH_HANDLER_H
