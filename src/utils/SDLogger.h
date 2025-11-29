#ifndef SDLOGGER_H
#define SDLOGGER_H

#include <SD.h>
#include <Arduino.h>
#include <FS.h>

// SD Card Pin Definitions for SC01 Plus
#define SD_CS_PIN   41
#define SD_MOSI_PIN 40
#define SD_CLK_PIN  39
#define SD_MISO_PIN 38

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
    bool begin();
    bool isReady();

    // Core logging
    void log(LogLevel level, const char* message);
    void logf(LogLevel level, const char* format, ...);

    // Specialized logging
    void logAPI(const char* service, const char* endpoint, int status, long duration_ms, size_t response_size);
    void logAPIError(const char* service, const char* endpoint, int status, const char* error);
    void logData(const char* filename, const char* csvLine);
    void logCrash(const char* stackTrace);
    void logMemoryUsage();              // v1.2.0: Log memory statistics
    void logWatchdogCrash();            // v1.2.0: Log watchdog timeout crashes

    // CSV data logging for historical export
    void logPrice(float usd, float eur);
    void logBlock(int height, int txCount, uint32_t timestamp);
    void logMempool(int count, float sizeMB);

    // Boot logging
    void logBoot(const char* message);

    // Configuration
    void setLogLevel(LogLevel level);
    void setBufferFlushInterval(unsigned long ms);
    void setRetentionDays(int days);
    void enable();
    void disable();
    bool isEnabled();

    // Maintenance
    void flush();  // Force write buffer to SD
    void rotate(); // Create new log file (called daily)
    void cleanup(); // Delete old logs per retention policy
    void checkHotSwap(); // Check if SD card was removed/inserted
    bool formatCard(); // Format SD card (WARNING: Deletes all data)
    void exportData(const char* dataType); // Export CSV data to serial console

    // Status
    uint64_t getFreeSpace();
    uint64_t getTotalSpace();
    size_t getLogSize();
    int getLogFileCount();
    const char* getStatusString();
    bool isCardPresent();

    // Public helper for external logging
    String getTimestamp();

private:
    File currentLogFile;
    File currentAPILogFile;
    char logBuffer[4096];
    size_t bufferPos;
    LogLevel currentLevel;
    unsigned long lastFlush;
    unsigned long flushInterval;
    int retentionDays;
    bool ready;
    bool enabled;
    String currentDate;
    bool cardPresent;
    unsigned long lastHotSwapCheck;
    int writeRetryCount;

    static const int MAX_WRITE_RETRIES = 3;
    static const unsigned long HOT_SWAP_CHECK_INTERVAL = 5000; // Check every 5 seconds

    void writeBuffer();
    void ensureDirectories();
    const char* getLevelString(LogLevel level);
    String getCurrentDate();
    String formatLogLine(LogLevel level, const char* message);
    bool openLogFile(const char* path);
    void closeLogFile();
    bool shouldRotate();
    bool deleteRecursive(File dir, const char* path); // Helper for formatCard()
    void cleanupOldCSVFiles(const char* pattern, int retentionDays); // Helper for CSV retention
    int getFileDaysOld(const char* filename); // Parse date from filename
};

extern SDLogger sdLogger;

// Convenience macros
#define LOG_DEBUG(msg) sdLogger.log(LOG_DEBUG, msg)
#define LOG_INFO(msg) sdLogger.log(LOG_INFO, msg)
#define LOG_WARN(msg) sdLogger.log(LOG_WARN, msg)
#define LOG_ERROR(msg) sdLogger.log(LOG_ERROR, msg)
#define LOG_FATAL(msg) sdLogger.log(LOG_FATAL, msg)

#define LOG_DEBUGF(fmt, ...) sdLogger.logf(LOG_DEBUG, fmt, ##__VA_ARGS__)
#define LOG_INFOF(fmt, ...) sdLogger.logf(LOG_INFO, fmt, ##__VA_ARGS__)
#define LOG_WARNF(fmt, ...) sdLogger.logf(LOG_WARN, fmt, ##__VA_ARGS__)
#define LOG_ERRORF(fmt, ...) sdLogger.logf(LOG_ERROR, fmt, ##__VA_ARGS__)
#define LOG_FATALF(fmt, ...) sdLogger.logf(LOG_FATAL, fmt, ##__VA_ARGS__)

#endif
