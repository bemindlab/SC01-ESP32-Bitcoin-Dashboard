# Development Plan

Bitcoin Dashboard for Panlee SC01 Plus - Strategic development roadmap and feature planning.

## Executive Summary

The Bitcoin Dashboard has evolved from a multi-screen system to a **unified MainScreen with smooth 120 FPS scrolling** and AI-powered analysis. The project now features:

**Key Achievements:**
- 🎯 **MainScreen (Unified Interface)** - Single screen with 8 cards, smooth vertical scrolling
- ⚡ **120 FPS Scrolling** - Optimized rendering with 8ms frame time, GPU acceleration
- 🧠 **AI Integration** - Gemini API for DCA recommendations and trading signals
- 🎨 **Bitcoin Orange Theme** - Modern UI with #F7931A header and white text
- ⚙️ **NVRAM Configuration** - Persistent WiFi credentials and API keys
- 🤖 **Multi-Agent Development** - Tmux-based parallel workflow system
- 💾 **SD Card Logging** - Optional data logging with serial commands
- 🛡️ **Crash Handler** - Exception logging and recovery
- 📸 **Developer Tools** - Screenshot capture, serial commands, comprehensive docs

**Current Focus (v2.1.0 - In Progress):**
- Telegram bot integration infrastructure (✅ Week 1 complete)
- Alert system implementation (AlertManager)
- Price threshold and percentage alerts
- Daily summary reports
- End-to-end testing

**Completed (v2.0.0):**
- ✅ Smooth scrolling optimization (achieved 120 FPS)
- ✅ AI-powered DCA and trading signals
- ✅ Multi-agent development workflows
- ✅ Performance tuning and memory efficiency

**Next Milestone (v2.1.0):**
- Complete Telegram bot notifications (Week 2)
- Remote monitoring commands (Phase 2)
- Device control via Telegram (Phase 3)

## Project Status

**Current Version:** 2.0.0 → 2.1.0 (In Development)
**Status:** Production-Ready v2.0.0 + Telegram Bot Infrastructure (Week 1)
**Last Updated:** 2025-11-30
**Performance:** 120 FPS scrolling, 16.4% RAM, 16.7% Flash
**Build Status:** ✅ Compiles successfully

### Completed Features ✓

#### MainScreen (v2.0 - Unified Interface)
- [x] Single unified screen with 8 information cards
- [x] Smooth 120 FPS vertical scrolling (8ms frame time)
- [x] Bitcoin orange header (#F7931A) with white text
- [x] 4 rows × 2 columns card layout (412px scroll range)
- [x] Optimized rendering with batch operations
- [x] GPU clipping regions for acceleration
- [x] Simplified card design (no shadows/rounded corners)
- [x] Touch-responsive scrolling (immediate feedback)
- [x] Screen rotation support (tap top-right corner)
- [x] BTC Price (USD/EUR) cards
- [x] Block Height and Mempool Count cards
- [x] Fast Fee and WiFi Signal cards
- [x] DCA Signal (AI) - Gemini recommendations
- [x] Trading Signal (AI) - 15m-1h timeframe analysis

#### Performance Optimizations (v2.0)
- [x] 120 FPS target achieved (110-120 FPS actual)
- [x] 8ms minimum draw interval (down from 50ms)
- [x] 1-pixel scroll threshold (down from 5px)
- [x] Batch rendering with startWrite/endWrite
- [x] Clipping regions for GPU acceleration
- [x] Removed rounded corners and shadows
- [x] Immediate scroll response (no delay)
- [x] Touch response <50ms
- [x] Memory: RAM 16.3%, Flash 16.5%

#### AI-Powered Analysis (v2.0)
- [x] Gemini API integration
- [x] DCA (Dollar Cost Average) recommendations
  - [x] BUY/SELL/WAIT signals
  - [x] Color-coded cards (Green/Red/Yellow)
  - [x] 5-minute update interval
- [x] Trading Signal (15m-1h timeframe)
  - [x] BUY/SELL/HOLD signals
  - [x] Color-coded cards (Green/Red/Gray)
  - [x] Short-term market analysis
- [x] API key storage in NVRAM
- [x] Cost-effective update intervals

#### Multi-Agent Development System (v2.0)
- [x] Tmux-based session manager
- [x] 7-window layout with specialized agents
  - [x] Window 0: Coordinator (main control)
  - [x] Window 1: ESP32 Hardware Specialist
  - [x] Window 2: UI/UX Designer
  - [x] Window 3: Bitcoin API Specialist
  - [x] Window 4: Embedded Debugger
  - [x] Window 5: Build Specialist
  - [x] Window 6: Global Monitor
- [x] Inter-agent communication system
  - [x] agent_log - Log to own log
  - [x] agent_broadcast - Broadcast to all
  - [x] agent_send - Send to specific agent
  - [x] agent_read - Read messages
  - [x] agent_monitor - Live tail
- [x] Makefile integration
  - [x] make agents - Start session
  - [x] make agents-attach - Reattach
  - [x] make agents-stop - Stop session
  - [x] make agents-status - Show status
  - [x] make agents-logs - View logs
- [x] Communication logs (.tmp/agent-comm/)
- [x] Session persistence (detach/reattach)
- [x] Documentation (CLAUDE.md, guides)

#### Core Dashboard (v1.0)
- [x] Real-time BTC price display (USD/EUR)
- [x] Latest block information
- [x] Mempool statistics
- [x] Fee rate recommendations (Fast/Medium/Slow)
- [x] WiFi connectivity with auto-reconnect
- [x] Touch screen support
- [x] Serial debugging capabilities
- [x] PlatformIO build configuration
- [x] Comprehensive documentation

#### Configuration System (v1.2)
- [x] NVRAM-based persistent storage (Preferences library)
- [x] WiFi credentials management (auto-connect on boot)
- [x] API key storage (Gemini)
- [x] Configurable update intervals (price, blocks, mempool, AI)
- [x] Serial command interface (SET_*, STATUS)
- [x] First-run detection and setup wizard
- [x] Secure key storage with masked serial output
- [x] Configuration via serial (115200 baud)

#### API Integrations (v1.3 + v2.0)
- [x] mempool.space API (BTC price, blocks, mempool, fees)
- [x] Gemini API client (DCA + Trading signals)
- [x] Configurable HTTP client with timeouts
- [x] JSON response parsing with validation
- [x] Error handling and retry logic
- [x] 5-minute AI update interval (cost-effective)

#### SD Card Logging (v1.4)
- [x] SD card library integration (GPIO 38-41)
- [x] SDLogger utility class
- [x] Log levels (DEBUG, INFO, WARN, ERROR, FATAL)
- [x] Buffered writes (4KB buffer)
- [x] Serial commands (CHECK_SD_CARD, LOG_ENABLE, etc.)
- [x] Makefile commands (make sd-status, sd-format, etc.)
- [x] Hot-swap detection
- [x] Graceful degradation on errors
- [x] Documentation (guides, troubleshooting)

#### Crash Handler (v1.4)
- [x] Exception logging and stack traces
- [x] Automatic crash detection
- [x] Pre-reboot log flush
- [x] Memory allocation failure tracking
- [x] Crash recovery notifications

#### Screenshot & Debugging (v1.5)
- [x] Screenshot capture functionality
- [x] Serial monitor screenshot transmission
- [x] SCREENSHOT command via serial
- [x] Python capture scripts
- [x] Debugging utilities

#### Documentation & Security (v1.6 + v2.0)
- [x] Comprehensive README with all features
- [x] Complete pin configuration (Display, Touch, SD, USB)
- [x] Available GPIO documentation (14 unused pins)
- [x] Multi-agent development guide
- [x] SECURITY.md with security policies
- [x] LICENSE file (MIT License)
- [x] Hardware specifications
- [x] Serial commands reference
- [x] Troubleshooting guides
- [x] Architecture diagrams
- [x] Performance specifications

#### Telegram Bot Integration - Infrastructure (v2.1.0 - Week 1)
- [x] UniversalTelegramBot library integration
  - [x] Added to platformio.ini (witnessmenow/UniversalTelegramBot@^1.3.0)
  - [x] ~30KB flash, ~6KB RAM overhead
- [x] TelegramClient wrapper class
  - [x] src/telegram/TelegramClient.h (class definition)
  - [x] src/telegram/TelegramClient.cpp (implementation)
  - [x] WiFiClientSecure HTTPS setup
  - [x] Message queue system (10 message circular buffer)
  - [x] 1-second update interval
  - [x] Error handling and logging
- [x] NVRAM configuration system (12 keys)
  - [x] telegram_token, telegram_chat_id (authentication)
  - [x] telegram_enabled (master switch)
  - [x] price_threshold_high, price_threshold_low (price alerts)
  - [x] alert_5pct, alert_10pct, alert_20pct (percentage alerts)
  - [x] daily_report_enabled, daily_report_hour, daily_report_minute
  - [x] alert_cooldown_ms (spam prevention)
  - [x] Config.h/cpp updated with getters/setters
- [x] Serial command interface (11 commands)
  - [x] SET_TELEGRAM_TOKEN=xxx (masked output)
  - [x] SET_TELEGRAM_CHAT_ID=xxx
  - [x] TELEGRAM_ENABLE / TELEGRAM_DISABLE
  - [x] SET_PRICE_ALERT_HIGH/LOW=xxx
  - [x] SET_PRICE_CHANGE_ALERT=5,10,20
  - [x] SET_DAILY_REPORT=HH:MM
  - [x] DISABLE_DAILY_REPORT
  - [x] TELEGRAM_STATUS
  - [x] TEST_TELEGRAM (placeholder)
  - [x] Updated HELP command
- [x] Documentation
  - [x] docs/telegram-nvram-schema.md (complete API reference)
  - [x] Integration notes for all agents
  - [x] Security considerations
  - [x] Example configurations

## Development Phases

### Phase 1: Core Stability ✅ COMPLETE

**Goals:**
- ✅ Stable hardware compatibility
- ✅ Reliable WiFi connectivity
- ✅ API rate limiting
- ✅ Error handling

**Completed:**
- [x] Hardware verification (SC01 Plus)
- [x] WiFi auto-reconnect
- [x] API timeout handling
- [x] Configuration system
- [x] Serial debugging

### Phase 2: MainScreen with Smooth Scrolling ✅ COMPLETE (v2.0)

**Goals:**
- ✅ Unified single-screen interface
- ✅ 120 FPS smooth scrolling
- ✅ AI-powered analysis
- ✅ Modern UI design

**Completed Features:**
- [x] MainScreen implementation (8 cards)
- [x] 120 FPS vertical scrolling
- [x] Bitcoin orange theme (#F7931A)
- [x] Performance optimizations
  - [x] 8ms frame time (was 50ms)
  - [x] Batch rendering
  - [x] GPU clipping
  - [x] Simplified card design
- [x] AI integration
  - [x] DCA recommendations
  - [x] Trading signals (15m-1h)
  - [x] Gemini API client
  - [x] 5-minute updates
- [x] Touch interactions
  - [x] Smooth scrolling
  - [x] Screen rotation
  - [x] Immediate feedback
- [x] Memory efficiency
  - [x] RAM: 16.3% (53,404 bytes)
  - [x] Flash: 16.5% (1,078,861 bytes)

**Success Metrics:**
- ✅ 110-120 FPS achieved (target: 120 FPS)
- ✅ <50ms touch response
- ✅ <100ms screen rotation
- ✅ 16.3% RAM usage (target: <20%)
- ✅ Smooth user experience

### Phase 3: Multi-Agent Development System ✅ COMPLETE (v2.0)

**Goals:**
- ✅ Parallel development workflows
- ✅ Agent communication
- ✅ Session management
- ✅ Documentation

**Completed Features:**
- [x] Tmux session manager (7 windows)
- [x] Specialized agents (5 + coordinator + monitor)
- [x] Communication API (log, broadcast, send)
- [x] Makefile integration (6 commands)
- [x] Communication logs (.tmp/agent-comm/)
- [x] Session persistence
- [x] CLAUDE.md workflows
- [x] Multi-agent development guide

**Agent Types:**
1. **Coordinator** - Task management and oversight
2. **ESP32 Hardware** - Hardware config, memory analysis
3. **UI/UX Designer** - LVGL, layouts, styling
4. **Bitcoin API** - API integration, JSON parsing
5. **Embedded Debugger** - Serial debugging, crash analysis
6. **Build Specialist** - Compilation, optimization
7. **Global Monitor** - All agent communications

**Success Metrics:**
- ✅ 7 agent windows functional
- ✅ Communication logging working
- ✅ Makefile commands operational
- ✅ Documentation complete
- ✅ Workflow examples provided

### Phase 4: Extended Metrics & Features (Current)

**Goals:**
- Add historical data visualization
- Expand Bitcoin metrics
- Improve analytics

**Planned Features:**
- [ ] Price change indicators (↑↓ arrows, % change)
- [ ] Historical price chart (last 24h)
- [ ] Block time countdown timer
- [ ] Fee rate color coding (green/yellow/red)
- [ ] Difficulty adjustment countdown
- [ ] Hashrate display
- [ ] Lightning Network statistics
- [ ] 24h high/low prices
- [ ] Transaction volume statistics

**API Endpoints to Add:**
- [ ] `/api/v1/difficulty-adjustment`
- [ ] `/api/v1/lightning/statistics/latest`
- [ ] `/api/v1/mining/hashrate/pools/1w`

### Phase 5: Telegram Bot Integration (In Progress v2.1.0)

**Goals:**
- Price alerts and notifications
- Remote monitoring
- Remote device control
- Screenshot transmission

**Status:** 🚧 Week 1 Infrastructure Complete (2025-11-30)

**Infrastructure Complete (Week 1):**
- [x] UniversalTelegramBot library integration (platformio.ini)
- [x] TelegramClient wrapper class (src/telegram/TelegramClient.h/cpp)
- [x] WiFiClientSecure HTTPS setup
- [x] Message queue system (prevent spam)
- [x] NVRAM configuration schema (12 keys)
- [x] Serial command interface (11 commands)
- [x] Token security masking
- [x] Configuration validation
- [x] Documentation (docs/telegram-nvram-schema.md)

**Planned Features:**
- [ ] **Phase 1: Basic Notifications** (2 weeks - In Progress)
  - [x] TelegramClient infrastructure
  - [x] NVRAM configuration system
  - [x] Serial commands for setup
  - [ ] AlertManager class implementation
  - [ ] Price threshold alerts (above/below)
  - [ ] Percentage change alerts (5%, 10%, 20%)
  - [ ] System notifications (boot, WiFi, errors)
  - [ ] Daily summary reports
  - [ ] Integration with main loop
  - [ ] End-to-end testing

- [ ] **Phase 2: Remote Monitoring** (1 week)
  - [ ] Query commands (/price, /blocks, /mempool, /fees, /status)
  - [ ] Screenshot command (/screenshot)
  - [ ] Scheduled reports (hourly, daily, weekly)

- [ ] **Phase 3: Remote Control** (1 week)
  - [ ] Configuration commands (set intervals, alerts)
  - [ ] Device control (restart, refresh, rotate screen)
  - [ ] Brightness control

- [ ] **Phase 4: Advanced Features** (2 weeks)
  - [ ] Trading signal notifications
  - [ ] Custom alerts (block milestones, mempool congestion)
  - [ ] Interactive inline keyboards
  - [ ] Conversation-based setup

**Technical Requirements:**
- [x] UniversalTelegramBot library (~30KB flash, ~6KB RAM)
- [x] TelegramClient class implementation
- [ ] AlertManager class for threshold monitoring
- [x] NVRAM configuration for bot token and chat ID
- [x] Serial commands: SET_TELEGRAM_TOKEN, SET_TELEGRAM_CHAT_ID, etc.

**Timeline:** 6 weeks total (Q1 2025)
**Progress:** Week 1 Complete (Infrastructure)

**See:** `docs/features/telegram-bot-integration.md` for detailed specification

### Phase 6: Advanced Connectivity & OTA (Planned v2.2.0)

**Goals:**
- Web configuration interface
- OTA firmware updates
- MQTT monitoring
- Multiple WiFi networks

**Planned Features:**
- [ ] WiFi manager with captive portal
- [ ] Multiple WiFi network support (failover)
- [ ] NTP time synchronization
- [ ] OTA (Over-The-Air) firmware updates
- [ ] Web interface for configuration
- [ ] MQTT publishing for monitoring
- [ ] REST API for status queries

**Technical Requirements:**
- WiFiManager library
- NTPClient library
- AsyncWebServer library
- ArduinoOTA library
- PubSubClient (MQTT)

**Timeline:** Q2 2025

### Phase 7: Optimization & Performance (Ongoing)

**Goals:**
- Maintain optimal performance
- Reduce power consumption
- Memory profiling

**Tasks:**
- [x] 120 FPS scrolling achieved
- [x] Batch rendering optimization
- [x] GPU acceleration (clipping)
- [x] Memory efficiency (16.3% RAM, 16.5% Flash)
- [ ] Deep sleep during inactivity
- [ ] Battery operation support
- [ ] Power consumption profiling
- [ ] Long-term stability testing (48+ hours)

## Recent Achievements (v2.0.0)

### Major Milestones

**MainScreen Implementation (2025-11-29):**
- ✅ Unified single-screen interface replacing multi-screen system
- ✅ 8 information cards in 4 rows × 2 columns
- ✅ 120 FPS smooth vertical scrolling (8ms frame time)
- ✅ Bitcoin orange header (#F7931A) with white text
- ✅ Performance optimizations (batch rendering, GPU clipping)
- ✅ AI-powered DCA and Trading signals
- ✅ Removed legacy screens (BTCDashboard, News, Trading, Settings)
- ✅ Memory: RAM 16.3%, Flash 16.5%

**Multi-Agent System (2025-11-29):**
- ✅ Tmux-based development environment
- ✅ 7-window layout with specialized agents
- ✅ Inter-agent communication system
- ✅ Makefile integration (make agents, agents-attach, agents-stop)
- ✅ Communication logs and monitoring
- ✅ Comprehensive documentation

**Scroll Performance (2025-11-29):**
- ✅ 110-120 FPS achieved (from 20 FPS)
- ✅ 8ms frame time (from 50ms)
- ✅ 1-pixel threshold (from 5px)
- ✅ Immediate touch response
- ✅ Simplified card rendering
- ✅ Batch operations and clipping

### Code Quality Improvements
- ✅ Consolidated screens into unified MainScreen
- ✅ Optimized rendering pipeline
- ✅ Removed unnecessary complexity
- ✅ Improved touch responsiveness
- ✅ Better memory efficiency
- ✅ Enhanced documentation

## Technical Debt & Improvements

### High Priority
- [ ] NTP time synchronization (required for SD logging timestamps)
- [ ] Watchdog timer for crash recovery
- [ ] millis() rollover handling (49-day bug)
- [ ] Certificate validation for HTTPS
- [ ] Long-term stability testing (48+ hours)

### Medium Priority
- [ ] Historical data logging to SD card
- [ ] Log rotation and retention
- [ ] Visual loading indicators
- [ ] Error state UI feedback
- [ ] Memory leak detection
- [ ] Performance profiling on hardware

### Low Priority
- [ ] CI/CD pipeline (GitHub Actions)
- [ ] Hardware integration tests
- [ ] Coverage reporting
- [ ] Multiple hardware platform support

## Hardware Considerations

### Tested Hardware
- Panlee SC01 Plus (ESP32-S3, 16MB Flash, 2MB PSRAM)

### Pin Configuration
- **Display:** Parallel 8-bit (D0-D7, WR, RS, RST, BL)
- **Touch:** I2C (SDA=18, SCL=19, INT=7)
- **SD Card:** SPI (CS=41, MOSI=40, MISO=38, CLK=39)
- **USB:** D+=20, D-=19 (Serial/JTAG)

### Available GPIOs (14 unused)
- GPIO 1, 2, 5, 6, 10, 11, 12, 13, 14, 21, 42, 43, 44, 48
- Available for sensors, relays, LEDs, additional I2C/SPI

### Future Hardware Support
- [ ] SC01 (non-Plus version)
- [ ] Other ESP32-S3 displays
- [ ] Larger displays (4.3", 7")
- [ ] E-ink displays

## Security Considerations

### Current Status (v2.0.0)
- ✅ WiFi credentials stored in NVRAM
- ✅ API keys stored in NVRAM with masked output
- ✅ SECURITY.md documentation
- ⏳ HTTPS connections (TLS enabled, no cert validation)
- ⏳ No encrypted storage (plaintext NVRAM)

### Improvements Needed
- [ ] HTTPS certificate validation
- [ ] Encrypted preferences storage
- [ ] Flash encryption for production
- [ ] Secure boot implementation
- [ ] Rate limiting for API calls

## Testing Strategy

### Completed Testing
- ✅ Hardware compatibility (SC01 Plus)
- ✅ UI/UX on actual device
- ✅ Scrolling performance (120 FPS verified)
- ✅ AI API integration (Gemini)
- ✅ Touch responsiveness
- ✅ Screen rotation
- ✅ Serial commands
- ✅ SD card logging

### Manual Testing Needed
- [ ] Long-running stability (48+ hours)
- [ ] Memory leak detection
- [ ] Network failure scenarios
- [ ] Power loss recovery
- [ ] SD card hot-swap

### Automated Testing
- [ ] CI/CD integration
- [ ] Performance benchmarking
- [ ] Regression testing
- [ ] Memory profiling

## Release Strategy

### Version Numbering
- Major.Minor.Patch (Semantic Versioning)
- Current: **2.0.0**

### Milestone Releases

**Completed:**
- ✅ **v1.0.0-alpha** - Core dashboard
- ✅ **v1.0.0-beta** - Multi-screen system
- ✅ **v2.0.0** (Current) - MainScreen with 120 FPS scrolling and AI

**Planned:**
- **v2.1.0** (Q1 2025) - Telegram Bot Integration
  - Price alerts and notifications
  - Remote monitoring (/price, /status, /screenshot)
  - Remote device control
  - Daily/weekly reports

- **v2.2.0** (Q2 2025) - Advanced Connectivity
  - Web configuration interface
  - OTA firmware updates
  - MQTT monitoring
  - Multiple WiFi networks

- **v2.3.0** (Q2 2025) - Extended Metrics
  - Historical price charts
  - Difficulty adjustment data
  - Lightning Network statistics
  - Fee distribution histograms

- **v3.0.0** (Q3 2025) - Enterprise Features
  - Multi-device management
  - Advanced analytics
  - Custom API integrations
  - Plugin ecosystem

## Resources & Dependencies

### Current Dependencies (v2.0.0)
- **LovyanGFX** v1.1.0 - Display driver
- **ArduinoJson** v6.21.0 - JSON parsing
- **FT6X36** - Touch controller
- **ESP32 WiFi** - WiFi library (built-in)
- **Preferences** - NVRAM storage (built-in)
- **HTTPClient** - HTTPS/TLS client (built-in)
- **SD** - SD card library (built-in)

### External Services
- **mempool.space API** - Bitcoin data (FREE)
- **Gemini API** - AI analysis (requires key)

### Future Dependencies
- **NTPClient** - Time synchronization (v2.1.0)
- **UniversalTelegramBot** - Telegram integration (v2.1.0)
- **WiFiManager** - Captive portal (v2.2.0)
- **ArduinoOTA** - OTA updates (v2.2.0)
- **PubSubClient** - MQTT client (v2.2.0)

## Project Metrics (v2.0.0)

### Performance
- **RAM:** 16.3% (53,404 bytes / 320 KB)
- **Flash:** 16.5% (1,078,861 bytes / 16 MB)
- **Scroll FPS:** 110-120 FPS
- **Touch Response:** <50ms
- **Boot Time:** 2-5 seconds

### Codebase
- **Source Files:** MainScreen, API clients, utilities
- **Documentation:** 20+ markdown files
- **Screens:** 1 unified MainScreen (8 cards)
- **API Integrations:** 2 (mempool.space, Gemini)
- **Commands:** 15+ serial commands

### Development
- **Multi-Agent System:** 7 windows, 5 specialized agents
- **Communication:** Inter-agent messaging
- **Makefile:** 25+ commands
- **Documentation:** Complete guides and specs

## Long-term Vision

### Goals
- Reference Bitcoin dashboard for ESP32 platforms
- Active community of contributors
- Plugin ecosystem for custom features
- Educational resource for embedded development
- 120 FPS smoothness as industry standard

### Success Metrics
- ✅ 120 FPS scrolling achieved
- ✅ AI integration working
- ✅ Multi-agent system operational
- ✅ Comprehensive documentation
- ⏳ 1000+ GitHub stars
- ⏳ 50+ community contributions
- ⏳ Active Discord/Telegram community
- ⏳ Regular releases

## Notes

- Performance is paramount - maintain 120 FPS standard
- User experience drives design decisions
- Documentation must stay current
- Community feedback shapes roadmap
- Open-source principles maintained
- Security and stability over features

---

**Last Updated:** 2025-11-29
**Version:** 2.0.0 (MainScreen with 120 FPS Scrolling)
**Status:** Production-Ready
