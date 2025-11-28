# BTC News Feature Implementation Plan

**Feature:** Add a new page for Bitcoin news and market insights powered by Google Gemini AI

**Last Updated:** 2025-11-28

## Overview

This plan outlines the implementation of a BTC News screen that uses Google Gemini AI to fetch, analyze, and summarize current Bitcoin market news, trends, and insights. The page will be accessible via swipe gesture navigation from the main dashboard.

## User Requirements

- **AI Provider:** Google Gemini API (free tier available)
- **Content Type:** Current BTC news, market analysis, and insights
- **Update Method:** Manual refresh (tap button to fetch latest news)
- **Navigation:** Swipe gesture between Dashboard ↔ News pages
- **Analysis Types:**
  - Technical analysis
  - Market sentiment
  - Risk assessment
  - Price predictions

## Architecture Changes

### 1. Screen System Enhancement

**Current State:**
- `ScreenManager` handles 3 screen types: WiFi Scan, WiFi Connect, Dashboard
- Touch handling uses tap events only
- Screen switching via `switchScreen()` method

**Required Changes:**
- Add `SCREEN_BTC_NEWS` to Screen enum (ScreenManager.h:9-13)
- Implement swipe gesture detection in touch callback
- Add horizontal page navigation with visual feedback
- Track swipe direction and distance for smooth transitions

### 2. New Components

#### BTCNewsScreen Class
**Location:** `src/screens/BTCNewsScreen.h` and `BTCNewsScreen.cpp`

**Responsibilities:**
- Display Gemini-generated news and analysis
- Handle manual refresh button
- Show loading animation during API calls
- Display error messages if API fails
- Render scrollable news content

**UI Layout:**
```
┌─────────────────────────────────────────────────┐
│  [<] BTC News & Analysis          [Refresh 🔄] │ Header bar
├─────────────────────────────────────────────────┤
│                                                 │
│  📈 Market Summary                              │ Gemini content
│  [Latest news and analysis text...]            │ (scrollable)
│                                                 │
│  💡 Technical Analysis                          │
│  [Technical indicators and patterns...]         │
│                                                 │
│  📊 Market Sentiment: Bullish                  │
│  [Sentiment analysis...]                        │
│                                                 │
│  ⚠️  Risk Assessment                           │
│  [Risk levels and warnings...]                  │
│                                                 │
│  🎯 Price Outlook                              │
│  [Short-term predictions...]                    │
│                                                 │
│  ← Swipe to return to Dashboard                │ Footer hint
└─────────────────────────────────────────────────┘
```

#### GeminiClient Class
**Location:** `src/api/GeminiClient.h` and `GeminiClient.cpp`

**Responsibilities:**
- HTTP POST requests to Gemini API
- API key management (secure storage)
- Request formatting with proper headers
- JSON response parsing
- Error handling and timeout management
- Rate limiting awareness

**API Endpoint:**
```
POST https://generativelanguage.googleapis.com/v1beta/models/gemini-1.5-flash:generateContent?key=YOUR_API_KEY
```

**Request Format:**
```json
{
  "contents": [{
    "parts": [{
      "text": "Analyze current Bitcoin market..."
    }]
  }],
  "generationConfig": {
    "temperature": 0.7,
    "maxOutputTokens": 1024
  }
}
```

### 3. Swipe Gesture Detection

**Implementation in ScreenManager:**

```cpp
struct SwipeGesture {
    int16_t startX, startY;
    int16_t endX, endY;
    unsigned long startTime;
    bool isActive;
};

// Detection parameters
#define SWIPE_MIN_DISTANCE 80    // Minimum pixels for swipe
#define SWIPE_MAX_TIME 500       // Maximum time (ms) for swipe
#define SWIPE_THRESHOLD_Y 50     // Max vertical deviation
```

**Swipe Actions:**
- **Left Swipe (on Dashboard):** Navigate to News page
- **Right Swipe (on News):** Return to Dashboard
- Visual feedback during swipe (partial page reveal)

### 4. Data Integration

**Current Bitcoin Data Available (from BTCDashboardScreen):**
- Price: USD, EUR
- Block: Height, TX count, hash, timestamp
- Mempool: TX count, size (MB)
- Fees: Fast, Medium, Slow (sat/vB)

**Gemini Prompt Template:**
```
You are a Bitcoin market analyst. Based on the following real-time data,
provide a concise market analysis (max 800 words):

CURRENT DATA:
- BTC Price: $[PRICE] USD
- Latest Block: #[HEIGHT] ([TX_COUNT] transactions)
- Mempool: [COUNT] pending transactions ([SIZE] MB)
- Network Fees: Fast=[FAST] Medium=[MEDIUM] Slow=[SLOW] sat/vB

Provide:
1. Market Summary (2-3 sentences)
2. Technical Analysis (support/resistance, trends)
3. Market Sentiment (bullish/bearish indicators)
4. Risk Assessment (volatility warnings)
5. Price Outlook (short-term forecast)

Format with clear sections using emoji headers.
```

## Implementation Steps

### Phase 1: Foundation (Core Infrastructure)

**1.1 Update Screen Enum**
- Edit `src/screens/ScreenManager.h`
- Add `SCREEN_BTC_NEWS` to enum

**1.2 Create Placeholder BTCNewsScreen**
- Create `src/screens/BTCNewsScreen.h`
- Create `src/screens/BTCNewsScreen.cpp`
- Implement basic BaseScreen interface
- Add simple "BTC News - Coming Soon" UI
- Register in ScreenManager switch statement

**1.3 Add PlatformIO Dependencies**
- Verify HTTPClient is available (built-in ESP32)
- Verify ArduinoJson is sufficient (already in project)
- No additional libraries needed

### Phase 2: Swipe Navigation

**2.1 Enhance Touch Handling**
- Add swipe gesture detection to ScreenManager
- Track touch start/end positions and time
- Calculate swipe direction and velocity
- Filter out accidental swipes

**2.2 Implement Page Transitions**
- Add smooth screen transitions
- Visual feedback during swipe (optional: partial reveal)
- Haptic feedback (if supported by hardware)
- Update both screens to support swipe hints

**2.3 Test Navigation**
- Swipe from Dashboard → News
- Swipe from News → Dashboard
- Ignore vertical swipes (scrolling)
- Handle edge cases (quick taps, multi-touch)

### Phase 3: Gemini API Integration

**3.1 Create GeminiClient Library**
- Create `libs/GeminiClient/` directory structure
- Implement HTTP POST to Gemini API
- Add API key configuration (constants or EEPROM)
- Handle timeouts (30-second max for Gemini)
- Parse JSON responses

**3.2 API Key Storage**
- Option 1: Hardcoded constant (simple, for personal use)
- Option 2: EEPROM/Preferences (secure, user-configurable)
- Add placeholder `#define GEMINI_API_KEY "your-key-here"`

**3.3 Request/Response Handling**
- Format prompt with current BTC data
- Send HTTP POST with proper headers
- Parse Gemini response (nested JSON structure)
- Extract generated text from response
- Handle API errors (rate limits, network issues)

**3.4 Memory Optimization**
- Estimate response size (typically 2-4 KB)
- Use streaming JSON parser if needed
- Allocate DynamicJsonDocument appropriately
- Free memory after parsing
- Monitor PSRAM usage

### Phase 4: UI Implementation

**4.1 Design News Screen Layout**
- Header with back button and refresh icon
- Scrollable content area for news text
- Loading animation during API call
- Error message display area
- Footer with swipe hint

**4.2 Implement Text Rendering**
- Use LovyanGFX text rendering
- Handle word wrapping
- Implement scrolling (touch drag or auto-scroll)
- Format with sections and emoji headers
- Use appropriate fonts (readable, compact)

**4.3 Add Refresh Button**
- Position in top-right corner
- Tap to trigger Gemini API call
- Show loading spinner during fetch
- Disable button during active request
- Update timestamp on successful refresh

**4.4 Loading States**
- Initial state: "Tap refresh to load news"
- Loading state: Animated spinner + "Analyzing Bitcoin market..."
- Success state: Display formatted news
- Error state: "Failed to fetch news. Tap refresh to retry."

### Phase 5: Data Integration

**5.1 Connect BTCData to Gemini**
- Pass BTCData structure to GeminiClient
- Format data into prompt template
- Include relevant metrics in request
- Ensure data is fresh before sending

**5.2 Context-Aware Analysis**
- Calculate price changes (need historical data)
- Detect unusual mempool activity
- Identify high fee periods
- Flag network congestion

**5.3 Response Processing**
- Parse Gemini's markdown-like output
- Extract sections (summary, analysis, etc.)
- Format for display with emoji icons
- Handle truncated responses

### Phase 6: Testing & Optimization

**6.1 Functional Testing**
- Test swipe navigation (both directions)
- Test refresh button functionality
- Test with various API responses
- Test error handling (no internet, API errors)
- Test memory stability (repeated refreshes)

**6.2 Performance Testing**
- Measure API response time (typically 2-5 seconds)
- Check memory usage before/after requests
- Monitor ESP32 heap fragmentation
- Test with low memory conditions

**6.3 Edge Cases**
- No WiFi connection
- Gemini API rate limiting
- Very long API responses
- Malformed JSON responses
- Touch during loading

**6.4 User Experience Polish**
- Smooth animations
- Clear loading indicators
- Informative error messages
- Intuitive swipe hints
- Responsive touch feedback

## Technical Considerations

### Memory Management

**ESP32-S3 Resources:**
- 16MB Flash
- 8MB OPI PSRAM
- ~300KB internal RAM

**Memory Allocation Estimates:**
- Gemini Request: ~1KB
- Gemini Response: 2-4KB
- JSON Parsing Buffer: 6KB
- Display Buffer: existing allocation
- **Total Additional:** ~10KB (well within limits)

### API Rate Limits

**Google Gemini Free Tier:**
- 15 requests per minute
- 1,500 requests per day
- 1 million tokens per day

**Our Usage (Manual Refresh):**
- ~1-10 requests per day (typical user)
- Well below free tier limits
- No throttling needed

### Network Considerations

**API Call Timing:**
- Gemini typically responds in 2-5 seconds
- Set 30-second HTTP timeout
- Show loading indicator immediately
- Allow user to cancel (future enhancement)

**Error Handling:**
- Retry logic for transient errors
- Clear error messages
- Fallback to cached data (future enhancement)
- Offline mode notification

### Security

**API Key Protection:**
- Never commit API key to git
- Use #define or EEPROM storage
- Add `.env` or `secrets.h` to `.gitignore`
- Document key setup in README

**HTTPS:**
- ESP32 supports HTTPS
- Gemini API requires HTTPS
- Use `HTTPClient` with secure connection

## File Structure

```
btc-dashboard/
├── src/
│   ├── main.cpp
│   ├── DisplayConfig.h
│   ├── screens/
│   │   ├── ScreenManager.h          [MODIFY: Add swipe detection]
│   │   ├── ScreenManager.cpp        [MODIFY: Implement swipe]
│   │   ├── BTCDashboardScreen.h     [MODIFY: Add swipe hint]
│   │   ├── BTCDashboardScreen.cpp   [MODIFY: Add swipe hint]
│   │   ├── BTCNewsScreen.h          [NEW: News screen header]
│   │   └── BTCNewsScreen.cpp        [NEW: News screen implementation]
│   └── api/
│       ├── GeminiClient.h           [NEW: Gemini API client]
│       └── GeminiClient.cpp         [NEW: API implementation]
└── docs/
    └── btc-news-feature-plan.md     [THIS FILE]
```

## Configuration Constants

**Add to BTCNewsScreen.h:**
```cpp
// Gemini API Configuration
#define GEMINI_API_KEY "your-api-key-here"
#define GEMINI_MODEL "gemini-1.5-flash"
#define GEMINI_ENDPOINT "https://generativelanguage.googleapis.com/v1beta/models/"

// UI Configuration
#define NEWS_REFRESH_TIMEOUT 30000    // 30 seconds
#define NEWS_MAX_RESPONSE_SIZE 4096   // 4KB
#define NEWS_SCROLL_SPEED 2           // pixels per frame
```

## Testing Checklist

- [ ] Screen enum updated with SCREEN_BTC_NEWS
- [ ] BTCNewsScreen placeholder created
- [ ] Swipe left on Dashboard navigates to News
- [ ] Swipe right on News returns to Dashboard
- [ ] Vertical touch doesn't trigger swipe
- [ ] GeminiClient sends properly formatted requests
- [ ] API responses are parsed correctly
- [ ] Loading animation displays during fetch
- [ ] Refresh button triggers API call
- [ ] Error messages display on failure
- [ ] News content renders with proper formatting
- [ ] Memory usage is stable after multiple refreshes
- [ ] Text scrolling works smoothly
- [ ] Swipe hints are visible on both screens
- [ ] Touch response is immediate and smooth
- [ ] API key is not committed to git

## Future Enhancements (Post-MVP)

1. **News Caching:** Store last response in EEPROM for offline viewing
2. **Historical Data:** Track price changes over time for better analysis
3. **Custom Prompts:** Allow user to customize analysis focus
4. **Voice Output:** TTS for hands-free news updates
5. **Notifications:** Alert user when major market events detected
6. **Multiple Pages:** Add more screens (Settings, Alerts, Charts)
7. **Gesture Library:** Use more gestures (pinch, long-press, etc.)
8. **Auto-Refresh:** Optional periodic updates (configurable interval)

## Estimated Development Time

- Phase 1 (Foundation): 1-2 hours
- Phase 2 (Swipe Navigation): 2-3 hours
- Phase 3 (Gemini Integration): 3-4 hours
- Phase 4 (UI Implementation): 3-4 hours
- Phase 5 (Data Integration): 1-2 hours
- Phase 6 (Testing & Polish): 2-3 hours

**Total: 12-18 hours** (spread across multiple sessions)

## Dependencies

**No new external libraries required:**
- HTTPClient (ESP32 built-in)
- ArduinoJson (already in project)
- LovyanGFX (already in project)
- WiFi (ESP32 built-in)

## References

- [Google Gemini API Documentation](https://ai.google.dev/docs)
- [Gemini API Quickstart](https://ai.google.dev/tutorials/rest_quickstart)
- [LovyanGFX Documentation](https://github.com/lovyan03/LovyanGFX)
- [ESP32-S3 Technical Reference](https://www.espressif.com/sites/default/files/documentation/esp32-s3_technical_reference_manual_en.pdf)

## Notes

- Manual refresh keeps API costs at $0 (free tier)
- Swipe gesture provides intuitive navigation
- Gemini Flash is fast and efficient for this use case
- Design allows easy addition of more pages later
- All data stays local except API calls (privacy-friendly)
