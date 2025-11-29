# API Key Configuration Examples

**Complete guide with real-world examples for configuring your Bitcoin Dashboard**

## Table of Contents

1. [First-Time Setup](#first-time-setup)
2. [Getting Your Gemini API Key](#getting-your-gemini-api-key)
3. [Setting the API Key](#setting-the-api-key)
4. [Verification](#verification)
5. [Common Scenarios](#common-scenarios)
6. [Troubleshooting](#troubleshooting)

---

## First-Time Setup

### Step 1: Get Hardware Ready

```bash
# Connect ESP32-S3 to computer via USB
# LED should light up indicating power
```

### Step 2: Upload Firmware

```bash
# Navigate to project directory
cd btc-dashboard

# Build and upload firmware
make flash

# Expected output:
Building project...
Uploading firmware...
Writing at 0x00010000... (100%)
Hard resetting via RTS pin...
========================= [SUCCESS] =========================
```

### Step 3: Open Serial Monitor

```bash
# Open serial monitor at 115200 baud
make monitor

# Expected output:
Bitcoin Dashboard - Bootstrap Mode

=== Initializing Configuration ===
Configuration loaded successfully!

--- Current Configuration ---
Gemini API Key: NOT SET
WiFi SSID: NOT SET
...
First Run: YES
-----------------------------

⚠️  FIRST RUN DETECTED
Configure via Serial Monitor:
  SET_GEMINI_KEY=your-api-key-here

Display initialized!
Touch initialized successfully!
```

---

## Getting Your Gemini API Key

### Step 1: Visit Google AI Studio

1. **Open browser:** https://makersuite.google.com/app/apikey
2. **Sign in** with your Google account
3. **Click** "Create API Key"

### Step 2: Choose Project

**Option A: Create in new project (Recommended)**
- Click "Create API key in new project"
- Auto-generates project name
- Isolated from other projects

**Option B: Use existing project**
- Select from dropdown
- Uses existing Google Cloud project

### Step 3: Copy API Key

**Your key will look like:**
```
AIzaSyAbCdEfGhIjKlMnOpQrStUvWxYz1234567
```

**Key Characteristics:**
- Starts with: `AIza`
- Length: 39 characters
- Contains: Letters (upper/lower), numbers
- No spaces or special characters

### Step 4: Secure Your Key

⚠️ **IMPORTANT:**
- Don't share publicly
- Don't commit to git
- Don't post in forums
- Keep in password manager

---

## Setting the API Key

### Method 1: Serial Monitor (Recommended)

**Example 1: Basic Configuration**

```bash
# In Serial Monitor, type:
SET_GEMINI_KEY=AIzaSyAbCdEfGhIjKlMnOpQrStUvWxYz1234567

# Press Enter/Return

# Expected response:
Gemini API key updated
✓ Gemini API key saved successfully!
```

**Example 2: With Copy-Paste**

```bash
# 1. Copy your API key from Google AI Studio
AIzaSyAbCdEfGhIjKlMnOpQrStUvWxYz1234567

# 2. Type command in Serial Monitor:
SET_GEMINI_KEY=

# 3. Paste your key right after the equals sign
SET_GEMINI_KEY=AIzaSyAbCdEfGhIjKlMnOpQrStUvWxYz1234567

# 4. Press Enter

# Response:
✓ Gemini API key saved successfully!
```

**Example 3: Using Make Monitor**

```bash
# Terminal 1: Start monitor
make monitor

# You'll see:
--- Available filters and text transformations: ...
--- More details at https://bit.ly/pio-monitor-filters
--- Quit: Ctrl+C

Bitcoin Dashboard - Bootstrap Mode
...

# Terminal 1: Type command directly
SET_GEMINI_KEY=AIzaSyAbCdEfGhIjKlMnOpQrStUvWxYz1234567

# Press Enter, see confirmation:
✓ Gemini API key saved successfully!
```

### Method 2: PlatformIO Device Monitor

```bash
# Start PlatformIO monitor
pio device monitor -b 115200

# Type command:
SET_GEMINI_KEY=AIzaSyAbCdEfGhIjKlMnOpQrStUvWxYz1234567

# Press Enter
```

### Method 3: Arduino Serial Monitor

```
1. Open Arduino IDE
2. Tools → Serial Monitor
3. Set baud rate: 115200
4. Set line ending: Newline
5. Type in input field:
   SET_GEMINI_KEY=AIzaSyAbCdEfGhIjKlMnOpQrStUvWxYz1234567
6. Click Send or press Enter
```

### Method 4: Screen Command (macOS/Linux)

```bash
# Find USB port
ls /dev/tty.* | grep usb

# Example output:
/dev/tty.usbserial-0001

# Connect with screen
screen /dev/tty.usbserial-0001 115200

# Type command:
SET_GEMINI_KEY=AIzaSyAbCdEfGhIjKlMnOpQrStUvWxYz1234567

# Press Enter

# To exit screen: Ctrl+A, then K, then Y
```

### Method 5: Minicom (Linux)

```bash
# Install minicom
sudo apt-get install minicom

# Configure
sudo minicom -s
# Serial Device: /dev/ttyUSB0
# Baud Rate: 115200
# Save and exit

# Connect
minicom

# Type command:
SET_GEMINI_KEY=AIzaSyAbCdEfGhIjKlMnOpQrStUvWxYz1234567

# Press Enter

# Exit: Ctrl+A, then X
```

---

## Verification

### Check Configuration Status

```bash
# Command:
STATUS

# Expected output:
WiFi: Disconnected
Free Heap: 245678 bytes
Uptime: 45 seconds

--- Current Configuration ---
Gemini API Key: AIza...4567
WiFi SSID: NOT SET
WiFi Password: ****
Price Interval: 30000 ms (30 sec)
Block Interval: 60000 ms (60 sec)
Mempool Interval: 30000 ms (30 sec)
First Run: NO
-----------------------------
```

### Verify Key is Masked

**Notice:**
- Full key: `AIzaSyAbCdEfGhIjKlMnOpQrStUvWxYz1234567`
- Displayed as: `AIza...4567`
- Only first 4 and last 4 characters shown
- This is a security feature!

### Test API Connection

```bash
# After connecting to WiFi and navigating to News screen
# Tap the @ (refresh) button

# Serial output should show:
✓ Using Gemini API key from configuration
Gemini API Endpoint: https://generativelanguage.googleapis.com/...
Sending request to Gemini API...
HTTP Response code: 200
Response received, parsing...
News fetched successfully!
```

---

## Common Scenarios

### Scenario 1: Brand New Device

**Situation:** Just flashed firmware for the first time

**Steps:**

```bash
# 1. Device boots, shows:
⚠️  FIRST RUN DETECTED
Configure via Serial Monitor:
  SET_GEMINI_KEY=your-api-key-here

# 2. Get API key from Google AI Studio
# (Follow steps in "Getting Your Gemini API Key" section)

# 3. Configure key:
SET_GEMINI_KEY=AIzaSyAbCdEfGhIjKlMnOpQrStUvWxYz1234567

# 4. Verify:
STATUS

# 5. Connect WiFi via touch screen
# (Touch WiFi Scan screen, select network, enter password)

# 6. Done! Dashboard loads with live data
```

### Scenario 2: Updating API Key

**Situation:** Need to rotate or change API key

**Steps:**

```bash
# 1. Open serial monitor:
make monitor

# 2. Set new key (overwrites old one):
SET_GEMINI_KEY=AIzaSyNewKeyHere123456789AbCdEfGhIjKl

# Response:
Gemini API key updated
✓ Gemini API key saved successfully!

# 3. Verify:
STATUS

# Shows:
Gemini API Key: AIza...jKl

# 4. No restart needed! New key active immediately
```

### Scenario 3: Multiple Devices

**Situation:** Setting up 3 identical devices

**Device 1:**
```bash
make flash
make monitor
SET_GEMINI_KEY=AIzaSyDevice1Key123456789AbCdEfGhIj
STATUS
# Verify, then disconnect
```

**Device 2:**
```bash
# Reconnect USB to Device 2
make flash
make monitor
SET_GEMINI_KEY=AIzaSyDevice2Key987654321ZyXwVuTsRq
STATUS
# Verify, then disconnect
```

**Device 3:**
```bash
# Reconnect USB to Device 3
make flash
make monitor
SET_GEMINI_KEY=AIzaSyDevice3KeyAaBbCcDdEeFfGgHhIiJj
STATUS
# Verify, done!
```

**Note:** Each device can have the same or different keys depending on your needs.

### Scenario 4: Factory Reset

**Situation:** Selling device or troubleshooting

**Steps:**

```bash
# 1. Connect to device:
make monitor

# 2. Reset all configuration:
RESET_CONFIG

# Response:
Resetting configuration...
✓ Configuration reset complete!

# 3. Verify:
STATUS

# Shows:
--- Current Configuration ---
Gemini API Key: NOT SET
WiFi SSID: NOT SET
...
First Run: YES
-----------------------------

# 4. Device now in factory state
# 5. Ready to configure fresh or ship to new user
```

### Scenario 5: Backup Before Reset

**Situation:** Want to save config before resetting

**Steps:**

```bash
# 1. Check current config:
STATUS

# 2. Copy output to file:
# macOS/Linux:
make monitor | tee config_backup.txt

# Or manually copy from terminal

# Example backup:
# --- Current Configuration ---
# Gemini API Key: AIza...4567
# WiFi SSID: MyHomeNetwork
# Price Interval: 30000 ms (30 sec)
# ...

# 3. Perform reset:
RESET_CONFIG

# 4. Later, restore from backup:
SET_GEMINI_KEY=AIzaSyAbCdEfGhIjKlMnOpQrStUvWxYz1234567
# WiFi: Reconfigure via UI

# 5. Verify:
STATUS
```

### Scenario 6: API Key Not Working

**Situation:** Key configured but API calls failing

**Steps:**

```bash
# 1. Check current key:
STATUS

# Output shows:
Gemini API Key: AIza...4567

# 2. Verify key is correct at Google AI Studio
# https://makersuite.google.com/app/apikey

# 3. If key is wrong, update:
SET_GEMINI_KEY=AIzaCorrectKeyHere123456789AbCdEf

# 4. Test immediately:
# Navigate to News screen, tap @ to refresh

# 5. Check serial for errors:
# Success:
HTTP Response code: 200
News fetched successfully!

# Failure (wrong key):
HTTP Response code: 403
API Error: HTTP 403

# 6. If still failing, check:
# - WiFi connection (must be connected)
# - Key restrictions in Google Cloud Console
# - API quota not exceeded
```

### Scenario 7: Development vs Production Keys

**Situation:** Different keys for testing and production

**Development Key (unlimited testing):**
```bash
SET_GEMINI_KEY=AIzaSyDevKey123TestingOnlyAbCdEfGhIj
```

**Production Key (restricted, monitored):**
```bash
SET_GEMINI_KEY=AIzaSyProdKey456SecureMonitoredXyZaB
```

**Switch between environments:**
```bash
# Test build:
SET_GEMINI_KEY=AIzaSyDevKey123TestingOnlyAbCdEfGhIj

# Production build:
SET_GEMINI_KEY=AIzaSyProdKey456SecureMonitoredXyZaB
```

---

## Troubleshooting

### Problem 1: Command Not Working

**Symptom:**
```bash
SET_GEMINI_KEY=AIzaSy...
# No response
```

**Solutions:**

**A. Check Serial Monitor Settings**
```
Arduino IDE:
- Line ending: "Newline" or "Both NL & CR"
- Baud rate: 115200

PlatformIO:
- Should work automatically
- Try: pio device monitor --echo
```

**B. Check Device Connection**
```bash
# Verify device is connected:
ls /dev/tty.* | grep usb

# Or on Linux:
ls /dev/ttyUSB*

# Reconnect if needed:
make monitor
```

**C. Check Command Format**
```bash
# ✅ Correct:
SET_GEMINI_KEY=AIzaSy...

# ❌ Wrong (space after =):
SET_GEMINI_KEY= AIzaSy...

# ❌ Wrong (quotes):
SET_GEMINI_KEY="AIzaSy..."

# ❌ Wrong (lowercase):
set_gemini_key=AIzaSy...
```

### Problem 2: Key Not Persisting

**Symptom:**
```bash
# Set key:
SET_GEMINI_KEY=AIzaSy...
✓ Saved successfully!

# Restart device, check:
STATUS
# Shows: NOT SET
```

**Solutions:**

**A. Check NVRAM**
```bash
# Reset and reconfigure:
RESET_CONFIG
SET_GEMINI_KEY=AIzaSy...

# Verify immediately:
STATUS
```

**B. Check for Errors**
```bash
# Look for errors in serial output:
Failed to open preferences for writing
# Indicates NVRAM issue

# Try reflashing:
make flash
# Then reconfigure
```

### Problem 3: Wrong Key Entered

**Symptom:**
```bash
# Accidentally entered wrong key:
SET_GEMINI_KEY=AIzaSyWRONGKEY12345

# News screen shows:
API Error: HTTP 403
```

**Solution:**
```bash
# Simply set correct key (overwrites):
SET_GEMINI_KEY=AIzaSyCORRECTKEY67890

# No restart needed
# Test immediately in News screen
```

### Problem 4: Can't See Key

**Symptom:**
```bash
STATUS
# Shows: AIza...4567
# Need to verify full key
```

**Explanation:**
This is intentional security! Keys are masked in output.

**Solutions:**

**A. Check Original Source**
- Look in Google AI Studio
- Check password manager
- Review backup file

**B. If Lost, Generate New Key**
```bash
# 1. Visit: https://makersuite.google.com/app/apikey
# 2. Click "Create API Key"
# 3. Copy new key
# 4. Update device:
SET_GEMINI_KEY=AIzaSyNewKey...

# Old key still works until revoked
```

### Problem 5: Special Characters in Key

**Symptom:**
```bash
# Key with special characters (shouldn't happen with Gemini)
SET_GEMINI_KEY=AIzaSy...@#$...

# Might cause parsing issues
```

**Gemini Keys Don't Have Special Characters:**
- Only: A-Z, a-z, 0-9
- No spaces, @, #, $, etc.
- If you see special characters, key might be corrupted

**Solution:**
```bash
# Copy key carefully from Google AI Studio
# Don't modify or add characters
# Gemini keys: AIza[A-Za-z0-9]{35}
```

---

## Quick Reference Card

### Essential Commands

```bash
# Set API Key
SET_GEMINI_KEY=your-api-key-here

# Check Status
STATUS

# Reset Everything
RESET_CONFIG

# Get Help
HELP
```

### Serial Monitor Setup

| Platform | Baud Rate | Line Ending |
|----------|-----------|-------------|
| Make | 115200 | Any |
| PlatformIO | 115200 | Any |
| Arduino IDE | 115200 | Newline |
| Screen | 115200 | N/A |

### API Key Format

```
Format: AIza[A-Za-z0-9]{35}
Length: 39 characters
Example: AIzaSyAbCdEfGhIjKlMnOpQrStUvWxYz1234567
```

### Success Indicators

✅ **Key Set Successfully:**
```
Gemini API key updated
✓ Gemini API key saved successfully!
```

✅ **Key Loaded:**
```
✓ Using Gemini API key from configuration
```

✅ **API Call Success:**
```
HTTP Response code: 200
News fetched successfully!
```

### Error Indicators

❌ **Key Not Set:**
```
Gemini API Key: NOT SET
⚠️  Using hardcoded Gemini API key (not configured)
```

❌ **Invalid Key:**
```
HTTP Response code: 403
API Error: HTTP 403
```

❌ **Network Issue:**
```
Network Error: Failed to connect to Gemini API
```

---

## Security Reminders

### ✅ DO:
- Store keys in password manager
- Use different keys for dev/prod
- Rotate keys periodically
- Monitor API usage
- Set usage limits in Google Cloud

### ❌ DON'T:
- Share keys publicly
- Commit keys to git
- Post keys in forums/Discord
- Email keys unencrypted
- Use same key across all projects

### 🔒 Key Protection:
- Keys stored in encrypted NVRAM
- Masked in all output (AIza...4567)
- Not visible in source code
- Protected by .gitignore
- Can be remotely revoked in Google Cloud

---

## Next Steps

After configuring your API key:

1. **Connect WiFi** - Use WiFi Scan screen
2. **View Dashboard** - See live Bitcoin data
3. **Swipe Left** - Navigate to News screen
4. **Tap @ Button** - Fetch AI-powered news
5. **Read Analysis** - Scroll through Gemini's insights

Enjoy your secure Bitcoin Dashboard! 🚀
