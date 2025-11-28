# ⚡ Quick Start Guide

**Get your Bitcoin Dashboard running in 5 minutes!**

## 1️⃣ Get Gemini API Key (2 min)

1. Visit: https://makersuite.google.com/app/apikey
2. Sign in with Google
3. Click "Create API Key"
4. Copy key (starts with `AIza`)

## 2️⃣ Flash Firmware (1 min)

```bash
# Connect ESP32 via USB
make flash
```

## 3️⃣ Configure API Key (1 min)

```bash
# Open serial monitor
make monitor

# Set your API key (paste your actual key)
SET_GEMINI_KEY=AIzaSyAbCdEfGhIjKlMnOpQrStUvWxYz1234567

# Response:
✓ Gemini API key saved successfully!
```

## 4️⃣ Connect WiFi (1 min)

- Touch screen shows WiFi networks
- Tap your network
- Enter password with on-screen keyboard
- Dashboard loads automatically

## 5️⃣ Done! 🎉

**Dashboard Features:**
- 📊 Live BTC price, blocks, mempool, fees
- 🗞️ Swipe left → AI-powered news
- ⚙️ Tap gear icon → Settings
- 📡 Tap WiFi icon → Change network

---

## Commands Cheat Sheet

```bash
# Setup
SET_GEMINI_KEY=your-key    # Configure API key
STATUS                      # View configuration

# Tools
SCREENSHOT                 # Capture display buffer
make screenshot            # Automated screenshot capture

# Maintenance
RESET_CONFIG               # Factory reset
HELP                       # Show all commands
```

**Available Serial Commands:**
- `SET_GEMINI_KEY=xxx` - Set API key securely
- `STATUS` - View device status (WiFi, memory, uptime, config)
- `SCREENSHOT` - Capture display buffer to `.tmp/screenshot_*.ppm`
- `RESET_CONFIG` - Erase all configuration (factory reset)
- `HELP` - Show all available commands

---

## Navigation

- **Swipe Left** (on Dashboard) → News screen
- **Swipe Right** (on News) → Dashboard
- **Tap Gear** → Settings
- **Tap WiFi** → WiFi settings
- **Tap @** (on News) → Refresh news

---

## Troubleshooting

**Key not working?**
```bash
STATUS  # Check if key is set
SET_GEMINI_KEY=your-key  # Reset key
```

**WiFi not connecting?**
- ESP32-S3 only supports 2.4GHz networks
- Tap WiFi icon to reconfigure

**News not loading?**
- Check WiFi connection
- Verify API key in Settings
- Tap @ button to retry

**Screenshot not working?**
- Make sure firmware is uploaded: `make upload`
- Check device is connected: `make devices`
- Try manual mode: `make screenshot-interactive` then type `SCREENSHOT`
- Requires Python pyserial: `pip3 install pyserial`

---

## Documentation

- **Full Setup:** [docs/examples/set-keys-example.md](docs/examples/set-keys-example.md)
- **Configuration:** [docs/configuration-system-guide.md](docs/configuration-system-guide.md)
- **Security:** [docs/secure-key-storage.md](docs/secure-key-storage.md)
- **Screenshots:** [scripts/README.md](scripts/README.md)

---

**Need help?** Check serial monitor output at 115200 baud for detailed logs.

**Ready to go!** 🚀
