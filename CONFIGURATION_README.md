# 🔐 Secure Configuration System

**No hardcoded API keys required!** Configure everything via Serial Monitor or Settings screen.

## Quick Start

### 1. Upload Firmware
```bash
make flash
```

### 2. Configure API Key (via Serial Monitor)
```bash
# Open serial monitor
make monitor

# Set your Gemini API key
SET_GEMINI_KEY=AIzaSyAbCdEfGhIjKlMnOpQrStUvWxYz1234567

# Response:
✓ Gemini API key saved successfully!
```

### 3. Done! 🎉
Your key is now:
- ✅ Stored securely in ESP32 NVRAM
- ✅ Persists across reboots
- ✅ Never visible in source code
- ✅ Masked in all output

## Features

### 🔒 Security
- **No Hardcoding** - Keys never in source code
- **Encrypted Storage** - ESP32 NVRAM with optional flash encryption
- **Masked Output** - Keys show as `AIza...4567` in serial
- **Runtime Config** - Change without recompilation

### ⚙️ Settings Screen
Access via gear icon on Dashboard:
- **Gemini API Key** - View status, configure via Serial
- **WiFi Settings** - Auto-saves credentials
- **Update Intervals** - Customize refresh rates
- **Reset Configuration** - Factory reset

### 🖥️ Serial Commands

| Command | Description | Usage |
|---------|-------------|-------|
| `SET_GEMINI_KEY=xxx` | Set API key securely | `SET_GEMINI_KEY=AIza...` |
| `STATUS` | View config (keys masked) | Shows WiFi, memory, uptime, config |
| `SCREENSHOT` | Capture display buffer | Saves to `.tmp/screenshot_*.ppm` |
| `RESET_CONFIG` | Erase all settings | Factory reset configuration |
| `HELP` | Show all commands | Lists available commands |

**Screenshot Capture:**
```bash
# Automated capture (recommended)
make screenshot

# Manual capture via serial monitor
make monitor
SCREENSHOT  # Type this command

# Interactive mode
make screenshot-interactive
```

## Configuration Storage

All settings stored in ESP32 NVRAM (persists across power cycles):

| Setting | Default | Configurable Via |
|---------|---------|------------------|
| Gemini API Key | (empty) | Serial / Settings |
| WiFi SSID | (empty) | WiFi Scan Screen |
| WiFi Password | (empty) | WiFi Scan Screen |
| Price Interval | 30s | Code (UI coming) |
| Block Interval | 60s | Code (UI coming) |
| Mempool Interval | 30s | Code (UI coming) |

## Documentation

- **📖 Full Guide:** [docs/configuration-system-guide.md](docs/configuration-system-guide.md)
- **🔐 Security Details:** [docs/secure-key-storage.md](docs/secure-key-storage.md)
- **🗞️ News Feature:** [docs/btc-news-setup-guide.md](docs/btc-news-setup-guide.md)
- **📸 Screenshots:** [scripts/README.md](scripts/README.md) - Capture display via serial

## Migration from Hardcoded Keys

**Old Way (Not Recommended):**
```cpp
// src/api/GeminiClient.h
#define GEMINI_API_KEY "your-key-here"  // ❌ Hardcoded
```

**New Way (Recommended):**
```bash
# Via Serial Monitor
SET_GEMINI_KEY=your-key-here  # ✅ Secure storage
```

**Benefits:**
- ✅ No recompilation needed to change key
- ✅ Keys never committed to git
- ✅ Safer for production deployment
- ✅ Easy to update or rotate keys

## Security Best Practices

1. **Never commit API keys to git** (already protected by .gitignore)
2. **Use Serial Monitor to configure keys** (not hardcoding)
3. **Enable ESP32 flash encryption** for production (optional)
4. **Monitor API usage** in Google Cloud Console
5. **Rotate keys periodically** via `SET_GEMINI_KEY=new-key`

## Troubleshooting

### Key Not Loading?
```bash
# Check status
STATUS

# Reconfigure if needed
SET_GEMINI_KEY=your-key

# Verify
STATUS
```

### Configuration Reset?
```bash
# Reset all settings
RESET_CONFIG

# Reconfigure
SET_GEMINI_KEY=your-key
```

### WiFi Not Connecting?
- Stored credentials auto-connect on boot
- Failed connection falls back to WiFi Scan screen
- Reconfigure via Settings → WiFi Settings

## .gitignore Protection

The following patterns are now ignored by git:
```
secrets.h
credentials.h
config.ini
.env
*.key
*.pem
*.crt
api_keys.txt
config_backup*.txt
settings_export*.json
```

This prevents accidental commits of sensitive data.

## Build Status

✅ **Implementation Complete**
- RAM: 48428 bytes (14.8%)
- Flash: 1.02 MB (15.5%)
- Config System: +18KB only

## Quick Command Reference

```bash
# Setup & Configuration
make flash                          # Upload firmware
make monitor                        # Open serial monitor
SET_GEMINI_KEY=AIza...             # Configure API key

# Development Tools
make screenshot                     # Capture display buffer
make screenshot-interactive         # Manual screenshot mode
make test-native                   # Run unit tests (53 tests)

# Device Status
STATUS                              # Show config & device info
HELP                               # List all serial commands

# Maintenance
RESET_CONFIG                       # Factory reset
make clean                         # Clean build files
make devices                       # List USB devices
```

## Summary

Your Bitcoin Dashboard now features:
1. ✅ **Secure Configuration System** - No hardcoded keys!
2. ✅ **NVRAM Storage** - Persistent and encrypted
3. ✅ **Settings Screen** - Touch-based UI
4. ✅ **Serial Commands** - Easy configuration
5. ✅ **WiFi Auto-Connect** - Saved credentials
6. ✅ **BTC News with Gemini AI** - Powered by your secure key

**Configure once, use forever!** 🚀
