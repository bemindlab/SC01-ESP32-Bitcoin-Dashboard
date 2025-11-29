# Telegram Bot Integration Plan

## Overview

Telegram bot integration for the Bitcoin Dashboard will enable push notifications, price alerts, remote monitoring, and two-way communication with the device. Users can receive real-time Bitcoin updates and control the dashboard remotely via Telegram chat.

**Target Version:** v2.1.0
**Priority:** High
**Estimated Implementation:** Q4 2025

## Features

### Phase 1: Basic Notifications (v2.1.0)

#### 1.1 Price Alerts
- **Threshold Alerts** - Notify when BTC price crosses user-defined thresholds
  - Example: "BTC just crossed $100,000! Current price: $100,234"
  - Support for multiple price levels (up to 5 alerts)
  - One-time or repeating alerts
  - Configurable via Settings screen or serial commands

- **Percentage Change Alerts** - Notify on significant price movements
  - Example: "BTC up 5.2% in the last hour! Now at $98,450"
  - Configurable thresholds (1%, 5%, 10%, 20%)
  - Time window options: 5min, 1h, 24h

- **Price Drop/Rise Alerts** - Directional movement notifications
  - "BTC dropped below $95,000"
  - "BTC reached new all-time high: $105,234"

#### 1.2 System Notifications
- **Device Status** - Boot, WiFi connection, errors
  - "Bitcoin Dashboard started successfully"
  - "WiFi connected to MyNetwork (RSSI: -45 dBm)"
  - "Error fetching BTC price - check internet connection"

- **Daily Summary** - End-of-day Bitcoin statistics
  - "Daily Summary: BTC opened at $96,500, closed at $98,200 (+1.76%)"
  - Include high/low, volume, mempool stats

### Phase 2: Remote Monitoring (v2.2.0)

#### 2.1 Query Commands
Users can request current data via Telegram chat:

- `/price` - Current BTC price in USD/EUR
- `/blocks` - Latest block height and info
- `/mempool` - Current mempool statistics
- `/fees` - Current fee recommendations
- `/status` - Device status (WiFi, uptime, memory)
- `/screenshot` - Request screenshot from device (send as image)
- `/help` - List all available commands

#### 2.2 Scheduled Reports
- **Hourly Updates** - "BTC: $98,234 | Block: 870,123 | Fees: 12 sat/vB"
- **Daily Reports** - Comprehensive summary with charts (if implemented)
- **Weekly Reports** - 7-day trends and analysis

### Phase 3: Remote Control (v2.3.0)

#### 3.1 Configuration Commands
- `/set_interval <type> <seconds>` - Change update intervals
  - Example: `/set_interval price 60`
- `/set_alert <type> <threshold>` - Configure price alerts
  - Example: `/set_alert price_above 100000`
- `/enable <feature>` / `/disable <feature>` - Toggle features
  - Example: `/disable trading_suggestions`

#### 3.2 Device Control
- `/restart` - Restart the device
- `/refresh` - Force data refresh
- `/switch_screen <name>` - Change active screen
  - Example: `/switch_screen news`
- `/brightness <level>` - Adjust display brightness (0-100)

### Phase 4: Advanced Features (v2.4.0)

#### 4.1 Trading Signals
- AI trading suggestion notifications
  - "Trading Signal: STRONG BUY (92% confidence)"
  - Include key factors and recommendations
  - Delivered when signal changes or on schedule

#### 4.2 Custom Alerts
- Block height milestones (e.g., "Block 900,000 mined!")
- Mempool congestion alerts ("Mempool over 200MB - high fees expected")
- Fee rate changes ("Fast fee dropped to 5 sat/vB - good time to transact")
- Network events (difficulty adjustment, halving countdown)

#### 4.3 Interactive Features
- Inline keyboard buttons for quick actions
  - "Refresh Price | View Chart | Get Analysis"
- Conversation-based setup wizard
  - Bot guides user through initial configuration

## Technical Architecture

### Library Selection

**UniversalTelegramBot** (Recommended)
- Repository: https://github.com/witnessmenow/Universal-Arduino-Telegram-Bot
- ESP32 compatible
- Simple API for sending/receiving messages
- Active development and community support
- Size: ~30KB

**Alternative: AsyncTelegram2**
- Repository: https://github.com/cotestatnt/AsyncTelegram2
- Async support (better for ESP32)
- More features but slightly larger

### Implementation Components

#### 1. TelegramClient Class
```cpp
class TelegramClient {
private:
    UniversalTelegramBot* bot;
    String botToken;
    String chatId;
    unsigned long lastCheckTime;
    int checkInterval; // milliseconds

public:
    void init(const String& token, const String& chat);
    void sendMessage(const String& message);
    void sendPhoto(const uint8_t* data, size_t length, const String& caption);
    void processMessages();
    void handleCommand(const String& command, const String& args);
};
```

#### 2. Alert Manager
```cpp
class AlertManager {
private:
    struct PriceAlert {
        float threshold;
        bool above; // true = alert when above, false = below
        bool oneTime;
        bool triggered;
    };

    PriceAlert alerts[5];
    TelegramClient* telegram;

public:
    void addPriceAlert(float threshold, bool above, bool oneTime);
    void checkAlerts(const BTCData& data);
    void sendAlert(const String& message);
    void clearAlert(int index);
};
```

#### 3. Configuration Storage
Add to NVRAM (Preferences):
```cpp
// Telegram configuration
String telegram_bot_token;
String telegram_chat_id;
bool telegram_enabled;
int telegram_check_interval; // milliseconds (default: 5000)

// Alert configuration
struct {
    float price_above[5];
    float price_below[5];
    float percent_change_1h;
    float percent_change_24h;
    bool daily_summary;
    bool system_notifications;
} alerts_config;
```

#### 4. Serial Commands
```bash
# Setup
SET_TELEGRAM_TOKEN=123456:ABC-DEF1234ghIkl-zyx57W2v1u123ew11
SET_TELEGRAM_CHAT_ID=123456789
TELEGRAM_ENABLE
TELEGRAM_DISABLE

# Alerts
ADD_PRICE_ALERT=100000,ABOVE,ONCE
ADD_PRICE_ALERT=90000,BELOW,REPEAT
CLEAR_PRICE_ALERT=0
LIST_ALERTS

# Testing
TELEGRAM_TEST  # Send test message
```

### Message Processing Flow

1. **Periodic Check** (every 5 seconds by default)
   - Check for new messages from Telegram
   - Parse commands and parameters
   - Execute requested actions
   - Send response back to user

2. **Alert Monitoring** (every data update)
   - Compare current BTC data with alert thresholds
   - Trigger notifications when conditions met
   - Mark one-time alerts as triggered

3. **Scheduled Reports** (based on timer)
   - Generate summary message
   - Send to configured chat ID
   - Update next scheduled time

### Security Considerations

1. **Bot Token Storage**
   - Store in NVRAM (not in code)
   - Mask in serial output (show only last 4 chars)
   - No plaintext logging

2. **Chat ID Validation**
   - Only respond to configured chat ID
   - Reject messages from unknown users
   - Optional: Support multiple authorized chat IDs

3. **Command Authentication**
   - Verify message sender matches chat ID
   - Rate limiting to prevent spam
   - Timeout for sensitive commands (restart, etc.)

4. **Network Security**
   - Use HTTPS for Telegram API
   - Certificate validation (if possible)
   - Handle network errors gracefully

## Setup Process

### Getting Started with Telegram Bot

#### 1. Create Telegram Bot
1. Open Telegram and search for `@BotFather`
2. Send `/newbot` command
3. Follow prompts to name your bot (e.g., "Bitcoin Dashboard Bot")
4. Save the bot token (e.g., `123456:ABC-DEF1234ghIkl-zyx57W2v1u123ew11`)

#### 2. Get Chat ID
1. Start a chat with your new bot
2. Send any message to the bot
3. Visit: `https://api.telegram.org/bot<YOUR_BOT_TOKEN>/getUpdates`
4. Find `"chat":{"id":123456789}` in the JSON response
5. Save the chat ID

#### 3. Configure Device
```bash
# Via serial monitor
SET_TELEGRAM_TOKEN=123456:ABC-DEF1234ghIkl-zyx57W2v1u123ew11
SET_TELEGRAM_CHAT_ID=123456789
TELEGRAM_ENABLE
STATUS  # Verify configuration

# Test the connection
TELEGRAM_TEST
```

#### 4. Set Up Alerts (Optional)
```bash
# Price alerts
ADD_PRICE_ALERT=100000,ABOVE,ONCE    # Alert once when BTC crosses $100k
ADD_PRICE_ALERT=90000,BELOW,REPEAT   # Alert every time BTC drops below $90k

# Enable daily summary
SET_TELEGRAM_DAILY_SUMMARY=1
```

### Example Configuration File
Add to `configs/keys.txt.template`:
```
# Telegram Bot Configuration
SET_TELEGRAM_TOKEN=<your_bot_token_here>
SET_TELEGRAM_CHAT_ID=<your_chat_id_here>

# Enable Telegram notifications
TELEGRAM_ENABLE

# Configure price alerts
ADD_PRICE_ALERT=100000,ABOVE,ONCE
ADD_PRICE_ALERT=95000,BELOW,REPEAT

# Enable features
SET_TELEGRAM_DAILY_SUMMARY=1
SET_TELEGRAM_SYSTEM_NOTIFICATIONS=1
```

## Memory Considerations

### Flash Memory
- **UniversalTelegramBot library:** ~30KB
- **TelegramClient implementation:** ~5KB
- **AlertManager implementation:** ~3KB
- **Total estimated:** ~40KB additional flash

Current flash usage: 1,031,961 bytes (6.3% of 16MB)
After Telegram: ~1,072,000 bytes (6.5% of 16MB)
**Status:** ✅ Well within limits

### RAM Memory
- **Bot instance:** ~2KB
- **Message buffers:** ~4KB (1KB per message × 4)
- **Alert storage:** ~200 bytes
- **Total estimated:** ~6KB additional RAM

Current RAM usage: 48,444 bytes (15% of 320KB)
After Telegram: ~54,500 bytes (17% of 320KB)
**Status:** ✅ Acceptable overhead

### Network Overhead
- **Check interval:** 5 seconds (configurable)
- **Data per check:** ~500 bytes (HTTPS request + response)
- **Data per hour:** ~360KB (720 checks × 500 bytes)
- **Monthly data:** ~260MB (assuming 24/7 operation)

**Recommendations:**
- Increase check interval to 10-30 seconds if WiFi is unstable
- Disable when not needed to save bandwidth
- Use webhooks instead of polling (advanced, requires public IP)

## API Rate Limits

**Telegram Bot API Limits:**
- 30 messages per second per bot
- 1 message per second per chat (for individual chats)
- No limit on incoming updates

**Our Usage:**
- Maximum: 1 message every 5 seconds (check interval)
- Typical: 1-5 messages per hour (alerts + reports)
- Peak: 10-20 messages per hour (active user interaction)

**Status:** ✅ Well within Telegram's limits

## Cost Analysis

**Telegram Bot API:** FREE (no cost)

**Bandwidth Cost:** Depends on ISP/cellular plan
- ~260MB/month for 5-second polling
- ~130MB/month for 10-second polling
- ~65MB/month for 20-second polling

**Recommendation:** Use WiFi instead of cellular data

## Development Timeline

### Phase 1: Basic Notifications (v2.1.0) - 2 weeks
- Week 1: Library integration, bot setup, basic send functionality
- Week 2: Price alerts, system notifications, testing

### Phase 2: Remote Monitoring (v2.2.0) - 1 week
- Command parser implementation
- Query commands (`/price`, `/status`, etc.)
- Screenshot transmission

### Phase 3: Remote Control (v2.3.0) - 1 week
- Configuration commands
- Device control commands
- Safety checks and validation

### Phase 4: Advanced Features (v2.4.0) - 2 weeks
- Trading signal notifications
- Custom alerts
- Interactive keyboards
- Optimization and polish

**Total Estimated Time:** 6 weeks

## Testing Strategy

### Unit Tests
- TelegramClient message sending
- Command parsing and validation
- Alert threshold checking
- Chat ID verification

### Integration Tests
- End-to-end message flow
- Screenshot capture and transmission
- Alert triggering on price changes
- Configuration persistence

### Manual Tests
- Real Telegram bot interaction
- Multiple users (authorized/unauthorized)
- Network failure scenarios
- Long-running stability (24+ hours)

## Documentation Requirements

### User Documentation
- **docs/guides/telegram-setup.md** - Step-by-step setup guide
- **docs/guides/telegram-commands-reference.md** - All bot commands
- **README.md updates** - Telegram features section

### Developer Documentation
- **docs/features/telegram-bot-integration.md** - This file (architecture)
- **CLAUDE.md updates** - Telegram configuration guidance
- **Code comments** - TelegramClient and AlertManager classes

## Risks and Mitigations

### Risk 1: Network Overhead
- **Impact:** Polling every 5 seconds may strain WiFi
- **Mitigation:** Configurable check interval, webhook option for advanced users
- **Status:** Low risk - 500 bytes per check is minimal

### Risk 2: Bot Token Security
- **Impact:** Exposed token allows unauthorized access to bot
- **Mitigation:** NVRAM storage, masked serial output, no logging
- **Status:** Medium risk - need user education on token security

### Risk 3: Message Spam
- **Impact:** User could spam commands, causing device issues
- **Mitigation:** Rate limiting, command throttling, max 1 command per 2 seconds
- **Status:** Low risk - easy to implement rate limiting

### Risk 4: Telegram API Changes
- **Impact:** Breaking changes could stop functionality
- **Mitigation:** Use stable library (UniversalTelegramBot), pin library version
- **Status:** Low risk - Telegram API is stable

### Risk 5: Memory Leaks
- **Impact:** Long-running bot could exhaust RAM
- **Mitigation:** Careful memory management, periodic restart option
- **Status:** Medium risk - requires thorough testing

## Success Criteria

### Minimum Viable Product (MVP)
- ✅ Bot can send messages to configured chat
- ✅ User can query current BTC price
- ✅ Price alerts trigger correctly
- ✅ Configuration persists across reboots
- ✅ No performance degradation on main dashboard

### Full Feature Set
- ✅ All Phase 1-3 features implemented
- ✅ Comprehensive documentation
- ✅ Unit tests for core functionality
- ✅ 24-hour stability test passes
- ✅ User-friendly setup process

## Future Enhancements (v3.0+)

### Webhook Support
- Replace polling with webhooks (requires public IP or ngrok)
- Instant message delivery
- Reduced bandwidth usage
- More complex to set up

### Multi-User Support
- Allow multiple authorized chat IDs
- Per-user alert preferences
- Admin vs. read-only access levels

### Voice Commands
- Telegram voice message support
- "What's the current Bitcoin price?"
- Requires speech-to-text integration

### Charts and Graphs
- Send price charts as images
- Historical data visualization
- Interactive chart controls via inline buttons

### Telegram Mini App
- Embedded web interface in Telegram
- Touch-friendly controls
- Real-time data updates

## References

### Libraries
- **UniversalTelegramBot:** https://github.com/witnessmenow/Universal-Arduino-Telegram-Bot
- **AsyncTelegram2:** https://github.com/cotestatnt/AsyncTelegram2

### Telegram Bot API
- **Official Documentation:** https://core.telegram.org/bots/api
- **BotFather Guide:** https://core.telegram.org/bots#6-botfather
- **Webhook Guide:** https://core.telegram.org/bots/webhooks

### Examples
- **ESP32 Telegram Examples:** https://github.com/witnessmenow/Universal-Arduino-Telegram-Bot/tree/master/examples
- **Price Alert Bots:** Various GitHub repositories

### Related Documentation
- **docs/features/trading.md** - AI trading suggestions (can be sent via Telegram)
- **docs/features/news.md** - Bitcoin news (can be delivered via Telegram)
- **docs/configuration.md** - Configuration system (used for Telegram settings)
- **docs/security.md** - Security best practices (applies to bot token storage)

---

**Status:** Planning Phase
**Last Updated:** 2025-11-29
**Next Steps:** Review plan, gather user feedback, prioritize features for v2.1.0
