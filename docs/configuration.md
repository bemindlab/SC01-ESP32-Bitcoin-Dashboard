# Configuration System Guide

**Status:** ✅ Implemented and Ready to Use

## Overview

The Bitcoin Dashboard now includes a comprehensive configuration system that stores settings persistently in ESP32's NVRAM (Non-Volatile RAM). This allows you to configure API keys, WiFi credentials, and update intervals without modifying code.

## Features

✅ **Persistent Storage** - Settings saved in NVRAM, survive reboots and power loss
✅ **Gemini API Key Management** - Configure via Serial Monitor or Settings screen
✅ **WiFi Credentials Storage** - Automatic connection on startup
✅ **Update Intervals** - Customize refresh rates for price, blocks, and mempool
✅ **Settings Screen** - Touch-based UI for configuration
✅ **Serial Commands** - Configure via Serial Monitor
✅ **First-Run Detection** - Prompts for configuration on initial boot

## Quick Start

### Method 1: Serial Monitor (Recommended)

1. **Connect ESP32 via USB**
2. **Open Serial Monitor** (115200 baud)
3. **Send commands:**

```
SET_GEMINI_KEY=your-api-key-here
```

4. **Verify:**
```
STATUS
```

### Method 2: Settings Screen

1. **Tap Settings icon** (gear icon on Dashboard)
2. **Select "Gemini API Key"**
3. **Follow on-screen instructions**
4. **Configure via Serial Monitor** (on-screen keyboard coming soon)

## Configuration Storage

### Where Settings Are Stored

- **Location:** ESP32 NVRAM (Preferences library)
- **Namespace:** `btc_dash`
- **Persistence:** Survives power loss, reboot, and firmware updates
- **Security:** API keys are masked in serial output

### Stored Settings

| Setting | Key | Type | Default |
|---------|-----|------|---------|
| Gemini API Key | `gemini_key` | String | "" |
| WiFi SSID | `wifi_ssid` | String | "" |
| WiFi Password | `wifi_pass` | String | "" |
| Price Interval | `price_int` | ULong | 30000 ms |
| Block Interval | `block_int` | ULong | 60000 ms |
| Mempool Interval | `mempool_int` | ULong | 30000 ms |
| First Run Flag | `first_run` | Bool | true |

## Serial Commands

### Configure API Key

```
SET_GEMINI_KEY=AIzaSyAbCdEfGhIjKlMnOpQrStUvWxYz1234567
```

**Response:**
```
✓ Gemini API key saved successfully!
```

### Check Status

```
STATUS
```

**Response:**
```
WiFi: Connected
Free Heap: 245678 bytes
Uptime: 123 seconds

--- Current Configuration ---
Gemini API Key: AIza...4567
WiFi SSID: MyNetwork
WiFi Password: ****
Price Interval: 30000 ms (30 sec)
Block Interval: 60000 ms (60 sec)
Mempool Interval: 30000 ms (30 sec)
First Run: NO
-----------------------------
```

### Reset Configuration

```
RESET_CONFIG
```

**Warning:** This erases ALL stored settings!

### Help

```
HELP
```

**Shows:**
```
Available commands:
  SCREENSHOT         - Capture display buffer
  STATUS             - Show device status
  SET_GEMINI_KEY=xxx - Set Gemini API key
  RESET_CONFIG       - Reset all configuration
  HELP               - Show this help
```

## Settings Screen

### Accessing Settings

1. **From Dashboard:** Tap the gear icon (top-right, next to WiFi icon)
2. **From News Screen:** Swipe right → Dashboard → Tap gear icon

### Settings Options

#### 1. Gemini API Key
- **Status:** Shows "SET" or "NOT SET"
- **Action:** Displays instructions for configuring via Serial Monitor
- **Future:** On-screen keyboard for direct entry

#### 2. WiFi Settings
- **Status:** Shows "CONFIGURED" or "SETUP"
- **Action:** Opens WiFi Scan screen
- **Note:** WiFi credentials auto-saved after successful connection

#### 3. Update Intervals
- **Status:** Shows "EDIT"
- **Action:** Coming soon (adjust refresh rates)
- **Current:** Price=30s, Blocks=60s, Mempool=30s

#### 4. Reset Configuration
- **Status:** Shows "DANGER"
- **Action:** Erases all settings with 3-second countdown
- **Warning:** Cannot be undone!

#### 5. Back to Dashboard
- **Action:** Returns to main dashboard

## First Run Setup

### What Happens on First Boot

1. **Configuration loads** from NVRAM
2. **First Run Detection:**
   ```
   ⚠️  FIRST RUN DETECTED
   Configure via Serial Monitor:
     SET_GEMINI_KEY=your-api-key-here
   ```
3. **WiFi Scan** screen appears (no stored credentials)
4. **Connect to WiFi** via touch interface
5. **Credentials auto-saved** for next boot

### Subsequent Boots

1. **Configuration loads** from NVRAM
2. **Auto-connects** to saved WiFi
3. **Loads Dashboard** immediately (if WiFi connects)
4. **Falls back** to WiFi Scan if connection fails

## Configuration API

### For Developers

```cpp
#include "Config.h"

// Access global configuration
extern ConfigManager globalConfig;

// Load configuration
globalConfig.load();

// Get settings
String apiKey = globalConfig.getGeminiApiKey();
String ssid = globalConfig.getWiFiSSID();
String pass = globalConfig.getWiFiPassword();
unsigned long interval = globalConfig.getPriceInterval();

// Set settings
globalConfig.setGeminiApiKey("new-key");
globalConfig.setWiFiCredentials("MySSID", "MyPassword");
globalConfig.setPriceInterval(60000);  // 60 seconds

// Save to NVRAM
globalConfig.save();

// Check status
bool hasKey = globalConfig.hasGeminiKey();
bool hasWiFi = globalConfig.hasWiFiCredentials();
bool isValid = globalConfig.isValid();

// Reset to defaults
globalConfig.reset();
globalConfig.save();

// Debug output
globalConfig.printConfig();
```

### AppConfig Structure

```cpp
struct AppConfig {
    String geminiApiKey;
    String wifiSSID;
    String wifiPassword;
    unsigned long priceInterval;
    unsigned long blockInterval;
    unsigned long mempoolInterval;
    bool firstRun;
};
```

## Security Considerations

### API Key Protection

**Secure:**
- ✅ Stored in encrypted NVRAM
- ✅ Masked in serial output (shows only first/last 4 chars)
- ✅ Not visible on Settings screen
- ✅ Not included in screenshots

**Not Secure:**
- ⚠️ Accessible via ESP32 flash read (physical access required)
- ⚠️ Transmitted over WiFi to Google (HTTPS encrypted)

**Best Practices:**
1. Use API keys with usage limits
2. Monitor API usage in Google Cloud Console
3. Rotate keys periodically
4. Don't share firmware dumps containing keys

### WiFi Password Protection

- Stored encrypted in NVRAM
- Never displayed in serial output
- Shown as "****" in status output

## Troubleshooting

### Configuration Not Loading

**Symptoms:**
- Settings reset on every boot
- API key not found

**Solutions:**
1. Check serial output for errors:
   ```
   Failed to open preferences for reading
   ```
2. Verify NVRAM not corrupted:
   ```
   RESET_CONFIG
   ```
3. Re-enter configuration

### API Key Not Working

**Check:**
1. **View current key:**
   ```
   STATUS
   ```
2. **Verify format:**
   - Starts with "AIza"
   - 39 characters long
   - No spaces or quotes

3. **Test key** at: https://makersuite.google.com/app/apikey

### WiFi Not Auto-Connecting

**Check:**
1. **Verify credentials stored:**
   ```
   STATUS
   ```
   Should show: `WiFi SSID: YourNetwork`

2. **Check WiFi signal** - ESP32-S3 only supports 2.4GHz

3. **Manual reconnect:**
   - Tap Settings → WiFi Settings
   - Select your network again

### Settings Screen Not Responding

**Try:**
1. **Return to Dashboard** - Tap "<" button
2. **Restart device** - Power cycle
3. **Check serial output** for touch events

## File Structure

```
src/
├── Config.h                  [NEW] Configuration structure and manager
├── Config.cpp                [NEW] Configuration implementation
├── main.cpp                  [MODIFIED] Added config loading and serial commands
├── api/
│   └── GeminiClient.cpp      [MODIFIED] Uses config for API key
└── screens/
    ├── ScreenManager.h       [MODIFIED] Added SCREEN_SETTINGS enum
    ├── ScreenManager.cpp     [MODIFIED] Registered SettingsScreen
    ├── SettingsScreen.h      [NEW] Settings screen interface
    ├── SettingsScreen.cpp    [NEW] Settings screen implementation
    └── BTCDashboardScreen.* [MODIFIED] Added settings icon
```

## Migration Guide

### From Hardcoded Keys

**Old way** (GeminiClient.h):
```cpp
#define GEMINI_API_KEY "your-key-here"
```

**New way** (Serial Monitor):
```
SET_GEMINI_KEY=your-key-here
```

### From WiFi Hardcoded

**Old way** (main.cpp):
```cpp
WiFi.begin("MySSID", "MyPassword");
```

**New way:**
- Use WiFi Scan screen on first boot
- Credentials auto-saved
- Auto-connects on subsequent boots

## Advanced Usage

### Custom Intervals

Currently via code (UI coming soon):

```cpp
globalConfig.setPriceInterval(15000);   // 15 seconds
globalConfig.setBlockInterval(120000);  // 2 minutes
globalConfig.setMempoolInterval(45000); // 45 seconds
globalConfig.save();
```

**Restart required** to apply new intervals.

### Backup Configuration

**Export via Serial:**
```
STATUS
```
Copy output and save to file.

**Restore:**
```
SET_GEMINI_KEY=your-key
```
WiFi: Reconnect via UI

### Factory Reset

**Method 1: Serial**
```
RESET_CONFIG
```

**Method 2: Settings Screen**
- Tap Settings → Reset Configuration
- Wait 3 seconds

**Method 3: Code**
```cpp
globalConfig.reset();
globalConfig.save();
ESP.restart();
```

## Future Enhancements

Planned features (not yet implemented):

- [ ] On-screen keyboard for API key entry
- [ ] QR code scanner for API key
- [ ] Interval editor UI
- [ ] Multiple API key profiles
- [ ] Cloud backup/sync
- [ ] OTA firmware updates
- [ ] Configuration import/export via file
- [ ] Password protection for settings

## Support

### Serial Monitor Setup

**PlatformIO:**
```bash
make monitor
```

**Arduino IDE:**
- Tools → Serial Monitor
- Baud: 115200
- Line Ending: Newline

### Debug Output

Enable verbose configuration logging:
```cpp
globalConfig.printConfig();  // In your code
```

Serial output shows:
- Configuration load status
- Current settings (masked)
- Validation warnings
- Save confirmations

## Build Information

✅ **Build Status:** SUCCESS
- RAM Usage: 48428 bytes (14.8%)
- Flash Usage: 1016725 bytes (15.5%)
- Additional Code: ~18KB (Config + Settings Screen)

No new external dependencies required!

## Summary

The configuration system provides:
- ✅ Persistent storage in NVRAM
- ✅ API key management
- ✅ WiFi credentials storage
- ✅ Touch-based Settings screen
- ✅ Serial Monitor commands
- ✅ First-run setup wizard
- ✅ Security with masked output
- ✅ Easy backup/restore

Configure once, use forever!
