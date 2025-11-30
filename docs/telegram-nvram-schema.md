# Telegram Bot NVRAM Configuration Schema

## Overview

This document defines the NVRAM (Non-Volatile RAM) storage schema for Telegram bot configuration on the Bitcoin Dashboard ESP32-S3 device. All Telegram-related settings are persisted using the ESP32 Preferences library under the `btc_dash` namespace.

## NVRAM Keys

### Telegram Authentication

| Key Name | Type | NVRAM Key | Default | Description |
|----------|------|-----------|---------|-------------|
| `telegramToken` | String | `tg_token` | "" | Bot authentication token from @BotFather |
| `telegramChatId` | String | `tg_chat_id` | "" | User's Telegram chat ID for notifications |
| `telegramEnabled` | bool | `tg_enabled` | false | Master enable/disable for Telegram notifications |

### Price Alerts

| Key Name | Type | NVRAM Key | Default | Description |
|----------|------|-----------|---------|-------------|
| `priceAlertHigh` | float | `price_hi` | 0.0 | Upper price threshold (USD). Alert when BTC > this value. 0 = disabled |
| `priceAlertLow` | float | `price_lo` | 0.0 | Lower price threshold (USD). Alert when BTC < this value. 0 = disabled |

### Percentage Change Alerts

| Key Name | Type | NVRAM Key | Default | Description |
|----------|------|-----------|---------|-------------|
| `alert5Percent` | bool | `alert_5pct` | false | Alert on 5% price change from baseline |
| `alert10Percent` | bool | `alert_10pct` | false | Alert on 10% price change from baseline |
| `alert20Percent` | bool | `alert_20pct` | false | Alert on 20% price change from baseline |

### Daily Reports

| Key Name | Type | NVRAM Key | Default | Description |
|----------|------|-----------|---------|-------------|
| `dailyReportEnabled` | bool | `daily_rpt_en` | false | Enable/disable daily market summary reports |
| `dailyReportHour` | int | `daily_rpt_hr` | 8 | Hour (0-23) for daily report delivery |
| `dailyReportMinute` | int | `daily_rpt_min` | 0 | Minute (0-59) for daily report delivery |

### Alert Management

| Key Name | Type | NVRAM Key | Default | Description |
|----------|------|-----------|---------|-------------|
| `alertCooldown` | unsigned long | `alert_cool` | 300000 | Cooldown period between alerts (milliseconds). Default: 5 minutes |

## Serial Commands Reference

### Basic Configuration

**Set Telegram Bot Token**
```
SET_TELEGRAM_TOKEN=1234567890:ABCdefGHIjklMNOpqrsTUVwxyz
```
- Get token from [@BotFather](https://t.me/BotFather)
- Token format: `<bot_id>:<auth_hash>`
- Stored with masked output (shows only last 4 chars in logs)

**Set Telegram Chat ID**
```
SET_TELEGRAM_CHAT_ID=123456789
```
- Get your chat ID by messaging [@userinfobot](https://t.me/userinfobot)
- Chat ID is a numeric value (can be negative for groups)
- Required for bot to know where to send notifications

**Enable/Disable Notifications**
```
TELEGRAM_ENABLE
TELEGRAM_DISABLE
```
- `TELEGRAM_ENABLE` requires both token and chat ID to be configured
- Master switch for all Telegram notifications

### Price Alerts

**Set Upper Price Alert**
```
SET_PRICE_ALERT_HIGH=100000
```
- Alert when BTC price exceeds this threshold
- Value in USD (float)
- Set to 0 to disable: `SET_PRICE_ALERT_HIGH=0`

**Set Lower Price Alert**
```
SET_PRICE_ALERT_LOW=50000
```
- Alert when BTC price falls below this threshold
- Value in USD (float)
- Set to 0 to disable: `SET_PRICE_ALERT_LOW=0`

**Configure Percentage Change Alerts**
```
SET_PRICE_CHANGE_ALERT=5,10,20
```
- Enable multiple percentage alerts in one command
- Comma-separated values: 5, 10, or 20
- Examples:
  - `SET_PRICE_CHANGE_ALERT=5` - Only 5% alerts
  - `SET_PRICE_CHANGE_ALERT=10,20` - 10% and 20% alerts
  - `SET_PRICE_CHANGE_ALERT=5,10,20` - All three alerts

### Daily Reports

**Enable Daily Report**
```
SET_DAILY_REPORT=08:00
```
- Format: `HH:MM` (24-hour format)
- Hour range: 0-23
- Minute range: 0-59
- Automatically enables daily reports

**Disable Daily Report**
```
DISABLE_DAILY_REPORT
```
- Turns off daily report delivery
- Preserves configured time for re-enabling

### Testing and Status

**Test Telegram Configuration**
```
TEST_TELEGRAM
```
- Validates configuration is stored correctly
- Shows masked token (last 4 chars only)
- Shows chat ID and enabled status
- Note: Does NOT send actual message (requires TelegramClient implementation)

**Show Telegram Status**
```
TELEGRAM_STATUS
```
- Displays complete Telegram configuration
- Shows all alert thresholds
- Shows daily report settings
- Shows alert cooldown period

## Configuration Workflow

### Initial Setup

1. **Create Telegram Bot**
   - Message [@BotFather](https://t.me/BotFather) on Telegram
   - Send `/newbot` command
   - Follow prompts to create bot
   - Copy the bot token

2. **Get Your Chat ID**
   - Message [@userinfobot](https://t.me/userinfobot)
   - Copy your chat ID (numeric value)

3. **Configure ESP32 Device**
   ```
   SET_TELEGRAM_TOKEN=your_token_here
   SET_TELEGRAM_CHAT_ID=your_chat_id_here
   TELEGRAM_ENABLE
   ```

4. **Verify Configuration**
   ```
   TELEGRAM_STATUS
   TEST_TELEGRAM
   ```

### Example Configurations

**Conservative Alerts (Minimal Noise)**
```
SET_PRICE_ALERT_HIGH=100000
SET_PRICE_ALERT_LOW=60000
SET_PRICE_CHANGE_ALERT=20
SET_DAILY_REPORT=08:00
TELEGRAM_ENABLE
```

**Active Trader (Frequent Updates)**
```
SET_PRICE_CHANGE_ALERT=5,10,20
SET_DAILY_REPORT=07:00
TELEGRAM_ENABLE
```

**Price Range Monitor**
```
SET_PRICE_ALERT_HIGH=95000
SET_PRICE_ALERT_LOW=85000
DISABLE_DAILY_REPORT
TELEGRAM_ENABLE
```

## Security Considerations

### Token Masking
- Bot tokens are masked in serial output (shows only last 4 characters)
- Full tokens are logged to SD card with masking: `...WXYZ (46 chars)`
- Never display full token in Serial Monitor to prevent shoulder surfing

### Chat ID Privacy
- Chat IDs are NOT masked (they're not secret credentials)
- Required for bot to identify the recipient
- Can be shared without security risk

### Configuration Access
- All configuration stored in ESP32 NVS (encrypted flash partition)
- Requires physical access to device for serial configuration
- No remote configuration API (security by design)

## Data Persistence

### Storage Location
- **Namespace**: `btc_dash`
- **Backend**: ESP32 NVS (Non-Volatile Storage)
- **Partition**: Default NVS partition (~20KB)

### Storage Size Estimates
| Data Type | Max Size | Typical Size |
|-----------|----------|--------------|
| Bot Token | ~64 bytes | 46 bytes |
| Chat ID | ~16 bytes | 10 bytes |
| Booleans | 1 byte each | 1 byte |
| Floats | 4 bytes each | 4 bytes |
| Integers | 4 bytes each | 4 bytes |
| **Total** | ~120 bytes | ~80 bytes |

### Configuration Backup
- Configuration persists across reboots and firmware updates
- To backup: Use `STATUS` command and save output
- To restore: Re-enter commands via serial
- Future: Export/import configuration file support

## Validation Rules

### Input Validation

**Token Validation**
- Must not be empty string
- No format validation (trust @BotFather output)

**Chat ID Validation**
- Must not be empty string
- Can be positive (user) or negative (group)
- Numeric value only

**Price Thresholds**
- Must be >= 0 (float)
- 0 means "disabled"
- No upper limit check

**Time Validation**
- Hour: 0-23 (inclusive)
- Minute: 0-59 (inclusive)
- Enforced in `setDailyReportTime()` method

**Percentage Alerts**
- Only 5, 10, 20 are valid values
- Other values silently ignored

## API Reference for Other Agents

### ConfigManager Getters

```cpp
// Authentication
String getTelegramToken() const;
String getTelegramChatId() const;
bool isTelegramEnabled() const;
bool hasTelegramConfig() const;  // Returns true if token AND chatId configured

// Price Alerts
float getPriceAlertHigh() const;
float getPriceAlertLow() const;

// Percentage Alerts
bool isAlert5Percent() const;
bool isAlert10Percent() const;
bool isAlert20Percent() const;

// Daily Reports
bool isDailyReportEnabled() const;
int getDailyReportHour() const;
int getDailyReportMinute() const;

// Alert Management
unsigned long getAlertCooldown() const;
```

### ConfigManager Setters

```cpp
// Authentication
void setTelegramToken(const String& token);
void setTelegramChatId(const String& chatId);
void setTelegramEnabled(bool enabled);

// Price Alerts
void setPriceAlertHigh(float threshold);
void setPriceAlertLow(float threshold);

// Percentage Alerts
void setAlert5Percent(bool enabled);
void setAlert10Percent(bool enabled);
void setAlert20Percent(bool enabled);

// Daily Reports
void setDailyReportEnabled(bool enabled);
void setDailyReportTime(int hour, int minute);  // Validates hour 0-23, minute 0-59

// Alert Management
void setAlertCooldown(unsigned long cooldown);
```

### Usage Example

```cpp
// Check if Telegram is configured and enabled
if (globalConfig.isTelegramEnabled() && globalConfig.hasTelegramConfig()) {
    // Send notification
    String token = globalConfig.getTelegramToken();
    String chatId = globalConfig.getTelegramChatId();

    // Check if price alert should fire
    float currentPrice = 95000.0;
    if (currentPrice > globalConfig.getPriceAlertHigh() &&
        globalConfig.getPriceAlertHigh() > 0) {
        // Send high price alert
    }
}
```

## Integration Notes for Other Agents

### bitcoin-api-specialist
- Use `globalConfig.getPriceAlert*()` methods to check thresholds
- Compare current BTC price against configured alerts
- Implement cooldown logic using `getAlertCooldown()`

### ui-designer
- Consider adding Telegram configuration screen (future enhancement)
- Show Telegram status indicator on main screen
- Display when last alert was sent

### build-specialist
- Telegram configuration adds ~80 bytes to NVRAM usage
- No additional library dependencies yet (TelegramClient TBD)
- All code in existing Config.h/cpp files

### debugger
- Use `TELEGRAM_STATUS` command for diagnostics
- Check SD logs for Telegram configuration changes
- Monitor alert cooldown timers in runtime debugging

## Future Enhancements

1. **Alert Cooldown Per Alert Type**
   - Separate cooldowns for price vs percentage alerts
   - Current: Single global cooldown

2. **Custom Alert Messages**
   - User-defined message templates
   - Current: Fixed message format (TBD by bitcoin-api-specialist)

3. **Multiple Chat IDs**
   - Support group notifications
   - Current: Single chat ID only

4. **Alert History**
   - Log when alerts were sent to SD card
   - Current: No alert history tracking

5. **Web-based Configuration**
   - Configure via web interface (requires WiFi)
   - Current: Serial-only configuration

## Version History

- **v1.0** (2025-11-30): Initial NVRAM schema design
  - 12 configuration keys
  - Serial command interface
  - Security masking for tokens
  - Validation for time and percentage values

## Related Documentation

- `/Users/lps/Solutions/bemind-embeded/btc-dashboard/src/Config.h` - NVRAM key definitions
- `/Users/lps/Solutions/bemind-embeded/btc-dashboard/src/Config.cpp` - Implementation
- `/Users/lps/Solutions/bemind-embeded/btc-dashboard/src/main.cpp` - Serial command handlers
