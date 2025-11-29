# Bitcoin Trading Suggestion Feature - Implementation Plan

**Feature:** AI-Powered Trading Suggestions using OpenAI API
**Status:** Planning Phase
**Created:** 2025-11-28
**Target:** Phase 4 Implementation

---

## Executive Summary

Add a new screen to the Bitcoin Dashboard that provides AI-powered trading suggestions using OpenAI's GPT API. The feature will analyze real-time Bitcoin market data and provide actionable trading insights with reasoning.

**Key Features:**
- Real-time trading analysis using OpenAI GPT-4
- Integration with existing Bitcoin market data
- Touch-based navigation (swipe from Dashboard or News)
- Configurable refresh intervals
- Secure API key storage in NVRAM

---

## Architecture Analysis

### Current System

**Existing Screens:**
1. `SCREEN_WIFI_SCAN` - WiFi network selection
2. `SCREEN_WIFI_CONNECT` - WiFi connection (unused currently)
3. `SCREEN_DASHBOARD` - Main BTC data display
4. `SCREEN_BTC_NEWS` - Gemini AI-powered news (swipe left from Dashboard)
5. `SCREEN_SETTINGS` - Configuration screen (gear icon)

**Navigation Pattern:**
- Dashboard ↔ News (horizontal swipe)
- Dashboard → Settings (gear icon tap)
- Any screen → WiFi Scan (WiFi icon tap)

**AI Integration:**
- Existing: `GeminiClient` for news generation
- Secure key storage via `Config` system (NVRAM)
- Pattern: API key set via serial command

---

## Feature Design

### Screen: SCREEN_TRADING_SUGGESTION

#### Visual Layout

```
┌─────────────────────────────────────────────────┐
│ 📊 BTC Trading Analysis            [⚙] [📡]    │ Header (40px)
├─────────────────────────────────────────────────┤
│                                                  │
│  Market Context:                                 │
│  ┌──────────────────────────────────────────┐  │
│  │ BTC: $91,396 (+2.3%)                     │  │ Market Summary
│  │ 24h High: $93,250 | Low: $89,100        │  │ (80px)
│  │ Volume: 23.5B | Mempool: 47k TXs        │  │
│  └──────────────────────────────────────────┘  │
│                                                  │
│  Trading Signal: 📈 BULLISH                     │ Signal Badge
│  Confidence: ████████░░ 85%                     │ (40px)
│                                                  │
│  Recommendation:                                 │
│  ┌──────────────────────────────────────────┐  │
│  │ Based on current market conditions,      │  │
│  │ consider ACCUMULATING in the $90k-$92k  │  │
│  │ range. Strong support at $89k. Set      │  │ AI Analysis
│  │ stop-loss at $87.5k. Target: $98k      │  │ (scrollable)
│  │                                          │  │ (120px)
│  │ Key Factors:                             │  │
│  │ • Rising mempool activity (+15%)         │  │
│  │ • Hash rate at all-time high            │  │
│  │ • Low exchange reserves                  │  │
│  └──────────────────────────────────────────┘  │
│                                                  │
│  [🔄 Refresh]  Last updated: 2m ago            │ Footer (40px)
└─────────────────────────────────────────────────┘
```

#### Touch Actions

| Element | Action | Target |
|---------|--------|--------|
| Swipe Left | Navigate | → Dashboard |
| Swipe Right | Navigate | → News Screen |
| Tap Gear Icon | Open | Settings Screen |
| Tap WiFi Icon | Open | WiFi Scan Screen |
| Tap Refresh Button | Refresh | Trading analysis |
| Scroll Gesture | Scroll | Analysis text |

#### Trading Signal Types

| Signal | Icon | Color | Description |
|--------|------|-------|-------------|
| STRONG BUY | 📈📈 | #00FF00 | Very bullish conditions |
| BUY | 📈 | #88FF88 | Bullish conditions |
| HOLD | ⏸️ | #FFFF00 | Neutral, wait for clarity |
| SELL | 📉 | #FF8888 | Bearish conditions |
| STRONG SELL | 📉📉 | #FF0000 | Very bearish conditions |
| UNCERTAIN | ❓ | #CCCCCC | Insufficient data |

---

## Technical Implementation

### 1. OpenAI API Integration

#### Create OpenAIClient Class

**File:** `src/api/OpenAIClient.h` & `src/api/OpenAIClient.cpp`

**Key Features:**
- Similar pattern to `GeminiClient`
- GPT-4 Turbo or GPT-3.5 Turbo support
- Secure API key from Config system
- HTTP timeout: 30 seconds
- Response parsing for JSON structured output

**API Endpoint:**
```
POST https://api.openai.com/v1/chat/completions
```

**Request Format:**
```json
{
  "model": "gpt-4-turbo-preview",
  "messages": [
    {
      "role": "system",
      "content": "You are a professional Bitcoin trading analyst. Provide concise, actionable trading suggestions based on market data."
    },
    {
      "role": "user",
      "content": "Analyze this Bitcoin market data: Price: $91,396, 24h Change: +2.3%, Mempool: 47,853 TXs, Fees: Fast 25 sat/vB..."
    }
  ],
  "max_tokens": 500,
  "temperature": 0.7
}
```

**Response Format:**
```json
{
  "choices": [{
    "message": {
      "content": "Signal: BUY\nConfidence: 85%\n\nRecommendation: Consider accumulating..."
    }
  }]
}
```

#### OpenAIClient Class Structure

```cpp
class OpenAIClient {
private:
    HTTPClient http;
    String apiKey;
    String model;

    String buildEndpointURL();
    String buildRequestBody(const BTCData& data, const String& additionalContext);
    bool parseResponse(const String& response, TradingSuggestion& suggestion);

public:
    OpenAIClient();
    OpenAIClient(const String& key, const String& modelName);
    ~OpenAIClient();

    // Main analysis method
    bool fetchTradingSuggestion(const BTCData& data, TradingSuggestion& suggestion);

    // Generate trading analysis prompt
    String generateTradingPrompt(const BTCData& data);

    // Test API connectivity
    bool testConnection();

    // Set custom API key
    void setApiKey(const String& key);
};
```

### 2. Trading Suggestion Data Structure

**File:** `src/screens/BTCTradingSuggestionScreen.h`

```cpp
enum TradingSignal {
    SIGNAL_STRONG_BUY,
    SIGNAL_BUY,
    SIGNAL_HOLD,
    SIGNAL_SELL,
    SIGNAL_STRONG_SELL,
    SIGNAL_UNCERTAIN
};

struct TradingSuggestion {
    TradingSignal signal = SIGNAL_UNCERTAIN;
    int confidence = 0;           // 0-100%
    String recommendation = "";    // Main analysis text
    String keyFactors[5];         // Up to 5 bullet points
    int keyFactorCount = 0;
    float targetPrice = 0;
    float stopLoss = 0;
    unsigned long timestamp = 0;
    bool isValid = false;
};
```

### 3. BTCTradingSuggestionScreen Class

**File:** `src/screens/BTCTradingSuggestionScreen.h` & `.cpp`

```cpp
class BTCTradingSuggestionScreen : public BaseScreen {
private:
    ScreenManager* manager;
    BTCData* btcData;  // Pointer to shared data from Dashboard
    TradingSuggestion suggestion;
    OpenAIClient* openaiClient;

    unsigned long lastUpdate;
    bool isLoading;
    int scrollOffset;

    // Update interval (5 minutes to reduce API calls)
    static const unsigned long UPDATE_INTERVAL = 300000;

    // UI methods
    void drawHeader();
    void drawMarketSummary();
    void drawSignalBadge();
    void drawRecommendation();
    void drawFooter();
    void drawLoadingIndicator();
    void drawErrorMessage(const char* error);

    // Data methods
    bool fetchNewSuggestion();
    void updateDisplay();

public:
    void init(ScreenManager* mgr) override;
    void update() override;
    void handleTouch(int16_t x, int16_t y) override;
    void setBTCData(BTCData* data);  // Share data from Dashboard
};
```

### 4. Screen Manager Integration

**Modifications:** `src/screens/ScreenManager.h` & `.cpp`

```cpp
// Add to enum Screen
enum Screen {
    SCREEN_WIFI_SCAN,
    SCREEN_WIFI_CONNECT,
    SCREEN_DASHBOARD,
    SCREEN_BTC_NEWS,
    SCREEN_TRADING_SUGGESTION,  // NEW
    SCREEN_SETTINGS
};

// Updated swipe navigation in handleSwipe() - Circular carousel pattern
void ScreenManager::handleSwipe(int16_t deltaX, int16_t deltaY, unsigned long duration) {
    // Validate swipe gesture
    if (!isHorizontalSwipe(deltaX, deltaY)) return;
    if (duration > SWIPE_MAX_TIME) return;
    if (abs(deltaX) < SWIPE_MIN_DISTANCE) return;

    // Get current BTC data to share across screens
    BTCData currentData;
    bool hasData = false;

    if (currentScreenType == SCREEN_DASHBOARD) {
        BTCDashboardScreen* dashboard = static_cast<BTCDashboardScreen*>(currentScreen);
        currentData = dashboard->getBTCData();
        hasData = true;
    }

    // Determine swipe direction and navigate (circular carousel)
    if (deltaX > 0) {
        // Right swipe (clockwise: Dashboard → Trading → News → Dashboard)
        switch (currentScreenType) {
            case SCREEN_DASHBOARD:
                // Dashboard → Trading
                switchScreen(SCREEN_TRADING_SUGGESTION);
                if (hasData) {
                    BTCTradingSuggestionScreen* trading =
                        static_cast<BTCTradingSuggestionScreen*>(currentScreen);
                    trading->setBTCData(&currentData);
                }
                break;

            case SCREEN_BTC_NEWS:
                // News → Dashboard
                switchScreen(SCREEN_DASHBOARD);
                break;

            case SCREEN_TRADING_SUGGESTION:
                // Trading → News
                switchScreen(SCREEN_BTC_NEWS);
                if (hasData) {
                    BTCNewsScreen* news = static_cast<BTCNewsScreen*>(currentScreen);
                    news->setBTCData(currentData);
                }
                break;
        }
    } else {
        // Left swipe (counter-clockwise: Dashboard → News → Trading → Dashboard)
        switch (currentScreenType) {
            case SCREEN_DASHBOARD:
                // Dashboard → News
                switchScreen(SCREEN_BTC_NEWS);
                if (hasData) {
                    BTCNewsScreen* news = static_cast<BTCNewsScreen*>(currentScreen);
                    news->setBTCData(currentData);
                }
                break;

            case SCREEN_BTC_NEWS:
                // News → Trading
                switchScreen(SCREEN_TRADING_SUGGESTION);
                if (hasData) {
                    BTCTradingSuggestionScreen* trading =
                        static_cast<BTCTradingSuggestionScreen*>(currentScreen);
                    trading->setBTCData(&currentData);
                }
                break;

            case SCREEN_TRADING_SUGGESTION:
                // Trading → Dashboard (completes circle)
                switchScreen(SCREEN_DASHBOARD);
                break;
        }
    }
}
```

**Improved Navigation Flow:**

Current implementation only supports Dashboard ↔ News. The trading screen will be added as a circular swipe carousel:

```
┌──────────────────────────────────────────┐
│    Dashboard → News → Trading → Dashboard   │  (Circular swipe left)
│    Dashboard ← Trading ← News ← Dashboard   │  (Circular swipe right)
└──────────────────────────────────────────┘

Vertical Navigation (from any screen):
├─ Tap Gear Icon → Settings
├─ Tap WiFi Icon → WiFi Scan
└─ Swipe Up/Down → (Reserved for future features)
```

**Swipe Behavior:**
- **From Dashboard:**
  - Swipe Left → News Screen
  - Swipe Right → Trading Screen
- **From News:**
  - Swipe Left → Trading Screen
  - Swipe Right → Dashboard
- **From Trading:**
  - Swipe Left → Dashboard (completes circle)
  - Swipe Right → News Screen

### 5. Configuration System Updates

**File:** `src/Config.h` & `src/Config.cpp`

Add OpenAI API key storage:

```cpp
class Config {
private:
    Preferences preferences;
    String geminiApiKey;
    String openaiApiKey;  // NEW
    // ... existing fields

public:
    // OpenAI API key methods
    void setOpenAIApiKey(const String& key);
    String getOpenAIApiKey();
    bool hasOpenAIApiKey();
    String getMaskedOpenAIApiKey();
};
```

**Serial Command:**
```bash
SET_OPENAI_KEY=sk-proj-xxx...
```

### 6. Main Application Updates

**File:** `src/main.cpp`

```cpp
// Add new serial command
void processSerialCommand() {
    // ... existing commands
    } else if (command.startsWith("SET_OPENAI_KEY=")) {
        String key = command.substring(15);
        key.trim();
        if (key.length() > 0) {
            globalConfig.setOpenAIApiKey(key);
            if (globalConfig.save()) {
                Serial.println("✓ OpenAI API key saved successfully!");
            } else {
                Serial.println("✗ Failed to save API key");
            }
        }
    }
}
```

---

## API Cost Analysis

### OpenAI Pricing (as of 2024)

**GPT-4 Turbo:**
- Input: $0.01 per 1K tokens
- Output: $0.03 per 1K tokens

**GPT-3.5 Turbo:**
- Input: $0.0005 per 1K tokens
- Output: $0.0015 per 1K tokens

### Estimated Costs

**Per Request:**
- Prompt: ~300 tokens (market data + context)
- Response: ~500 tokens (recommendation)

**GPT-4 Turbo:** $0.003 + $0.015 = **$0.018 per request**
**GPT-3.5 Turbo:** $0.00015 + $0.00075 = **$0.0009 per request**

**Daily Usage (5-minute refresh, 16 hours active):**
- Requests per day: ~192 requests
- GPT-4 Turbo: $3.46/day or **$103.80/month**
- GPT-3.5 Turbo: $0.17/day or **$5.18/month**

**Recommendation:** Use **GPT-3.5 Turbo** for cost efficiency, or implement:
- Manual refresh only (no auto-refresh)
- 15-minute auto-refresh interval
- Cached suggestions (5-minute validity)

---

## Memory & Performance

### Memory Requirements

**OpenAI Response Buffer:** ~4KB (same as Gemini)
**TradingSuggestion struct:** ~2KB
**OpenAIClient object:** ~1KB
**Total additional RAM:** ~7KB

**Current Status:**
- RAM usage: 48,428 bytes (14.8%)
- Available: ~278KB
- New feature: +7KB → **55KB total (16.8%)**

**Flash Storage:**
- Current: 1.02 MB (15.5%)
- Estimated addition: +25KB code
- Total: ~1.05 MB (15.9%)

**Verdict:** ✅ Sufficient memory available

### Network Performance

**API Call Time:**
- OpenAI latency: 2-5 seconds (typical)
- Timeout: 30 seconds
- No blocking of UI (loading indicator shown)

---

## Touch Color Feedback System

### Visual Touch Response

All interactive elements provide immediate visual feedback to confirm user interaction.

**Feedback Implementation:**

```cpp
// Refresh button touch feedback
void BTCTradingSuggestionScreen::handleTouch(int16_t x, int16_t y) {
    if (touchInRefreshButton(x, y)) {
        // Immediate visual feedback - fill with orange
        lcd->fillRoundRect(REFRESH_BTN_X, REFRESH_BTN_Y,
                          REFRESH_BTN_W, REFRESH_BTN_H,
                          5, COLOR_BTC_ORANGE);

        // Show "Refreshing..." text
        lcd->setTextColor(COLOR_BG);
        lcd->setCursor(REFRESH_BTN_X + 10, REFRESH_BTN_Y + 20);
        lcd->print("🔄 Refreshing...");

        // Start non-blocking refresh
        isLoading = true;
        fetchNewSuggestion();

        // Restore button after completion (handled in update())
    }
}
```

**Touch Feedback Elements:**

| Element | Normal State | Pressed State | Duration |
|---------|-------------|---------------|----------|
| Refresh Button | Dark gray border | Orange fill | 200ms |
| Signal Badge | Signal color | Bright highlight | 150ms |
| Scroll Area | Hidden | Orange thumb | While scrolling |
| Recommendation Text | Normal | Light blue bg | Selection |

**Non-Blocking Feedback:**
```cpp
// Use timers instead of delay()
unsigned long feedbackStartTime = 0;
bool showingFeedback = false;

void update() {
    // Restore button after feedback duration
    if (showingFeedback && (millis() - feedbackStartTime > 200)) {
        restoreButtonNormalState();
        showingFeedback = false;
    }

    // Continue screen updates...
}
```

**Benefits:**
- ✅ Immediate response < 50ms
- ✅ Non-blocking (UI stays responsive)
- ✅ Consistent feedback across all elements
- ✅ Clear visual distinction between states
- ✅ Maintains smooth performance

See [touch-feedback-specification.md](./touch-feedback-specification.md) for complete implementation details.

---

## User Experience

### First-Time Setup

1. **Upload Firmware**
   ```bash
   make flash
   ```

2. **Configure OpenAI API Key**
   ```bash
   make monitor
   SET_OPENAI_KEY=sk-proj-xxx...
   ```

3. **Navigate to Trading Screen**
   - From Dashboard: Swipe right
   - From News: Swipe left

### Daily Usage

**Typical Flow:**
1. View Dashboard for current market data
2. Swipe right to see AI trading analysis
3. Read recommendation and confidence level
4. Tap refresh for updated analysis (if needed)
5. Return to Dashboard or check News

**Refresh Strategy:**
- Auto-refresh: Every 5 minutes (configurable)
- Manual refresh: Tap refresh button anytime
- Cache: Previous suggestion shown until new one loads

---

## Implementation Phases

### Phase 1: OpenAI Client (Week 1)
- ✅ Create `OpenAIClient` class
- ✅ Implement API communication
- ✅ Add response parsing
- ✅ Unit tests for API client
- ✅ Test with real API key

### Phase 2: Data Structures (Week 1)
- ✅ Define `TradingSuggestion` struct
- ✅ Define `TradingSignal` enum
- ✅ Create prompt generator
- ✅ Test prompt quality

### Phase 3: UI Implementation (Week 2)
- ✅ Create `BTCTradingSuggestionScreen` class
- ✅ Implement layout and rendering
- ✅ Add loading and error states
- ✅ Implement scrolling for long text
- ✅ Test touch interactions

### Phase 4: Integration (Week 2)
- ✅ Update `ScreenManager` with new screen
- ✅ Implement swipe navigation
- ✅ Share BTCData between screens
- ✅ Add Configuration storage
- ✅ Add serial commands

### Phase 5: Testing & Polish (Week 3)
- ✅ End-to-end testing
- ✅ Performance optimization
- ✅ Error handling improvements
- ✅ Documentation
- ✅ Unit tests

### Phase 6: Deployment (Week 3)
- ✅ Final testing on hardware
- ✅ Update documentation
- ✅ Create setup guide
- ✅ Git commit and push

**Total Timeline:** 3 weeks

---

## Security Considerations

### API Key Storage
- ✅ Stored in ESP32 NVRAM (encrypted)
- ✅ Never visible in source code
- ✅ Masked in serial output
- ✅ Same security as Gemini key

### API Key Protection
```cpp
// Output format
Serial.println("OpenAI Key: sk-proj-...xyz");  // First 8 + last 3 chars
```

### Best Practices
1. Never commit API keys to git
2. Use environment-specific keys (dev/prod)
3. Monitor API usage in OpenAI dashboard
4. Set usage limits to prevent overuse
5. Rotate keys periodically

---

## Risk Analysis

### Technical Risks

| Risk | Probability | Impact | Mitigation |
|------|-------------|--------|------------|
| API timeout | Medium | Low | Show cached result + retry |
| Insufficient memory | Low | High | Tested - 7KB available |
| API rate limiting | Low | Medium | 5-min refresh + manual only |
| Poor suggestion quality | Medium | Medium | Fine-tune prompts + GPT-4 |
| Network connectivity | Medium | Low | Show connection error |

### Financial Risks

| Risk | Probability | Impact | Mitigation |
|------|-------------|--------|------------|
| High API costs | Medium | Medium | Use GPT-3.5 + longer intervals |
| Unexpected usage spike | Low | High | Set OpenAI budget alerts |
| API pricing changes | Low | Medium | Monitor OpenAI announcements |

### User Experience Risks

| Risk | Probability | Impact | Mitigation |
|------|-------------|--------|------------|
| Slow response time | Medium | Medium | Loading indicator + cache |
| Confusing suggestions | Low | Medium | Clear disclaimers |
| Over-reliance on AI | Medium | High | Add risk warnings |

---

## Legal & Disclaimer

### Required Disclaimer

Display on Trading screen:

```
⚠️ DISCLAIMER
AI-generated suggestions are for informational purposes only.
Not financial advice. Trade at your own risk. Past performance
does not guarantee future results. Cryptocurrency trading
involves substantial risk of loss.
```

### Implementation
```cpp
void BTCTradingSuggestionScreen::drawDisclaimer() {
    lcd->setTextSize(1);
    lcd->setTextColor(COLOR_TEXT_DIM);
    lcd->setCursor(10, 300);
    lcd->print("⚠️ Not financial advice. Trade at own risk.");
}
```

---

## Testing Plan

### Unit Tests

**OpenAIClient Tests:**
- ✅ API request formatting
- ✅ Response parsing
- ✅ Error handling
- ✅ Timeout behavior

**TradingSuggestion Tests:**
- ✅ Signal type parsing
- ✅ Confidence validation
- ✅ Data structure integrity

### Integration Tests

**Screen Tests:**
- ✅ Touch interaction
- ✅ Swipe navigation
- ✅ Refresh button
- ✅ Scrolling behavior

**End-to-End Tests:**
- ✅ Dashboard → Trading navigation
- ✅ API key configuration
- ✅ Full refresh cycle
- ✅ Error recovery

### Hardware Tests

**On-Device:**
- ✅ Memory usage monitoring
- ✅ Performance with real API
- ✅ Network latency handling
- ✅ Display rendering quality

---

## Documentation Deliverables

### User Documentation
- **Setup Guide:** OpenAI API key configuration
- **Usage Guide:** How to interpret trading suggestions
- **Troubleshooting:** Common issues and solutions

### Developer Documentation
- **API Integration:** OpenAI client usage
- **Screen Architecture:** Trading screen implementation
- **Testing Guide:** How to test trading features

### Files to Create/Update
- `docs/features/btc-trading-suggestion-plan.md` (this document)
- `docs/guides/openai-setup-guide.md` (new)
- `docs/examples/trading-screen-example.md` (new)
- `CONFIGURATION_README.md` (update with OpenAI key)
- `QUICK_START.md` (update with trading feature)
- `README.md` (update with new feature)

---

## Alternative Approaches Considered

### 1. Use Existing Gemini API Instead of OpenAI

**Pros:**
- No additional API key needed
- Already integrated
- Similar capabilities

**Cons:**
- Separate analysis from news keeps concerns separate
- OpenAI has stronger reputation for structured analysis
- Different models for different tasks

**Decision:** Use OpenAI for dedicated trading analysis

### 2. On-Device ML Model

**Pros:**
- No API costs
- No internet dependency
- Instant results

**Cons:**
- ESP32-S3 insufficient for meaningful ML
- Model training requires expertise
- Limited accuracy compared to GPT

**Decision:** Cloud-based OpenAI API

### 3. Third-Party Trading API

**Pros:**
- Specialized for trading
- May include technical indicators

**Cons:**
- Limited customization
- Potential subscription costs
- Less flexible than LLM

**Decision:** OpenAI LLM for flexibility

---

## Success Metrics

### Functional Metrics
- ✅ Screen loads within 3 seconds
- ✅ API response within 5 seconds (90th percentile)
- ✅ Memory usage < 20% of total
- ✅ No crashes during 24-hour test

### User Experience Metrics
- ✅ Navigation feels smooth (no lag)
- ✅ Suggestions are readable and clear
- ✅ Confidence indicators are accurate
- ✅ Error messages are helpful

### Business Metrics
- ✅ API costs < $10/month per device
- ✅ 90%+ successful API calls
- ✅ User engagement with feature

---

## Future Enhancements

### Phase 5+ (Future)
1. **Historical Performance Tracking**
   - Track suggestion accuracy over time
   - Show win/loss ratio
   - Performance metrics

2. **Advanced Technical Analysis**
   - Include RSI, MACD indicators
   - Volume analysis
   - Multi-timeframe analysis

3. **Portfolio Management**
   - Track user holdings
   - P&L calculations
   - Portfolio suggestions

4. **Alert System**
   - Price alerts
   - Signal change notifications
   - Email/push notifications

5. **Multiple Cryptocurrencies**
   - ETH, SOL, BNB support
   - Cross-asset correlation
   - Portfolio diversification

---

## References

### API Documentation
- **OpenAI API:** https://platform.openai.com/docs/api-reference
- **GPT Models:** https://platform.openai.com/docs/models
- **Best Practices:** https://platform.openai.com/docs/guides/prompt-engineering

### Related Features
- **Gemini Integration:** `src/api/GeminiClient.h`
- **Config System:** `src/Config.h`
- **Screen Manager:** `src/screens/ScreenManager.h`

### External Resources
- **mempool.space API:** https://mempool.space/docs/api
- **Bitcoin Technical Analysis:** https://www.investopedia.com/bitcoin-trading-strategies

---

## Appendix A: Sample Prompts

### Trading Analysis Prompt Template

```
You are a professional Bitcoin trading analyst. Analyze the following Bitcoin market data and provide a concise trading suggestion.

Market Data:
- Current Price: $91,396 USD
- 24h Change: +2.3%
- Block Height: 867,095
- Mempool: 47,853 pending transactions
- Mempool Size: 85.4 MB
- Fee Rates: Fast 25 sat/vB, Medium 18 sat/vB, Slow 12 sat/vB

Provide your analysis in this format:

Signal: [STRONG_BUY|BUY|HOLD|SELL|STRONG_SELL]
Confidence: [0-100]%

Recommendation:
[2-3 sentences with actionable advice, including entry points, stop-loss, and targets if applicable]

Key Factors:
- [Factor 1]
- [Factor 2]
- [Factor 3]

Keep your response under 400 words. Focus on actionable insights.
```

### Response Format Example

```
Signal: BUY
Confidence: 82%

Recommendation:
Based on current market conditions, consider accumulating Bitcoin in the $90,000-$92,000 range. Strong support established at $89,000. Consider setting stop-loss at $87,500. Primary target: $98,000 in the short term.

Key Factors:
- Mempool activity increasing (+15% from yesterday), indicating rising network demand
- Hash rate at all-time high, showing strong network security
- Exchange reserves declining, reducing sell pressure
- Fee rates normalizing after recent spike, good entry timing
- Technical: Price above 50-day MA with bullish momentum
```

---

## Appendix B: Color Scheme

```cpp
// Trading Screen Colors
static const uint32_t COLOR_BG = 0x000000;              // Pure black
static const uint32_t COLOR_CARD_BG = 0x0C0C0C;         // Dark gray
static const uint32_t COLOR_TEXT_WHITE = 0xFFFFFF;      // White
static const uint32_t COLOR_TEXT_LIGHT = 0xCCCCCC;      // Light gray
static const uint32_t COLOR_TEXT_DIM = 0x999999;        // Dim gray

// Signal colors
static const uint32_t COLOR_STRONG_BUY = 0x00FF00;      // Pure green
static const uint32_t COLOR_BUY = 0x88FF88;             // Light green
static const uint32_t COLOR_HOLD = 0xFFFF00;            // Yellow
static const uint32_t COLOR_SELL = 0xFF8888;            // Light red
static const uint32_t COLOR_STRONG_SELL = 0xFF0000;     // Pure red
static const uint32_t COLOR_UNCERTAIN = 0xCCCCCC;       // Gray

// UI accents
static const uint32_t COLOR_BORDER = 0xFF9500;          // Bitcoin orange
static const uint32_t COLOR_LOADING = 0x00FFFF;         // Cyan
```

---

**Document Version:** 1.0
**Status:** Planning Complete - Ready for Implementation
**Next Step:** Begin Phase 1 - OpenAI Client Implementation
