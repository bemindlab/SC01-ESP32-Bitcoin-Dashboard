#include "SDLogger.h"
#include <time.h>
#include <sys/time.h>

// Global instance
SDLogger sdLogger;

SDLogger::SDLogger() {
    bufferPos = 0;
    currentLevel = LOG_INFO;
    lastFlush = 0;
    flushInterval = 30000; // 30 seconds
    retentionDays = 30;
    ready = false;
    enabled = true;
    currentDate = "";
    cardPresent = false;
    lastHotSwapCheck = 0;
    writeRetryCount = 0;
}

SDLogger::~SDLogger() {
    if (ready) {
        flush();
        closeLogFile();
    }
}

bool SDLogger::begin() {
    Serial.println("\n=== SD Card Initialization ===");

    // Initialize SPI for SD card
    SPI.begin(SD_CLK_PIN, SD_MISO_PIN, SD_MOSI_PIN, SD_CS_PIN);

    // Initialize SD card
    if (!SD.begin(SD_CS_PIN)) {
        Serial.println("✗ SD card initialization failed");
        Serial.println("  Possible causes:");
        Serial.println("  - No SD card inserted");
        Serial.println("  - SD card not formatted (use FAT32)");
        Serial.println("  - Hardware connection issue");
        ready = false;
        enabled = false;
        return false;
    }

    // Check card type
    uint8_t cardType = SD.cardType();
    if (cardType == CARD_NONE) {
        Serial.println("✗ No SD card attached");
        ready = false;
        enabled = false;
        return false;
    }

    // Print card info
    Serial.print("✓ SD card initialized: ");
    switch (cardType) {
        case CARD_MMC:  Serial.println("MMC"); break;
        case CARD_SD:   Serial.println("SDSC"); break;
        case CARD_SDHC: Serial.println("SDHC"); break;
        default:        Serial.println("UNKNOWN"); break;
    }

    uint64_t cardSize = SD.cardSize() / (1024 * 1024);
    Serial.printf("  Card Size: %llu MB\n", cardSize);
    Serial.printf("  Total Space: %.2f GB\n", SD.totalBytes() / (1024.0 * 1024.0 * 1024.0));
    Serial.printf("  Used Space: %.2f GB\n", SD.usedBytes() / (1024.0 * 1024.0 * 1024.0));
    Serial.printf("  Free Space: %.2f GB\n", (SD.totalBytes() - SD.usedBytes()) / (1024.0 * 1024.0 * 1024.0));

    // Create directory structure
    ensureDirectories();

    ready = true;
    cardPresent = true;
    currentDate = getCurrentDate();
    lastHotSwapCheck = millis();

    // Log initialization
    logBoot("SD Card logging initialized");

    Serial.println("✓ SD Card logging ready");
    return true;
}

bool SDLogger::isReady() {
    return ready && enabled;
}

void SDLogger::ensureDirectories() {
    const char* dirs[] = {
        "/logs",
        "/logs/system",
        "/logs/api",
        "/logs/data",
        "/logs/errors",
        "/logs/debug"
    };

    for (int i = 0; i < 6; i++) {
        if (!SD.exists(dirs[i])) {
            if (SD.mkdir(dirs[i])) {
                Serial.printf("  Created directory: %s\n", dirs[i]);
            } else {
                Serial.printf("  Failed to create: %s\n", dirs[i]);
            }
        }
    }
}

String SDLogger::getTimestamp() {
    struct timeval tv;
    gettimeofday(&tv, NULL);

    struct tm timeinfo;
    localtime_r(&tv.tv_sec, &timeinfo);

    char timestamp[32];
    snprintf(timestamp, sizeof(timestamp), "%04d-%02d-%02d %02d:%02d:%02d.%03ld",
             timeinfo.tm_year + 1900, timeinfo.tm_mon + 1, timeinfo.tm_mday,
             timeinfo.tm_hour, timeinfo.tm_min, timeinfo.tm_sec,
             tv.tv_usec / 1000);

    return String(timestamp);
}

String SDLogger::getCurrentDate() {
    struct timeval tv;
    gettimeofday(&tv, NULL);

    struct tm timeinfo;
    localtime_r(&tv.tv_sec, &timeinfo);

    char date[16];
    snprintf(date, sizeof(date), "%04d-%02d-%02d",
             timeinfo.tm_year + 1900, timeinfo.tm_mon + 1, timeinfo.tm_mday);

    return String(date);
}

const char* SDLogger::getLevelString(LogLevel level) {
    switch (level) {
        case LOG_DEBUG: return "DEBUG";
        case LOG_INFO:  return "INFO";
        case LOG_WARN:  return "WARN";
        case LOG_ERROR: return "ERROR";
        case LOG_FATAL: return "FATAL";
        default:        return "UNKNOWN";
    }
}

String SDLogger::formatLogLine(LogLevel level, const char* message) {
    char line[512];
    snprintf(line, sizeof(line), "[%s] [%s] %s\n",
             getTimestamp().c_str(), getLevelString(level), message);
    return String(line);
}

bool SDLogger::shouldRotate() {
    String newDate = getCurrentDate();
    return (newDate != currentDate);
}

void SDLogger::log(LogLevel level, const char* message) {
    if (!isReady() || level < currentLevel) {
        return;
    }

    // Also print to serial for important messages
    if (level >= LOG_WARN) {
        Serial.printf("[%s] %s\n", getLevelString(level), message);
    }

    // Check if we need to rotate (new day)
    if (shouldRotate()) {
        rotate();
    }

    // Format log line
    String logLine = formatLogLine(level, message);

    // Add to buffer
    size_t lineLen = logLine.length();
    if (bufferPos + lineLen >= sizeof(logBuffer)) {
        // Buffer full, flush first
        flush();
    }

    // Copy to buffer
    if (bufferPos + lineLen < sizeof(logBuffer)) {
        memcpy(logBuffer + bufferPos, logLine.c_str(), lineLen);
        bufferPos += lineLen;
    }

    // Auto-flush on ERROR or FATAL
    if (level >= LOG_ERROR) {
        flush();
    }

    // Periodic flush
    unsigned long now = millis();
    if (now - lastFlush >= flushInterval) {
        flush();
    }
}

void SDLogger::logf(LogLevel level, const char* format, ...) {
    char message[256];
    va_list args;
    va_start(args, format);
    vsnprintf(message, sizeof(message), format, args);
    va_end(args);

    log(level, message);
}

void SDLogger::logBoot(const char* message) {
    if (!ready) return;

    // Create boot log with timestamp
    String bootLogPath = "/logs/system/boot_" + getTimestamp() + ".log";
    bootLogPath.replace(" ", "_");
    bootLogPath.replace(":", "-");
    bootLogPath.replace(".", "-");

    File bootLog = SD.open(bootLogPath.c_str(), FILE_APPEND);
    if (bootLog) {
        String logLine = formatLogLine(LOG_INFO, message);
        bootLog.print(logLine);
        bootLog.close();
        Serial.printf("Boot log: %s\n", message);
    }
}

void SDLogger::logAPI(const char* service, const char* endpoint, int status, long duration_ms, size_t response_size) {
    if (!isReady()) return;

    // JSON Lines format for API logs
    char apiLog[512];
    snprintf(apiLog, sizeof(apiLog),
             "{\"timestamp\":\"%s\",\"service\":\"%s\",\"endpoint\":\"%s\",\"status\":%d,\"duration_ms\":%ld,\"response_size\":%zu}\n",
             getTimestamp().c_str(), service, endpoint, status, duration_ms, response_size);

    // Determine log file based on service
    String apiLogPath = "/logs/api/";
    if (strstr(service, "mempool")) {
        apiLogPath += "mempool_";
    } else if (strstr(service, "gemini")) {
        apiLogPath += "gemini_";
    } else if (strstr(service, "openai")) {
        apiLogPath += "openai_";
    } else {
        apiLogPath += "general_";
    }
    apiLogPath += getCurrentDate() + ".log";

    File apiFile = SD.open(apiLogPath.c_str(), FILE_APPEND);
    if (apiFile) {
        apiFile.print(apiLog);
        apiFile.close();
    }
}

void SDLogger::logAPIError(const char* service, const char* endpoint, int status, const char* error) {
    if (!isReady()) return;

    char errorMsg[512];
    snprintf(errorMsg, sizeof(errorMsg), "API Error: %s %s - Status %d: %s",
             service, endpoint, status, error);

    log(LOG_ERROR, errorMsg);

    // Also log to API error file
    char apiLog[512];
    snprintf(apiLog, sizeof(apiLog),
             "{\"timestamp\":\"%s\",\"service\":\"%s\",\"endpoint\":\"%s\",\"status\":%d,\"error\":\"%s\"}\n",
             getTimestamp().c_str(), service, endpoint, status, error);

    String errorLogPath = "/logs/errors/api_errors_" + getCurrentDate() + ".log";
    File errorFile = SD.open(errorLogPath.c_str(), FILE_APPEND);
    if (errorFile) {
        errorFile.print(apiLog);
        errorFile.close();
    }
}

void SDLogger::logData(const char* filename, const char* csvLine) {
    if (!isReady()) return;

    String dataPath = String(filename);
    File dataFile = SD.open(dataPath.c_str(), FILE_APPEND);
    if (dataFile) {
        dataFile.println(csvLine);
        dataFile.close();
    }
}

void SDLogger::logCrash(const char* stackTrace) {
    if (!ready) return; // Don't check enabled - always log crashes

    String crashLogPath = "/logs/errors/crash_" + getTimestamp() + ".log";
    crashLogPath.replace(" ", "_");
    crashLogPath.replace(":", "-");
    crashLogPath.replace(".", "-");

    File crashLog = SD.open(crashLogPath.c_str(), FILE_WRITE);
    if (crashLog) {
        crashLog.printf("=== CRASH DUMP ===\n");
        crashLog.printf("Timestamp: %s\n", getTimestamp().c_str());
        crashLog.printf("Free Heap: %d bytes\n", ESP.getFreeHeap());
        crashLog.printf("Free PSRAM: %d bytes\n", ESP.getFreePsram());
        crashLog.printf("Uptime: %lu seconds\n\n", millis() / 1000);
        crashLog.printf("Stack Trace:\n%s\n", stackTrace);
        crashLog.close();

        Serial.printf("Crash log saved to: %s\n", crashLogPath.c_str());
    }
}

void SDLogger::flush() {
    if (!isReady() || bufferPos == 0) {
        return;
    }

    // Open daily system log with retry logic
    String logPath = "/logs/system/system_" + getCurrentDate() + ".log";
    bool writeSuccess = false;

    for (int retry = 0; retry < MAX_WRITE_RETRIES; retry++) {
        File logFile = SD.open(logPath.c_str(), FILE_APPEND);

        if (logFile) {
            size_t written = logFile.write((uint8_t*)logBuffer, bufferPos);
            logFile.close();

            if (written == bufferPos) {
                writeSuccess = true;
                writeRetryCount = 0; // Reset retry counter on success
                break;
            } else {
                Serial.printf("WARN: SD write incomplete (%zu/%zu bytes), retry %d/%d\n",
                             written, bufferPos, retry + 1, MAX_WRITE_RETRIES);
                delay(50); // Brief delay before retry
            }
        } else {
            Serial.printf("WARN: Failed to open log file, retry %d/%d\n",
                         retry + 1, MAX_WRITE_RETRIES);
            delay(50);
        }
    }

    if (!writeSuccess) {
        Serial.println("ERROR: SD write failed after all retries");
        writeRetryCount++;

        // After 5 consecutive failures, check if card was removed
        if (writeRetryCount >= 5) {
            cardPresent = false;
            Serial.println("ERROR: SD card may have been removed");
        }
        return; // Keep buffer for next attempt
    }

    // Clear buffer only on successful write
    bufferPos = 0;
    lastFlush = millis();
}

void SDLogger::rotate() {
    flush(); // Flush current buffer
    currentDate = getCurrentDate();
    Serial.printf("Log rotated to date: %s\n", currentDate.c_str());
}

void SDLogger::cleanup() {
    if (!isReady()) return;

    Serial.println("\n=== SD Card Cleanup Starting ===");

    // Clean up CSV data files with specific retention policies
    cleanupOldCSVFiles("btc_price_", 90);    // Price data: 90 days
    cleanupOldCSVFiles("btc_mempool_", 30);  // Mempool data: 30 days
    // Block data is kept indefinitely (low volume)

    // Clean up system logs based on general retention policy
    // TODO: Implement system log cleanup based on retentionDays

    Serial.println("=== SD Card Cleanup Complete ===\n");
}

void SDLogger::setLogLevel(LogLevel level) {
    currentLevel = level;
    Serial.printf("Log level set to: %s\n", getLevelString(level));
}

void SDLogger::setBufferFlushInterval(unsigned long ms) {
    flushInterval = ms;
}

void SDLogger::setRetentionDays(int days) {
    retentionDays = days;
}

void SDLogger::enable() {
    enabled = true;
    Serial.println("SD logging enabled");
}

void SDLogger::disable() {
    flush(); // Flush before disabling
    enabled = false;
    Serial.println("SD logging disabled");
}

bool SDLogger::isEnabled() {
    return enabled;
}

void SDLogger::closeLogFile() {
    if (currentLogFile) {
        currentLogFile.close();
    }
    if (currentAPILogFile) {
        currentAPILogFile.close();
    }
}

uint64_t SDLogger::getFreeSpace() {
    if (!ready) return 0;
    return SD.totalBytes() - SD.usedBytes();
}

uint64_t SDLogger::getTotalSpace() {
    if (!ready) return 0;
    return SD.totalBytes();
}

size_t SDLogger::getLogSize() {
    // TODO: Calculate total size of all log files
    return 0;
}

int SDLogger::getLogFileCount() {
    // TODO: Count log files in /logs directory
    return 0;
}

const char* SDLogger::getStatusString() {
    if (!ready) return "Not initialized";
    if (!enabled) return "Disabled";
    if (!cardPresent) return "Card removed";
    return "Active";
}

void SDLogger::checkHotSwap() {
    unsigned long now = millis();

    // Only check periodically to avoid overhead
    if (now - lastHotSwapCheck < HOT_SWAP_CHECK_INTERVAL) {
        return;
    }

    lastHotSwapCheck = now;

    // Try to detect card presence
    uint8_t cardType = SD.cardType();

    if (cardType == CARD_NONE) {
        // Card was removed
        if (cardPresent) {
            cardPresent = false;
            ready = false;
            Serial.println("\n=== SD CARD REMOVED ===");
            Serial.println("Logging disabled until card is re-inserted");
        }
    } else {
        // Card is present
        if (!cardPresent) {
            // Card was re-inserted
            Serial.println("\n=== SD CARD DETECTED ===");
            Serial.println("Attempting to re-initialize...");

            // Try to re-initialize
            if (begin()) {
                Serial.println("✓ SD card re-initialized successfully");
                cardPresent = true;
                ready = true;
                writeRetryCount = 0;

                // Log the hot-swap event
                logf(LOG_INFO, "SD card hot-swap detected - re-initialized at %s",
                     getTimestamp().c_str());
            } else {
                Serial.println("✗ Failed to re-initialize SD card");
            }
        }
    }
}

bool SDLogger::isCardPresent() {
    return cardPresent;
}

void SDLogger::logMemoryUsage() {
    if (!isReady()) return;

    uint32_t freeHeap = ESP.getFreeHeap();
    uint32_t totalHeap = ESP.getHeapSize();
    uint32_t freePsram = ESP.getFreePsram();
    uint32_t totalPsram = ESP.getPsramSize();
    uint32_t usedHeap = totalHeap - freeHeap;
    uint32_t usedPsram = totalPsram - freePsram;

    // Calculate percentages
    float heapUsagePercent = (usedHeap * 100.0) / totalHeap;
    float psramUsagePercent = totalPsram > 0 ? (usedPsram * 100.0) / totalPsram : 0.0;

    // Create CSV line for memory tracking
    char csvLine[256];
    snprintf(csvLine, sizeof(csvLine),
             "%s,%u,%u,%.1f,%u,%u,%.1f",
             getTimestamp().c_str(),
             usedHeap, totalHeap, heapUsagePercent,
             usedPsram, totalPsram, psramUsagePercent);

    // Log to memory tracking file
    logData("/logs/data/memory_usage.csv", csvLine);

    // Also log to system log if usage is high
    if (heapUsagePercent > 80.0 || psramUsagePercent > 80.0) {
        logf(LOG_WARN, "High memory usage: Heap %.1f%%, PSRAM %.1f%%",
             heapUsagePercent, psramUsagePercent);
    }
}

void SDLogger::logWatchdogCrash() {
    if (!ready) return; // Don't check enabled - always log watchdog crashes

    String crashLogPath = "/logs/errors/watchdog_" + getTimestamp() + ".log";
    crashLogPath.replace(" ", "_");
    crashLogPath.replace(":", "-");
    crashLogPath.replace(".", "-");

    File crashLog = SD.open(crashLogPath.c_str(), FILE_WRITE);
    if (crashLog) {
        crashLog.printf("=== WATCHDOG TIMEOUT CRASH ===\n");
        crashLog.printf("Timestamp: %s\n", getTimestamp().c_str());
        crashLog.printf("Uptime: %lu seconds\n", millis() / 1000);
        crashLog.printf("\n--- Memory State ---\n");
        crashLog.printf("Free Heap: %u bytes (Total: %u bytes)\n",
                       ESP.getFreeHeap(), ESP.getHeapSize());
        crashLog.printf("Free PSRAM: %u bytes (Total: %u bytes)\n",
                       ESP.getFreePsram(), ESP.getPsramSize());
        crashLog.printf("Largest Free Heap Block: %u bytes\n",
                       ESP.getMaxAllocHeap());

        crashLog.printf("\n--- CPU State ---\n");
        crashLog.printf("CPU Frequency: %u MHz\n", ESP.getCpuFreqMHz());
        crashLog.printf("Flash Size: %u bytes\n", ESP.getFlashChipSize());
        crashLog.printf("Flash Speed: %u MHz\n", ESP.getFlashChipSpeed() / 1000000);

        crashLog.printf("\n--- Task State ---\n");
        crashLog.printf("Likely cause: Task blocked for >10 seconds\n");
        crashLog.printf("Check for:\n");
        crashLog.printf("  - Long SD card write operations\n");
        crashLog.printf("  - Blocking HTTP requests\n");
        crashLog.printf("  - Infinite loops without yield()\n");
        crashLog.printf("  - Mutex deadlocks\n");

        crashLog.printf("\n--- Recommended Actions ---\n");
        crashLog.printf("1. Review recent code changes\n");
        crashLog.printf("2. Check serial output for stuck operations\n");
        crashLog.printf("3. Verify all HTTP requests have timeouts\n");
        crashLog.printf("4. Ensure SD writes are non-blocking\n");
        crashLog.printf("5. Add vTaskDelay() to long-running loops\n");

        crashLog.close();

        Serial.printf("\nWatchdog crash log saved to: %s\n", crashLogPath.c_str());
    }
}

bool SDLogger::formatCard() {
    if (!cardPresent) {
        Serial.println("✗ No SD card present");
        return false;
    }

    Serial.println("\n========================================");
    Serial.println("⚠️  WARNING: SD CARD FORMAT");
    Serial.println("========================================");
    Serial.println("This will DELETE ALL DATA on the SD card!");
    Serial.println("All logs, screenshots, and data will be lost.");
    Serial.println("");

    // Flush any pending writes
    flush();

    // Close any open files
    closeLogFile();

    // End SD card
    SD.end();

    Serial.println("Formatting SD card...");
    Serial.println("Please wait, this may take 10-30 seconds...");

    // Re-initialize SD card with format option
    // Note: ESP32 SD library doesn't have a direct format() method
    // We need to delete all files and directories recursively

    if (!SD.begin(SD_CS_PIN)) {
        Serial.println("✗ Failed to reinitialize SD card");
        ready = false;
        cardPresent = false;
        return false;
    }

    // Delete all files and directories
    Serial.println("Deleting all files and directories...");

    File root = SD.open("/");
    if (!root) {
        Serial.println("✗ Failed to open root directory");
        return false;
    }

    // Recursive delete function
    bool success = deleteRecursive(root, "/");
    root.close();

    if (!success) {
        Serial.println("⚠️  Some files may not have been deleted");
    }

    // Recreate directory structure
    Serial.println("Creating log directory structure...");
    ensureDirectories();

    // Re-enable logging
    ready = true;
    cardPresent = true;
    currentDate = getCurrentDate();

    Serial.println("========================================");
    Serial.println("✓ SD card formatted successfully");
    Serial.println("========================================");
    Serial.printf("Free Space: %.2f GB / %.2f GB\n",
                 getFreeSpace() / (1024.0 * 1024.0 * 1024.0),
                 getTotalSpace() / (1024.0 * 1024.0 * 1024.0));
    Serial.println("Log directory structure recreated");

    // Log the format event
    logf(LOG_WARN, "SD card formatted - all previous data deleted");

    return true;
}

bool SDLogger::deleteRecursive(File dir, const char* path) {
    bool success = true;

    File file = dir.openNextFile();
    while (file) {
        String fileName = String(path) + String(file.name());

        if (file.isDirectory()) {
            // Recursively delete directory contents
            if (!deleteRecursive(file, (fileName + "/").c_str())) {
                success = false;
            }
            file.close();

            // Delete the empty directory
            if (!SD.rmdir(fileName.c_str())) {
                Serial.printf("Failed to delete directory: %s\n", fileName.c_str());
                success = false;
            } else {
                Serial.printf("Deleted directory: %s\n", fileName.c_str());
            }
        } else {
            file.close();

            // Delete the file
            if (!SD.remove(fileName.c_str())) {
                Serial.printf("Failed to delete file: %s\n", fileName.c_str());
                success = false;
            } else {
                Serial.printf("Deleted file: %s\n", fileName.c_str());
            }
        }

        file = dir.openNextFile();
    }

    return success;
}

// ==================== CSV Data Logging ====================

void SDLogger::logPrice(float usd, float eur) {
    if (!isReady()) return;

    // Create filename: /logs/data/btc_price_YYYY-MM-DD.csv
    String filename = "/logs/data/btc_price_" + getCurrentDate() + ".csv";

    // Check if file exists to determine if we need to write header
    bool fileExists = SD.exists(filename.c_str());

    File csvFile = SD.open(filename.c_str(), FILE_APPEND);
    if (!csvFile) {
        Serial.printf("ERROR: Failed to open %s\n", filename.c_str());
        return;
    }

    // Write CSV header if new file
    if (!fileExists) {
        csvFile.println("timestamp,price_usd,price_eur");
    }

    // Write data row
    char csvLine[128];
    snprintf(csvLine, sizeof(csvLine), "%s,%.2f,%.2f",
             getTimestamp().c_str(), usd, eur);
    csvFile.println(csvLine);
    csvFile.close();

    // Log success (DEBUG level to avoid spam)
    if (currentLevel <= LOG_DEBUG) {
        Serial.printf("[CSV] Price logged: $%.2f / €%.2f\n", usd, eur);
    }
}

void SDLogger::logBlock(int height, int txCount, uint32_t timestamp) {
    if (!isReady()) return;

    // Create filename: /logs/data/btc_blocks_YYYY-MM-DD.csv
    String filename = "/logs/data/btc_blocks_" + getCurrentDate() + ".csv";

    // Check if file exists to determine if we need to write header
    bool fileExists = SD.exists(filename.c_str());

    File csvFile = SD.open(filename.c_str(), FILE_APPEND);
    if (!csvFile) {
        Serial.printf("ERROR: Failed to open %s\n", filename.c_str());
        return;
    }

    // Write CSV header if new file
    if (!fileExists) {
        csvFile.println("timestamp,block_height,tx_count,block_timestamp");
    }

    // Write data row
    char csvLine[128];
    snprintf(csvLine, sizeof(csvLine), "%s,%d,%d,%u",
             getTimestamp().c_str(), height, txCount, timestamp);
    csvFile.println(csvLine);
    csvFile.close();

    // Log success
    Serial.printf("[CSV] Block logged: Height %d (%d TXs)\n", height, txCount);
}

void SDLogger::logMempool(int count, float sizeMB) {
    if (!isReady()) return;

    // Create filename: /logs/data/btc_mempool_YYYY-MM-DD.csv
    String filename = "/logs/data/btc_mempool_" + getCurrentDate() + ".csv";

    // Check if file exists to determine if we need to write header
    bool fileExists = SD.exists(filename.c_str());

    File csvFile = SD.open(filename.c_str(), FILE_APPEND);
    if (!csvFile) {
        Serial.printf("ERROR: Failed to open %s\n", filename.c_str());
        return;
    }

    // Write CSV header if new file
    if (!fileExists) {
        csvFile.println("timestamp,tx_count,size_mb");
    }

    // Write data row
    char csvLine[128];
    snprintf(csvLine, sizeof(csvLine), "%s,%d,%.2f",
             getTimestamp().c_str(), count, sizeMB);
    csvFile.println(csvLine);
    csvFile.close();

    // Log success (DEBUG level to avoid spam)
    if (currentLevel <= LOG_DEBUG) {
        Serial.printf("[CSV] Mempool logged: %d TXs (%.2f MB)\n", count, sizeMB);
    }
}

// ==================== CSV Data Export ====================

void SDLogger::exportData(const char* dataType) {
    if (!isReady()) {
        Serial.println("ERROR: SD card not ready");
        return;
    }

    String pattern;
    if (strcmp(dataType, "PRICE") == 0) {
        pattern = "btc_price_";
    } else if (strcmp(dataType, "BLOCKS") == 0) {
        pattern = "btc_blocks_";
    } else if (strcmp(dataType, "MEMPOOL") == 0) {
        pattern = "btc_mempool_";
    } else if (strcmp(dataType, "ALL") == 0) {
        // Export all data types
        Serial.println("\n=== EXPORTING ALL DATA ===\n");
        exportData("PRICE");
        exportData("BLOCKS");
        exportData("MEMPOOL");
        return;
    } else {
        Serial.printf("ERROR: Unknown data type '%s'\n", dataType);
        Serial.println("Valid types: PRICE, BLOCKS, MEMPOOL, ALL");
        return;
    }

    Serial.printf("\n=== EXPORT START: %s ===\n", dataType);

    // Open data directory
    File dataDir = SD.open("/logs/data");
    if (!dataDir) {
        Serial.println("ERROR: Failed to open /logs/data directory");
        return;
    }

    int filesExported = 0;
    int totalLines = 0;

    // Iterate through files
    File file = dataDir.openNextFile();
    while (file) {
        String filename = String(file.name());

        // Check if filename matches pattern
        if (filename.startsWith(pattern)) {
            Serial.printf("\n--- FILE: %s ---\n", filename.c_str());

            // Read and output entire file
            while (file.available()) {
                String line = file.readStringUntil('\n');
                Serial.println(line);
                totalLines++;
            }

            filesExported++;
        }

        file.close();
        file = dataDir.openNextFile();
    }

    dataDir.close();

    Serial.printf("\n=== EXPORT END: %s ===\n", dataType);
    Serial.printf("Files exported: %d\n", filesExported);
    Serial.printf("Total lines: %d\n", totalLines);
}

// ==================== CSV Data Retention ====================

int SDLogger::getFileDaysOld(const char* filename) {
    // Extract date from filename (format: prefix_YYYY-MM-DD.csv)
    String filenameStr = String(filename);

    // Find date pattern (YYYY-MM-DD)
    int dateStart = filenameStr.lastIndexOf('_') + 1;
    int dateEnd = filenameStr.lastIndexOf('.');

    if (dateStart <= 0 || dateEnd <= dateStart) {
        return -1; // Invalid format
    }

    String dateStr = filenameStr.substring(dateStart, dateEnd);

    // Parse date components
    int year, month, day;
    if (sscanf(dateStr.c_str(), "%d-%d-%d", &year, &month, &day) != 3) {
        return -1; // Parse failed
    }

    // Get current date
    struct timeval tv;
    gettimeofday(&tv, NULL);
    struct tm timeinfo;
    localtime_r(&tv.tv_sec, &timeinfo);

    int currentYear = timeinfo.tm_year + 1900;
    int currentMonth = timeinfo.tm_mon + 1;
    int currentDay = timeinfo.tm_mday;

    // Simple day calculation (approximate - doesn't account for month lengths)
    int fileDays = year * 365 + month * 30 + day;
    int currentDays = currentYear * 365 + currentMonth * 30 + currentDay;

    return currentDays - fileDays;
}

void SDLogger::cleanupOldCSVFiles(const char* pattern, int retentionDays) {
    if (!isReady()) return;

    File dataDir = SD.open("/logs/data");
    if (!dataDir) {
        Serial.println("WARN: Failed to open /logs/data for cleanup");
        return;
    }

    int deletedCount = 0;

    File file = dataDir.openNextFile();
    while (file) {
        String filename = String(file.name());

        // Check if filename matches pattern
        if (filename.startsWith(pattern)) {
            int daysOld = getFileDaysOld(filename.c_str());

            if (daysOld > retentionDays) {
                file.close();

                // Delete the old file
                String fullPath = "/logs/data/" + filename;
                if (SD.remove(fullPath.c_str())) {
                    Serial.printf("Deleted old CSV: %s (%d days old)\n",
                                 filename.c_str(), daysOld);
                    deletedCount++;
                } else {
                    Serial.printf("Failed to delete: %s\n", filename.c_str());
                }

                // Re-open directory after deletion
                file = dataDir.openNextFile();
                continue;
            }
        }

        file.close();
        file = dataDir.openNextFile();
    }

    dataDir.close();

    if (deletedCount > 0) {
        Serial.printf("Cleanup: Deleted %d old CSV files (%s)\n",
                     deletedCount, pattern);
    }
}
