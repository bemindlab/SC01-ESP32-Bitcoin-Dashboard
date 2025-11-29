# Single Screen Mode Guide

**Version:** v1.0.0-beta | **Flash Savings:** ~51KB | **Status:** ✅ Implemented

---

## Quick Overview

Single Screen Mode is a memory-optimized build that removes all screens except the Dashboard, saving approximately 51KB of flash memory. Perfect for headless deployments where configuration is done once via serial commands.

**In 30 seconds:**
- Dashboard-only display (price, blocks, mempool, fees)
- No touch navigation - serial commands only
- ~51KB flash savings (1,032,021 → 981,065 bytes)
- All Bitcoin data still updates every 30-60s
- Ideal for production deployments

---

## Table of Contents

1. [What Is Single Screen Mode?](#what-is-single-screen-mode)
2. [Quick Start](#quick-start)
3. [When To Use It](#when-to-use-it)
4. [Feature Comparison](#feature-comparison)
5. [Building & Flashing](#building--flashing)
6. [Configuration](#configuration)
7. [Serial Commands](#serial-commands)
8. [Memory Details](#memory-details)
9. [Troubleshooting](#troubleshooting)
10. [Technical Details](#technical-details)

---

## What Is Single Screen Mode?

A compile-time build variant that strips out all UI screens except the main Dashboard, disables touch navigation, and provides serial-only configuration.

### What You Keep

✅ **Full Dashboard Display**
- Live BTC price (USD/EUR)
- Latest block height & TX count
- Mempool stats (pending TXs, size MB)
- Fee rates (Fast/Medium/Slow sat/vB)
- Auto-refresh every 30-60 seconds

✅ **Core Features**
- WiFi connectivity (via serial)
- mempool.space API integration
- NVRAM secure storage
- Serial command interface
- Full display rendering

### What You Lose

❌ **Removed Screens**
- WiFi Scan & Selection → use `SET_WIFI` command
- Settings Screen → use `SET_*` commands
- BTC News Screen → not available
- Trading Suggestions → not available

❌ **Removed UI**
- Swipe left/right navigation
- WiFi icon touch button
- Settings gear icon button
- On-screen keyboard

---

## Quick Start

### 5-Minute Setup

```bash
# 1. Build and flash Single Screen Mode
make upload-single

# 2. Open serial monitor
make monitor

# 3. Configure WiFi (2.4GHz only)
SET_WIFI=YourNetworkName,YourPassword

# 4. Verify
STATUS

# Done! Dashboard shows live Bitcoin data
```

**Expected Response:**
```
✓ WiFi credentials saved to NVRAM
✓ Connected to YourNetworkName
IP: 192.168.1.100
Dashboard updating every 30-60s
```

---

## When To Use It

### ✅ Use Single Screen Mode For:

**Memory-Constrained Projects**
- Adding custom features that need flash space
- Running near the 6.5MB flash limit
- Maximum memory available for your code

**Production Deployments**
- Configure once, deploy many
- Headless operation (no user interaction)
- Monitoring-only installations
- Embedded in larger systems

**Simplified Operations**
- No touch interface needed
- Serial configuration only
- Lower complexity = fewer bugs
- Easier remote management

### ❌ Use Standard Mode For:

**Interactive Use Cases**
- Users need to change WiFi
- Touch-based configuration required
- News and settings screens wanted

**Development**
- Testing all features
- UI iteration and design
- Full feature demonstration

---

## Feature Comparison

| Aspect | Standard Mode | Single Screen Mode |
|--------|--------------|-------------------|
| **Memory** | | |
| Flash Usage | 1,032,021 bytes (15.7%) | 981,065 bytes (15.0%) |
| Flash Saved | - | **51KB (50,956 bytes)** |
| RAM Usage | 48,444 bytes (14.8%) | 48,412 bytes (14.8%) |
| | | |
| **Screens** | | |
| Dashboard | ✅ Always | ✅ Always |
| WiFi Setup | ✅ Touch UI | ❌ Serial only |
| Settings | ✅ Touch UI | ❌ Serial only |
| BTC News | ✅ Swipe left | ❌ Not available |
| Trading | 📋 Planned | ❌ Not available |
| | | |
| **User Interface** | | |
| Touch Navigation | ✅ Full support | ❌ Disabled |
| Swipe Gestures | ✅ Left/Right | ❌ Disabled |
| On-Screen Keyboard | ✅ Password input | ❌ Not included |
| Button Icons | ✅ WiFi/Settings | 🔒 Disabled |
| | | |
| **Configuration** | | |
| WiFi Setup | Touch + Serial | **Serial only** |
| API Keys | Touch + Serial | **Serial only** |
| Settings UI | ✅ Visual | ❌ Command line |
| | | |
| **Bitcoin Data** | | |
| Price Display | ✅ Real-time | ✅ Real-time |
| Block Info | ✅ Every 60s | ✅ Every 60s |
| Mempool Stats | ✅ Every 30s | ✅ Every 30s |
| Fee Rates | ✅ Every 30s | ✅ Every 30s |

---

## Building & Flashing

### Method 1: Makefile (Recommended)

```bash
# Build only
make build-single

# Flash to device
make upload-single

# Build + Flash
make all-single

# Flash + Monitor
make upload-single && make monitor
```

### Method 2: PlatformIO CLI

```bash
# Build
pio run -e sc01_plus_single

# Flash
pio run -e sc01_plus_single --target upload

# Flash + Monitor
pio run -e sc01_plus_single --target upload && pio device monitor
```

### Build Output

```
Building in Single Screen Mode...
Saves ~51KB flash compared to multi-screen mode

Compiling...
Linking...

Advanced Memory Usage:
RAM:   [=         ]  14.8% (used 48,412 bytes from 327,680 bytes)
Flash: [==        ]  15.0% (used 981,065 bytes from 6,553,600 bytes)

======================== [SUCCESS] ========================
```

**Memory Comparison:**
- Standard: 1,032,021 bytes → Single Screen: 981,065 bytes
- **Saved: 50,956 bytes (~51KB)**

---

## Configuration

### Initial WiFi Setup

Single Screen Mode **requires** serial configuration for WiFi:

```bash
# 1. Connect via USB
make monitor

# 2. Set WiFi credentials (no spaces around comma)
SET_WIFI=NetworkName,Password123

# Response:
✓ WiFi credentials saved to NVRAM
✓ Connecting to NetworkName...
✓ Connected!
  IP Address: 192.168.1.100
  Signal: -55 dBm (Good)
```

**WiFi Requirements:**
- ⚠️ **2.4GHz only** (ESP32-S3 doesn't support 5GHz)
- Format: `SET_WIFI=SSID,PASSWORD`
- No spaces around the comma
- Password can contain spaces
- Stored securely in NVRAM
- Persists across reboots

### API Key Configuration

For future features (News, Trading Suggestions):

```bash
# Gemini API Key (for BTC News)
SET_GEMINI_KEY=AIzaSyAbCdEfGhIjKlMnOpQrStUvWxYz1234567

# OpenAI API Key (for Trading Suggestions)
SET_OPENAI_KEY=sk-proj-abcdefghijklmnopqrstuvwxyz123456

# Response:
✓ Gemini API key saved successfully!
✓ Key stored in NVRAM (encrypted)
```

### Check Configuration

```bash
STATUS
```

**Example Output:**
```
=== Bitcoin Dashboard Status ===

Hardware:
  Chip: ESP32-S3-WROVER-N16R2
  Flash: 16MB | PSRAM: 2MB (OPI)
  Free Heap: 245KB
  Uptime: 5h 23m 15s

WiFi:
  Status: Connected ✓
  SSID: MyHomeNetwork
  IP: 192.168.1.100
  Signal: -45 dBm (Excellent)
  MAC: AA:BB:CC:DD:EE:FF

Config:
  Gemini Key: SET (AIza...4567)
  OpenAI Key: NOT SET
  Build Mode: Single Screen

Bitcoin:
  Last Update: 5s ago
  Price: $102,543 USD
  Block: #875,432
  Mempool: 12,345 TXs
```

---

## Serial Commands

### WiFi Management

```bash
SET_WIFI=SSID,PASSWORD    # Configure WiFi (2.4GHz only)
STATUS                     # Show WiFi connection status
```

### API Keys

```bash
SET_GEMINI_KEY=xxx        # Set Gemini API key (for News)
SET_OPENAI_KEY=xxx        # Set OpenAI API key (for Trading)
```

### System Commands

```bash
STATUS                     # Show complete system status
RESET_CONFIG              # Factory reset (erase all config)
HELP                      # Show all available commands
```

### Debug Commands

```bash
SCREENSHOT                # Capture display to .tmp/screenshot_*.ppm
```

### Serial Monitor Setup

**Baud Rate:** 115200
**Line Ending:** Newline (NL) or Both NL & CR

```bash
# Open monitor
make monitor

# With exception decoder (for crashes)
make debug-monitor

# List connected devices
make devices
```

---

## Memory Details

### Flash Breakdown

**What Was Removed (~51KB total):**

1. **WiFi Scan Screen** (~15KB)
   - Network list UI
   - Password keyboard
   - Connection progress
   - Touch handlers

2. **Settings Screen** (~10KB)
   - Settings menu UI
   - Option rendering
   - API key editor
   - Touch handlers

3. **BTC News Screen** (~8KB)
   - News article UI
   - Gemini AI integration
   - Refresh button
   - Data structures

4. **Swipe Navigation** (~12KB)
   - Gesture detection
   - Screen transitions
   - State management
   - Touch routing

5. **Touch Controls** (~6KB)
   - Button detection
   - Icon handlers
   - Feedback animations

**What Remains (shared components):**
- Display buffer: ~150KB (PSRAM)
- LovyanGFX library: ~200KB
- WiFi/HTTP stack: ~300KB
- Core logic: ~100KB
- Fonts: ~50KB

### RAM Usage

- Standard: 48,444 bytes (14.8%)
- Single Screen: 48,412 bytes (14.8%)
- **Saved: Only 32 bytes**

**Why minimal RAM savings?**
Display buffers (largest RAM users) are still needed for the Dashboard. Most screen-specific data was allocated on-demand anyway.

### Flash Utilization

```
6,553,600 bytes total flash (6.4MB)

Standard Mode:
[==              ] 15.7% (1,032,021 bytes)
└─ Available: 5,521,579 bytes (84.3%)

Single Screen Mode:
[==              ] 15.0% (981,065 bytes)
└─ Available: 5,572,535 bytes (85.0%)

Difference: +50,956 bytes free (+0.7%)
```

---

## Troubleshooting

### WiFi Won't Connect

**Problem:** `SET_WIFI` command not working

**Solutions:**

1. **Check 2.4GHz network**
   ```
   ESP32-S3 ONLY supports 2.4GHz WiFi
   5GHz networks will not work
   ```

2. **Verify format**
   ```bash
   ✓ Correct:   SET_WIFI=MyNetwork,Pass123
   ✗ Wrong:     SET_WIFI=MyNetwork, Pass123  (space after comma)
   ✗ Wrong:     SET_WIFI="MyNetwork","Pass123"  (no quotes)
   ```

3. **Check password**
   ```bash
   # Password is case-sensitive
   # Can contain spaces: SET_WIFI=Net,My Pass 123
   ```

4. **Factory reset and retry**
   ```bash
   RESET_CONFIG
   SET_WIFI=YourNetwork,YourPassword
   STATUS
   ```

### Serial Commands Not Responding

**Check baud rate:**
```bash
# Must be 115200
pio device monitor -b 115200
# or
make monitor
```

**Check line endings:**
- Arduino IDE: Set to "Newline" or "Both NL & CR"
- PlatformIO: Should work by default
- Terminal: Add `\n` at end of commands

**Verify device connected:**
```bash
make devices
# or
ls /dev/cu.usb*  # macOS
ls /dev/ttyUSB*  # Linux
```

### Dashboard Not Updating

**Check WiFi status:**
```bash
STATUS
# Look for "WiFi: Connected ✓"
```

**Check serial output:**
```bash
make monitor
# Look for API fetch errors
```

**Possible causes:**
1. WiFi disconnected → reconfigure with `SET_WIFI`
2. mempool.space API down → check website
3. Internet connectivity issue → check router

### Switch Back to Standard Mode

```bash
# Upload Standard Mode firmware
make upload

# Configuration (WiFi, keys) persists in NVRAM
# No need to reconfigure
```

### Out of Memory Errors

Even with Single Screen Mode, if you're adding features:

1. **Increase update intervals** (in `src/main.cpp`):
   ```cpp
   const unsigned long PRICE_INTERVAL = 60000;   // 60s instead of 30s
   const unsigned long BLOCK_INTERVAL = 120000;  // 120s instead of 60s
   ```

2. **Reduce buffer sizes** (in `src/main.cpp`):
   ```cpp
   static lv_color_t buf1[480 * 20];  // Reduce from 480 * 40
   ```

3. **Use smaller fonts** (in screen files)

---

## Technical Details

### Preprocessor Flag

**platformio.ini:**
```ini
[env:sc01_plus_single]
extends = env:sc01_plus
build_flags =
    ${env:sc01_plus.build_flags}
    -DSINGLE_SCREEN_MODE        # <-- Enables single screen mode
```

### Conditional Compilation

Code is wrapped in `#ifdef` blocks:

**src/screens/ScreenManager.h:**
```cpp
enum Screen {
#ifndef SINGLE_SCREEN_MODE
    SCREEN_WIFI_SCAN,
    SCREEN_WIFI_CONNECT,
#endif
    SCREEN_DASHBOARD,
#ifndef SINGLE_SCREEN_MODE
    SCREEN_BTC_NEWS,
    SCREEN_TRADING_SUGGESTION,
    SCREEN_SETTINGS
#endif
};
```

**src/main.cpp:**
```cpp
#ifdef SINGLE_SCREEN_MODE
    Serial.println("SINGLE_SCREEN_MODE: Use serial commands");
    Serial.println("WiFi: SET_WIFI=SSID,Password");
    screenManager->switchScreen(SCREEN_DASHBOARD);
#else
    screenManager->switchScreen(SCREEN_WIFI_SCAN);
#endif
```

**src/screens/BTCDashboardScreen.cpp:**
```cpp
void BTCDashboardScreen::handleTouch(int16_t x, int16_t y) {
#ifdef SINGLE_SCREEN_MODE
    Serial.println("SINGLE_SCREEN_MODE: Touch disabled");
    Serial.println("Use: STATUS, SET_*, HELP");
#else
    // Handle WiFi icon tap
    if (isWiFiIconTouched(x, y)) {
        manager->switchScreen(SCREEN_WIFI_SCAN);
    }
    // Handle Settings icon tap
    if (isSettingsIconTouched(x, y)) {
        manager->switchScreen(SCREEN_SETTINGS);
    }
#endif
}
```

### Modified Files

**Build Configuration:**
- `platformio.ini` - Added `sc01_plus_single` environment

**Screen System:**
- `src/screens/ScreenManager.h` - Conditional screen enum
- `src/screens/ScreenManager.cpp` - Conditional swipe handling
- `src/screens/BTCDashboardScreen.cpp` - Conditional touch handlers
- `src/screens/SettingsScreen.cpp` - Conditional WiFi navigation

**Main Application:**
- `src/main.cpp` - Conditional initial screen

**Documentation:**
- `README.md` - Build mode comparison
- `Makefile` - Single screen targets
- `guides/quick-start-guide.md` - Setup instructions
- This guide

### Build Targets

```bash
# Makefile targets
make build-single      # Build single screen firmware
make upload-single     # Upload to device
make all-single        # Build and upload

# PlatformIO environments
pio run -e sc01_plus          # Standard mode
pio run -e sc01_plus_single   # Single screen mode
```

### Verification

**Check if Single Screen Mode is running:**
```bash
STATUS
```

Look for:
```
Build Mode: Single Screen
```

**Test touch (should be disabled):**
Tap where WiFi/Settings icons would be:
```
SINGLE_SCREEN_MODE: Touch disabled
Use: STATUS, SET_*, HELP
```

---

## Best Practices

### Pre-Deployment Checklist

- [ ] Build and test Single Screen Mode locally
- [ ] Configure WiFi via serial and verify connection
- [ ] Set API keys if needed
- [ ] Run `STATUS` to verify all configuration
- [ ] Test across multiple reboots
- [ ] Document WiFi credentials securely
- [ ] Test data updates (wait 60s)
- [ ] Verify display shows correct data

### Production Deployment

1. **Flash firmware:**
   ```bash
   make upload-single
   ```

2. **Configure device:**
   ```bash
   make monitor
   SET_WIFI=ProductionSSID,ProductionPassword
   SET_GEMINI_KEY=your-production-key
   STATUS  # Verify
   ```

3. **Document setup:**
   - Record WiFi credentials used
   - Store API keys in password manager
   - Note device IP address
   - Document serial port if accessible

4. **Plan for updates:**
   - Keep USB access if possible
   - Consider OTA updates for remote devices
   - Have rollback plan (Standard Mode firmware ready)

### Development Workflow

**Recommended approach:**

1. **Develop in Standard Mode**
   - Full UI for testing
   - Touch interface for quick config
   - All screens accessible

2. **Test in Single Screen Mode**
   - Build periodically: `make build-single`
   - Verify serial commands work
   - Check memory usage

3. **Deploy in Single Screen Mode**
   - For production if memory is tight
   - Configure via serial before deployment
   - Monitor via serial logs

---

## Related Documentation

- **[Quick Start Guide](quick-start-guide.md)** - 5-minute setup for both modes
- **[README.md](../../README.md)** - Complete project overview
- **[Configuration Guide](configuration-guide.md)** - Serial command details
- **[Development Plan](../development/development-plan.md)** - Project roadmap
- **[Documentation Index](../INDEX.md)** - All documentation

---

## Changelog

### v1.0.0-beta (2025-11-28)

- ✅ Initial Single Screen Mode implementation
- ✅ Conditional compilation system
- ✅ Memory optimization (~51KB flash savings)
- ✅ Serial-only configuration (SET_WIFI command)
- ✅ Makefile targets (build-single, upload-single, all-single)
- ✅ Disabled touch navigation and swipe gestures
- ✅ Updated all screens with conditional compilation
- ✅ Complete documentation

**Tested on:** Panlee SC01 Plus (ESP32-S3-WROVER-N16R2)
**Build Status:** ✅ Both modes compile and run successfully

---

**Document Version:** 2.0  
**Last Updated:** 2025-11-28  
**Status:** Complete  
**Flash Savings:** ~51KB (50,956 bytes)
