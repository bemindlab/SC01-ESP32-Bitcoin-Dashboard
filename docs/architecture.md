# Bitcoin Dashboard - System Architecture

## Overview

```
┌─────────────────────────────────────────────────────────────────┐
│                     Bitcoin Dashboard                            │
│              ESP32-S3 with 3.5" Touch Display                    │
└─────────────────────────────────────────────────────────────────┘
```

## System Components

### Hardware Layer
```
┌──────────────────────────────────────────────────────────────┐
│  Panlee SC01 Plus                                            │
│  ┌────────────┐  ┌──────────────┐  ┌──────────────────┐    │
│  │  ESP32-S3  │  │  ST7796 LCD  │  │  FT6X36 Touch   │    │
│  │  240MHz    │  │  480x320 IPS │  │  Capacitive     │    │
│  │  16MB Flash│  │  Landscape   │  │  I2C Interface  │    │
│  │  8MB PSRAM │  │              │  │                  │    │
│  └────────────┘  └──────────────┘  └──────────────────┘    │
└──────────────────────────────────────────────────────────────┘
```

### Software Architecture
```
┌─────────────────────────────────────────────────────────────────┐
│                        Application Layer                         │
├─────────────────┬───────────────────┬──────────────────────────┤
│  BTC Dashboard  │   BTC News        │   Settings               │
│  - Price        │   - Gemini AI     │   - API Key              │
│  - Blocks       │   - Analysis      │   - WiFi                 │
│  - Mempool      │   - Scrolling     │   - Intervals            │
│  - Fees         │   - Refresh       │   - Reset                │
└─────────────────┴───────────────────┴──────────────────────────┘
         │                  │                      │
         ▼                  ▼                      ▼
┌─────────────────────────────────────────────────────────────────┐
│                      Screen Manager                              │
│  - Touch handling      - Swipe detection      - Navigation      │
└─────────────────────────────────────────────────────────────────┘
         │                                          │
         ▼                                          ▼
┌──────────────────────────┐      ┌─────────────────────────────┐
│    Display Driver        │      │   Configuration Manager      │
│    (LovyanGFX)          │      │   (NVRAM Storage)           │
│  - 480x320 rendering    │      │  - API Keys                 │
│  - Touch input          │      │  - WiFi Credentials         │
│  - Graphics primitives  │      │  - Intervals                │
└──────────────────────────┘      └─────────────────────────────┘
         │                                          │
         ▼                                          ▼
┌──────────────────────────────────────────────────────────────────┐
│                       Network Layer                               │
│  ┌────────────────┐  ┌─────────────────┐  ┌──────────────────┐ │
│  │ mempool.space  │  │  Gemini API     │  │  WiFi Manager    │ │
│  │ - BTC Price    │  │  - AI Analysis  │  │  - Auto-connect  │ │
│  │ - Blocks       │  │  - News Gen     │  │  - Credentials   │ │
│  │ - Mempool      │  │  - Chat         │  │  - Scan          │ │
│  │ - Fees         │  │                 │  │                  │ │
│  └────────────────┘  └─────────────────┘  └──────────────────┘ │
└──────────────────────────────────────────────────────────────────┘
```

## Data Flow

### Startup Sequence
```
1. Power On
   ↓
2. Load Configuration from NVRAM
   ├─ API Keys
   ├─ WiFi Credentials
   └─ User Preferences
   ↓
3. Initialize Display
   ├─ ST7796 LCD Driver
   └─ FT6X36 Touch Controller
   ↓
4. WiFi Connection
   ├─ Auto-connect (if credentials stored)
   └─ WiFi Scan Screen (if no credentials)
   ↓
5. Load Dashboard
   ├─ Fetch BTC Data
   ├─ Update UI
   └─ Start Update Timers
```

### Real-Time Updates
```
┌─────────────────────────────────────────────────────────────┐
│                    Main Loop (10ms)                          │
└─────────────────────────────────────────────────────────────┘
         │
         ├─→ Touch Processing ─────────→ Screen Manager
         │                                     │
         │                              ┌──────┴──────┐
         │                              │             │
         │                           Tap Event    Swipe Event
         │                              │             │
         │                         Button Action  Navigation
         │
         ├─→ Timer Check (Price: 30s)
         │         │
         │         └─→ Fetch Price → Update UI
         │
         ├─→ Timer Check (Blocks: 60s)
         │         │
         │         └─→ Fetch Blocks → Update UI
         │
         ├─→ Timer Check (Mempool: 30s)
         │         │
         │         └─→ Fetch Mempool + Fees → Update UI
         │
         └─→ Serial Commands ───────→ Configuration Manager
```

### News Generation Flow
```
User Action: Swipe Left
         ↓
   News Screen Loads
         ↓
   Show "Tap @ to refresh"
         ↓
User Action: Tap @ Button
         ↓
   Show Loading Animation
         ↓
   Collect Current BTC Data
   ├─ Price: $XX,XXX
   ├─ Block: #XXXXXX
   ├─ Mempool: XX,XXX TXs
   └─ Fees: Fast/Med/Slow
         ↓
   Generate Prompt Template
   "Analyze Bitcoin market with data..."
         ↓
   Gemini API Request
   POST /v1beta/models/gemini-1.5-flash
   {
     "contents": [{
       "parts": [{"text": "prompt..."}]
     }],
     "generationConfig": {
       "temperature": 0.7,
       "maxOutputTokens": 1024
     }
   }
         ↓
   Parse JSON Response
   Extract: candidates[0].content.parts[0].text
         ↓
   Render Formatted News
   ├─ 📈 Market Summary
   ├─ 💡 Technical Analysis
   ├─ 📊 Market Sentiment
   ├─ ⚠️  Risk Assessment
   └─ 🎯 Price Outlook
         ↓
   Enable Scrolling
   User can touch-drag to scroll
```

## Configuration System

### Storage Architecture
```
┌──────────────────────────────────────────────────────────┐
│              ESP32 NVRAM (Preferences)                   │
├──────────────────────────────────────────────────────────┤
│  Namespace: "btc_dash"                                   │
│  ┌────────────────────────────────────────────────────┐ │
│  │  Key              Value           Type    Size     │ │
│  ├────────────────────────────────────────────────────┤ │
│  │  gemini_key       AIza...4567    String  39 bytes │ │
│  │  wifi_ssid        MyNetwork      String  ~20 bytes│ │
│  │  wifi_pass        ********       String  ~20 bytes│ │
│  │  price_int        30000          ULong   4 bytes  │ │
│  │  block_int        60000          ULong   4 bytes  │ │
│  │  mempool_int      30000          ULong   4 bytes  │ │
│  │  first_run        false          Bool    1 byte   │ │
│  └────────────────────────────────────────────────────┘ │
│  Total Storage: ~100 bytes                               │
│  Partition Size: 16-32 KB (plenty of space)              │
└──────────────────────────────────────────────────────────┘
```

### Configuration Lifecycle
```
┌────────────────────────────────────────────────────────────┐
│                    First Boot                              │
│  1. NVRAM empty → Load defaults                           │
│  2. First Run: TRUE                                        │
│  3. Show WiFi Scan                                         │
│  4. Prompt for API key via Serial                         │
└────────────────────────────────────────────────────────────┘
                           ↓
┌────────────────────────────────────────────────────────────┐
│                 User Configuration                         │
│  1. Serial: SET_GEMINI_KEY=xxx                            │
│  2. WiFi Scan: Select network → Enter password           │
│  3. Save to NVRAM                                          │
│  4. Set First Run: FALSE                                   │
└────────────────────────────────────────────────────────────┘
                           ↓
┌────────────────────────────────────────────────────────────┐
│              Subsequent Boots                              │
│  1. Load config from NVRAM                                │
│  2. Auto-connect WiFi                                      │
│  3. Load Dashboard immediately                             │
│  4. All settings persist                                   │
└────────────────────────────────────────────────────────────┘
                           ↓
┌────────────────────────────────────────────────────────────┐
│              Configuration Update                          │
│  1. Change via Serial or Settings screen                  │
│  2. Save to NVRAM                                          │
│  3. Apply immediately (no restart)                         │
└────────────────────────────────────────────────────────────┘
```

## Screen Navigation

### Navigation Map
```
                    ┌─────────────────┐
                    │  WiFi Scan      │
                    │  (First Boot)   │
                    └────────┬────────┘
                             │
                             ▼
                    ┌─────────────────┐
              ┌────→│  BTC Dashboard  │←────┐
              │     │                 │     │
              │     │  Swipe Left  ══╬═══╗ │
    Swipe     │     │  Swipe Right ╔═╬═══╝ │     Tap Gear
    Right     │     │              ║ │     │     Icon
              │     │  Tap Gear ═══║═╬═════╪═════════╗
              │     │  Tap WiFi ═══║═╬═══╗ │         ║
              │     └──────────────╬─┼───╫─┘         ║
              │                    ║ │   ║           ║
              │     ┌──────────────╬─┘   ║           ║
              │     │              ║     ║           ║
              │     ▼              ║     ║           ▼
              │  ┌─────────────┐  ║     ║     ┌──────────────┐
              └──┤  BTC News   │  ║     ║     │   Settings   │
                 │             │  ║     ║     │              │
                 │ Tap @ ══════╬══╝     ║     │  Tap Back ═══╬══╗
                 │ (Refresh)   │        ║     │              │  ║
                 └─────────────┘        ║     └──────────────┘  ║
                                        ║            ▲           ║
                                        ║            ║           ║
                                        ╚════════════╩═══════════╝
```

### Gesture Recognition
```
Touch Event Flow:

1. Touch Start (TEvent::DragStart)
   ↓
   Record: startX, startY, startTime

2. Touch Move (TEvent::Drag)
   ↓
   Update: currentX, currentY

3. Touch End (TEvent::DragEnd)
   ↓
   Calculate: deltaX, deltaY, duration
   ↓
   Check: isHorizontalSwipe?
   ├─ |deltaX| > |deltaY|
   ├─ |deltaY| < 50px
   └─ |deltaX| > 80px
         ↓
   Check: duration < 500ms?
         ↓
   Determine Direction:
   ├─ deltaX > 0 → Right Swipe
   └─ deltaX < 0 → Left Swipe
         ↓
   Execute Navigation:
   ├─ Dashboard + Left → News
   └─ News + Right → Dashboard
```

## API Integration

### mempool.space API
```
Base URL: https://mempool.space/api

Endpoints:
├─ /v1/prices
│  └→ { USD: 45123.45, EUR: 41234.56, ... }
│
├─ /v1/blocks
│  └→ [{ height: 820000, tx_count: 3421, ... }, ...]
│
├─ /mempool
│  └→ { count: 45678, vsize: 123456789, ... }
│
└─ /v1/fees/recommended
   └→ { fastestFee: 25, halfHourFee: 15, hourFee: 10 }

Update Intervals:
├─ Price: 30 seconds
├─ Blocks: 60 seconds
└─ Mempool + Fees: 30 seconds

Timeout: 10 seconds per request
```

### Google Gemini API
```
Base URL: https://generativelanguage.googleapis.com

Endpoint: /v1beta/models/gemini-1.5-flash:generateContent

Authentication: API Key in URL parameter
?key=AIzaSy...

Request:
POST /v1beta/models/gemini-1.5-flash:generateContent?key=xxx
{
  "contents": [{
    "parts": [{
      "text": "Analyze Bitcoin market with data: Price=$XX,XXX..."
    }]
  }],
  "generationConfig": {
    "temperature": 0.7,
    "maxOutputTokens": 1024
  }
}

Response:
{
  "candidates": [{
    "content": {
      "parts": [{
        "text": "📈 Market Summary\n\n..."
      }]
    }
  }]
}

Timeout: 30 seconds
Manual Refresh: User-initiated only
```

## Memory Management

### RAM Usage
```
Total RAM: 320 KB (ESP32-S3)

Allocation:
├─ System/OS: ~100 KB
├─ WiFi Stack: ~40 KB
├─ Display Buffer: ~40 KB (480*40*2 bytes)
├─ Application: ~48 KB
│  ├─ Screen Objects: ~10 KB
│  ├─ BTC Data: ~1 KB
│  ├─ Config Data: ~1 KB
│  ├─ Gemini Request: ~2 KB
│  ├─ Gemini Response: ~4 KB
│  └─ Other: ~30 KB
└─ Free: ~92 KB (28%)

Peak Usage: 48.4 KB (14.8%)
```

### Flash Usage
```
Total Flash: 16 MB (ESP32-S3)

Allocation:
├─ Firmware: ~1 MB
│  ├─ Core Application: ~300 KB
│  ├─ LovyanGFX: ~200 KB
│  ├─ WiFi/HTTP: ~150 KB
│  ├─ ArduinoJson: ~50 KB
│  ├─ Touch Driver: ~20 KB
│  └─ Other Libraries: ~280 KB
├─ NVRAM (Preferences): 32 KB
├─ OTA Partition: ~1 MB (for updates)
└─ Free: ~14 MB (87%)

Code Size: 1016 KB (15.5% of available 6.5MB partition)
```

## Security Model

### Attack Surface
```
┌─────────────────────────────────────────────────────────┐
│                   Attack Vectors                        │
├─────────────────────────────────────────────────────────┤
│  1. Source Code Access                                  │
│     ├─ Risk: LOW (no keys in code)                     │
│     └─ Mitigation: NVRAM storage, .gitignore           │
│                                                         │
│  2. Firmware Binary Access                              │
│     ├─ Risk: LOW (keys in device flash only)           │
│     └─ Mitigation: Keys not in firmware binary         │
│                                                         │
│  3. Physical Device Access                              │
│     ├─ Risk: MEDIUM (flash dump possible)              │
│     └─ Mitigation: Flash encryption, secure boot       │
│                                                         │
│  4. Network Interception                                │
│     ├─ Risk: LOW (HTTPS encryption)                    │
│     └─ Mitigation: TLS/SSL for all APIs                │
│                                                         │
│  5. Serial Monitor Exposure                             │
│     ├─ Risk: LOW (keys masked in output)               │
│     └─ Mitigation: Show AIza...4567 only               │
└─────────────────────────────────────────────────────────┘
```

### Security Layers
```
Layer 1: Application Security
├─ No hardcoded keys
├─ Masked output (serial)
├─ Input validation
└─ Secure defaults

Layer 2: Storage Security
├─ ESP32 Preferences (NVS)
├─ Wear leveling
├─ CRC checksums
└─ Optional flash encryption

Layer 3: Network Security
├─ HTTPS for all APIs
├─ TLS 1.2/1.3
├─ Certificate validation
└─ Timeout protection

Layer 4: Physical Security
├─ Optional flash encryption
├─ Optional secure boot
├─ eFuse protection
└─ Debug interface lockdown
```

## Performance Metrics

### Response Times
```
Operation                    Time        Notes
─────────────────────────────────────────────────────────
Boot to Dashboard           3-5 sec      With WiFi connect
WiFi Connection             2-10 sec     Network dependent
BTC Data Fetch              500-2000ms   mempool.space API
Screen Navigation           <100ms       Instant
Swipe Recognition           <50ms        Touch response
Gemini API Call             2-5 sec      AI generation
Configuration Save          <100ms       NVRAM write
Configuration Load          <50ms        NVRAM read
Display Update              16ms         60 FPS capable
Touch Polling               10ms         100 Hz
```

### Network Bandwidth
```
Operation              Size        Frequency    Daily Usage
──────────────────────────────────────────────────────────
Price API             ~500 bytes   Every 30s    1.4 MB
Blocks API            ~4 KB        Every 60s    5.7 MB
Mempool API           ~1 KB        Every 30s    2.9 MB
Fees API              ~200 bytes   Every 30s    576 KB
Gemini Request        ~1 KB        Manual       Varies
Gemini Response       ~2-4 KB      Manual       Varies

Total (24h continuous): ~10.6 MB
With Gemini (10 requests): ~11 MB
```

## Future Architecture

### Planned Enhancements
```
Phase 1: Enhanced UI
├─ On-screen keyboard for API key entry
├─ Interval configuration UI
├─ Multiple theme support
└─ Status indicators

Phase 2: Advanced Features
├─ Historical charts (price, fees)
├─ Multiple cryptocurrency support
├─ Price alerts and notifications
└─ Custom data refresh rates

Phase 3: Cloud Integration
├─ Configuration backup/sync
├─ OTA firmware updates
├─ Remote management
└─ Usage analytics (opt-in)

Phase 4: AI Enhancements
├─ Multiple AI providers (OpenAI, Claude)
├─ Custom analysis prompts
├─ Voice output (TTS)
└─ Predictive alerts
```

## Development Tools

### Build System
```
Makefile Targets:
├─ make build       → Compile firmware
├─ make upload      → Flash to device
├─ make monitor     → Serial monitor
├─ make flash       → Build + Upload + Monitor
├─ make clean       → Clean build artifacts
└─ make devices     → List connected devices

PlatformIO:
├─ pio run          → Build
├─ pio run -t upload → Upload
├─ pio device monitor → Monitor
└─ pio test         → Run tests
```

### Debug Output
```
Serial Monitor (115200 baud):
├─ Configuration loading
├─ WiFi connection status
├─ API request/response logs
├─ Touch event debugging
├─ Error messages
└─ Performance metrics
```

---

**Architecture Version:** 2.0
**Last Updated:** 2025-11-28
**Status:** Production Ready ✅
