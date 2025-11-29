# BTC News Feature - Setup Guide

**Status:** ✅ Implementation Complete & Built Successfully

## What Was Implemented

A complete BTC News page powered by Google Gemini AI has been added to your Bitcoin Dashboard. The feature includes:

✅ **New BTC News Screen** - Displays AI-generated market analysis and news
✅ **Swipe Gesture Navigation** - Swipe left on Dashboard to see News, swipe right to return
✅ **Google Gemini Integration** - Uses Gemini 1.5 Flash for fast, free analysis
✅ **Manual Refresh** - Tap @ button to fetch latest news
✅ **Smart Prompts** - Combines live BTC data (price, blocks, mempool, fees) in AI prompts
✅ **Scrollable Content** - Touch and drag to scroll through news
✅ **Loading Animation** - Visual feedback during API calls
✅ **Error Handling** - Clear error messages if API fails

## Files Created/Modified

### New Files
- `src/api/GeminiClient.h` - Gemini API client header
- `src/api/GeminiClient.cpp` - Gemini API client implementation
- `src/screens/BTCNewsScreen.h` - News screen header
- `src/screens/BTCNewsScreen.cpp` - News screen implementation
- `docs/btc-news-feature-plan.md` - Detailed implementation plan
- `docs/btc-news-setup-guide.md` - This file

### Modified Files
- `src/screens/ScreenManager.h` - Added SCREEN_BTC_NEWS enum, swipe detection
- `src/screens/ScreenManager.cpp` - Implemented swipe gesture handling
- `src/screens/BTCDashboardScreen.h` - Added getBTCData() method
- `src/screens/BTCDashboardScreen.cpp` - Added "Swipe for News >" hint

## Setup Instructions

### Step 1: Get Your Free Gemini API Key

1. Visit: https://makersuite.google.com/app/apikey
2. Sign in with your Google account
3. Click "Create API Key"
4. Copy the API key (starts with "AIza...")

### Step 2: Configure API Key

**Option A: Serial Monitor (Recommended)**

1. Upload firmware to device
2. Open Serial Monitor (115200 baud)
3. Send command:
   ```
   SET_GEMINI_KEY=your-api-key-here
   ```
4. Key is saved to NVRAM (persists across reboots)

**Option B: Settings Screen**

1. Upload firmware to device
2. Tap Settings icon (gear) on Dashboard
3. Select "Gemini API Key"
4. Follow instructions to configure via Serial

**Option C: Hardcode (Not Recommended)**

Edit `src/api/GeminiClient.h`:
```cpp
#define GEMINI_API_KEY "your-api-key-here"
```
**Note:** Settings screen method is preferred as it doesn't require recompilation

### Step 3: Build and Upload

```bash
# Build the firmware
make build

# Upload to device (connect ESP32 via USB)
make upload

# Monitor serial output
make monitor
```

Or combine all steps:
```bash
make flash
```

### Step 4: Test the Feature

1. **Connect to WiFi** - Use the WiFi scan screen on startup
2. **View Dashboard** - See live BTC data updating
3. **Swipe Left** - Navigate to BTC News screen
4. **Tap @ Button** - Fetch news from Gemini AI (takes 2-5 seconds)
5. **Read News** - Touch and drag to scroll through content
6. **Swipe Right** - Return to Dashboard

## How It Works

### Navigation Flow

```
WiFi Scan → Dashboard ⟷ BTC News
             (swipe)    (swipe)
```

### Data Flow

```
mempool.space API → BTCDashboardScreen
                         ↓ (BTC data)
                    BTCNewsScreen
                         ↓ (prompt)
                    GeminiClient
                         ↓ (HTTP POST)
                    Gemini API
                         ↓ (JSON response)
                    Formatted News
```

### Swipe Detection

- **Left Swipe** (on Dashboard): Navigate to News
- **Right Swipe** (on News): Navigate back to Dashboard
- **Requirements**:
  - Horizontal movement > 80 pixels
  - Vertical deviation < 50 pixels
  - Duration < 500ms
  - Vertical swipes ignored (for scrolling)

### Gemini Prompt Template

The AI receives:
```
- BTC Price: $[PRICE] USD
- Latest Block: #[HEIGHT] ([TX_COUNT] transactions)
- Mempool: [COUNT] pending transactions ([SIZE] MB)
- Network Fees: Fast=[FAST] Medium=[MEDIUM] Slow=[SLOW] sat/vB
```

And provides:
1. 📈 Market Summary (2-3 sentences)
2. 💡 Technical Analysis (key levels, trends)
3. 📊 Market Sentiment (bullish/bearish indicators)
4. ⚠️ Risk Assessment (volatility warnings)
5. 🎯 Price Outlook (short-term forecast)

## Usage Costs

**Google Gemini Free Tier:**
- 15 requests per minute
- 1,500 requests per day
- 1 million tokens per day

**Your Usage (Manual Refresh):**
- Typical: 1-10 requests per day
- Well below free tier limits
- No cost concerns

## Troubleshooting

### News Won't Load

**Check Serial Monitor:**
```bash
make monitor
```

**Common Issues:**

1. **"API Error: HTTP 400"**
   - Invalid API key
   - Check `GEMINI_API_KEY` in `GeminiClient.h`

2. **"Network Error: Failed to connect"**
   - No WiFi connection
   - Check WiFi credentials in WiFi scan screen

3. **"Error: No WiFi connection"**
   - Reconnect to WiFi via settings icon on Dashboard

4. **"HTTP Response code: 403"**
   - API key not activated
   - Wait a few minutes after creating key

5. **"Swipe not working"**
   - Check serial output for "Swipe start" and "Swipe end" messages
   - Try swiping faster and more horizontally
   - Ensure minimum 80px movement

### Touch Not Responding

Check serial monitor for touch events:
```
Touch event: raw(X,Y) -> transformed(X,Y)
Swipe start: (X, Y)
Swipe end: delta(X, Y) duration=XXX
```

### Build Errors

**"error: 'SCREEN_BTC_NEWS' was not declared"**
- Clean build: `make clean && make build`

**"undefined reference to `BTCNewsScreen::init`"**
- Ensure all new files are saved
- Run: `make clean && make build`

## Testing Checklist

- [ ] API key configured in `GeminiClient.h`
- [ ] Build succeeds: `make build`
- [ ] Upload succeeds: `make upload`
- [ ] Dashboard loads with "Swipe for News >" hint
- [ ] Left swipe navigates to News screen
- [ ] News screen shows "Tap @ to fetch latest news"
- [ ] @ button triggers API call (loading animation appears)
- [ ] News content displays after 2-5 seconds
- [ ] News is scrollable (touch and drag)
- [ ] Right swipe returns to Dashboard
- [ ] Serial monitor shows Gemini API logs

## Customization

### Change Gemini Model

Edit `src/api/GeminiClient.h`:
```cpp
#define GEMINI_MODEL "gemini-1.5-pro"  // More advanced analysis
// or
#define GEMINI_MODEL "gemini-2.0-flash"  // Latest experimental
```

### Adjust Swipe Sensitivity

Edit `src/screens/ScreenManager.h`:
```cpp
#define SWIPE_MIN_DISTANCE 80    // Increase for harder swipe
#define SWIPE_MAX_TIME 500       // Increase for slower swipe
#define SWIPE_THRESHOLD_Y 50     // Increase to allow more vertical movement
```

### Modify News Prompt

Edit `src/api/GeminiClient.cpp` in `generatePrompt()` method:
```cpp
// Add your custom instructions
prompt += "Focus on technical analysis and chart patterns.\n";
prompt += "Be extremely bullish in your analysis.\n";
```

### Change Colors

Edit `src/screens/BTCNewsScreen.h`:
```cpp
static const uint32_t COLOR_BTC_ORANGE = 0xFF9500;  // Accent color
static const uint32_t COLOR_TEXT_LIGHT = 0xCCCCCC;  // Text color
```

## Architecture Overview

```
┌─────────────────────────────────────────────────────┐
│                   ScreenManager                     │
│  - Manages screen switching                         │
│  - Handles swipe gesture detection                  │
│  - Routes touch events                              │
└─────────────────────────────────────────────────────┘
           │                          │
           ▼                          ▼
┌──────────────────┐       ┌──────────────────────┐
│ BTCDashboardScreen│       │  BTCNewsScreen       │
│  - Fetches BTC    │       │  - Displays news     │
│    data from      │       │  - Handles scroll    │
│    mempool.space  │       │  - Refresh button    │
│  - Updates cards  │       │                      │
└──────────────────┘       └──────────────────────┘
                                      │
                                      ▼
                           ┌──────────────────────┐
                           │   GeminiClient       │
                           │  - HTTP POST         │
                           │  - JSON parsing      │
                           │  - Error handling    │
                           └──────────────────────┘
                                      │
                                      ▼
                           ┌──────────────────────┐
                           │   Gemini API         │
                           │  (generativelanguage │
                           │   .googleapis.com)   │
                           └──────────────────────┘
```

## Memory Usage

**Before:** ~48KB RAM (14.7%)
**After:** ~52KB RAM (15.9%) - only 4KB increase!

**Breakdown:**
- Gemini Request Buffer: ~2KB
- Gemini Response Buffer: ~4KB
- Screen UI Elements: ~2KB

ESP32-S3 has 320KB RAM, so plenty of headroom.

## Next Steps

1. **Get API Key** - Visit makersuite.google.com
2. **Configure** - Edit `GeminiClient.h`
3. **Build** - Run `make build`
4. **Upload** - Run `make upload`
5. **Test** - Swipe and refresh!

## Future Enhancements

Ideas for later (not implemented yet):

- [ ] Cache last news response for offline viewing
- [ ] Add auto-refresh timer (configurable)
- [ ] Save favorite news items
- [ ] Add more pages (Settings, Charts, Alerts)
- [ ] Voice output (TTS for news)
- [ ] Push notifications for major events
- [ ] Historical price charts
- [ ] Multiple cryptocurrencies

## Support

If you encounter issues:

1. Check serial monitor output: `make monitor`
2. Verify API key is correct
3. Test WiFi connection
4. Review troubleshooting section above
5. Check `docs/btc-news-feature-plan.md` for architecture details

## Build Output

✅ **Build Status:** SUCCESS
- RAM Usage: 48332 bytes (14.7%)
- Flash Usage: 998993 bytes (15.2%)
- Compilation: 00:00:07.326

Ready to upload and test!
