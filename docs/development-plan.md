# Development Plan

Bitcoin Dashboard for Panlee SC01 Plus - Strategic development roadmap and feature planning.

## Project Status

**Current Version:** 1.0.0-alpha
**Status:** Initial Release
**Last Updated:** 2025-11-28

### Completed Features ✓

- [x] Core dashboard functionality
- [x] Real-time BTC price display (USD/EUR)
- [x] Latest block information
- [x] Mempool statistics
- [x] Fee rate recommendations (Fast/Medium/Slow)
- [x] WiFi connectivity
- [x] LVGL dark theme UI
- [x] Four-card responsive layout
- [x] Touch screen support
- [x] Serial debugging capabilities
- [x] PlatformIO build configuration
- [x] Documentation and debugging guides

## Development Phases

### Phase 1: Core Stability & Testing (Current)

**Goals:**
- Verify hardware compatibility across different SC01 Plus boards
- Ensure stable WiFi connectivity
- Validate API rate limiting behavior
- Test long-running stability (24+ hours)
- Document edge cases and error scenarios

**Tasks:**
- [ ] Test on multiple SC01 Plus units
- [ ] Monitor memory usage over 48 hours
- [ ] Verify all API endpoints remain stable
- [ ] Document common hardware issues
- [ ] Create hardware compatibility matrix
- [ ] Add watchdog timer for crash recovery
- [ ] Implement millis() rollover handling

**Success Criteria:**
- Runs continuously for 48+ hours without crashes
- Memory remains stable (no leaks)
- All API calls successful >95% of the time
- Recovery from network interruptions

### Phase 2: Enhanced UI/UX

**Goals:**
- Improve visual design and readability
- Add interactive touch features
- Implement data visualizations
- Enhance user feedback

**Features to Implement:**
- [ ] Price change indicators (↑↓ arrows, % change)
- [ ] Historical price chart (last 24h)
- [ ] Block time countdown timer
- [ ] Fee rate color coding (green/yellow/red)
- [ ] Touch-to-refresh functionality
- [ ] Settings page (via touch menu)
- [ ] Brightness control
- [ ] Display sleep mode
- [ ] Custom update interval selection
- [ ] Network status icon

**Technical Requirements:**
- LVGL charts and graphs
- Touch gesture detection
- Non-volatile settings storage (SPIFFS/Preferences)
- Additional LVGL widgets

### Phase 3: Extended Bitcoin Metrics

**Goals:**
- Display additional Bitcoin network data
- Provide deeper blockchain insights
- Add Lightning Network statistics

**Features to Implement:**
- [ ] Difficulty adjustment countdown
- [ ] Hashrate display
- [ ] Next difficulty estimate
- [ ] Lightning Network stats (nodes, channels, capacity)
- [ ] Bitcoin dominance percentage
- [ ] Market cap display
- [ ] 24h high/low prices
- [ ] Transaction volume statistics
- [ ] Mempool fee distribution histogram

**API Endpoints to Add:**
- `/api/v1/difficulty-adjustment`
- `/api/v1/lightning/statistics/latest`
- `/api/v1/mining/hashrate/pools/1w`
- Additional price APIs for dominance/market cap

### Phase 4: Connectivity & Configuration

**Goals:**
- Improve WiFi management
- Add configuration options
- Enable remote monitoring

**Features to Implement:**
- [ ] WiFi manager (captive portal for setup)
- [ ] Multiple WiFi network support
- [ ] Auto-reconnect with exponential backoff
- [ ] NTP time synchronization
- [ ] Timezone selection
- [ ] OTA (Over-The-Air) firmware updates
- [ ] Web interface for configuration
- [ ] MQTT publishing for monitoring
- [ ] REST API for status queries
- [ ] Bluetooth configuration option

**Technical Requirements:**
- WiFiManager library
- NTP client
- AsyncWebServer
- ArduinoOTA
- MQTT client library

### Phase 5: Advanced Features

**Goals:**
- Add personalized features
- Enable alerts and notifications
- Support multiple cryptocurrencies

**Features to Implement:**
- [ ] Price alerts (push notifications via Telegram/email)
- [ ] Custom address monitoring
- [ ] Transaction notifications
- [ ] Multi-currency support (ETH, ADA, etc.)
- [ ] Portfolio tracker
- [ ] DCA (Dollar Cost Average) calculator
- [ ] Sats/USD conversion toggle
- [ ] Custom theme colors
- [ ] Multiple dashboard pages (swipe left/right)
- [ ] QR code display for receive addresses

**Technical Requirements:**
- Telegram Bot API integration
- Additional crypto APIs (CoinGecko, etc.)
- QR code generation library
- Multi-page LVGL navigation

### Phase 6: Optimization & Performance

**Goals:**
- Reduce memory footprint
- Optimize rendering performance
- Improve power efficiency

**Tasks:**
- [ ] Profile memory usage and optimize
- [ ] Implement double buffering if memory allows
- [ ] Optimize JSON parsing (use filters)
- [ ] Reduce API call payload sizes
- [ ] Implement deep sleep during inactive periods
- [ ] Optimize LVGL rendering
- [ ] Reduce binary size
- [ ] Battery operation support (if hardware allows)

## Technical Debt & Improvements

### High Priority
- [ ] Add millis() rollover handling (49-day bug)
- [ ] Implement proper error recovery for API failures
- [ ] Add watchdog timer for automatic crash recovery
- [ ] Validate all JSON responses before parsing
- [ ] Add certificate validation for HTTPS

### Medium Priority
- [ ] Refactor code into separate files (ui.cpp, api.cpp, etc.)
- [ ] Create configuration header file
- [ ] Add unit tests for JSON parsing
- [ ] Implement state machine for WiFi management
- [ ] Add logging levels (debug/info/error)

### Low Priority
- [ ] Add compile-time feature flags
- [ ] Create multiple build configurations
- [ ] Add CI/CD pipeline
- [ ] Create automated hardware testing framework

## Hardware Considerations

### Tested Hardware
- Panlee SC01 Plus (ESP32-S3, 16MB Flash, OPI PSRAM)

### Future Hardware Support
- [ ] SC01 (non-Plus version)
- [ ] Other ESP32-S3 displays (CYD, etc.)
- [ ] ESP32-S2 compatibility
- [ ] Larger displays (4.3", 7")
- [ ] E-ink displays for low power

## Community & Contributions

### Documentation Needs
- [ ] Hardware assembly guide with photos
- [ ] Video tutorial for setup
- [ ] Troubleshooting FAQ
- [ ] API rate limiting guidelines
- [ ] Performance benchmarks

### Community Features
- [ ] Example configurations
- [ ] Custom theme gallery
- [ ] User-submitted improvements
- [ ] Translation support (i18n)
- [ ] Community API endpoints

## Security Considerations

### Current Status
- WiFi credentials hardcoded (security risk)
- No certificate validation
- No encrypted storage

### Improvements Needed
- [ ] Move credentials to secure storage
- [ ] Implement HTTPS certificate validation
- [ ] Add encrypted preferences storage
- [ ] Secure boot implementation
- [ ] Rate limiting for API calls
- [ ] Input validation for configuration

## Testing Strategy

### Manual Testing
- Hardware compatibility testing
- UI/UX testing on actual device
- Network failure scenarios
- Long-running stability tests

### Automated Testing
- [ ] JSON parsing unit tests
- [ ] API response validation tests
- [ ] Memory leak detection
- [ ] Performance benchmarking

### Integration Testing
- [ ] WiFi connectivity tests
- [ ] API integration tests
- [ ] Display rendering tests
- [ ] Touch input tests

## Release Strategy

### Version Numbering
- Major.Minor.Patch (Semantic Versioning)
- Current: 1.0.0-alpha

### Release Process
1. Feature development on feature branches
2. Testing on `develop` branch
3. Release candidates for community testing
4. Merge to `main` for stable releases
5. Tag releases with version numbers
6. Create GitHub releases with binaries

### Milestone Releases
- **v1.0.0** - Stable core functionality
- **v1.1.0** - Enhanced UI/UX
- **v1.2.0** - Extended metrics
- **v2.0.0** - WiFi manager & OTA
- **v2.1.0** - Advanced features
- **v3.0.0** - Multi-currency support

## Resources & Dependencies

### Current Dependencies
- LVGL v8.3.0
- TFT_eSPI v2.5.0
- ArduinoJson v6.21.0
- FT6X36 (touch driver)

### Potential Future Dependencies
- WiFiManager
- ArduinoOTA
- PubSubClient (MQTT)
- ESPAsyncWebServer
- NTPClient
- QRCode
- TelegramBot

### External Services
- mempool.space API (primary)
- Fallback APIs (if needed)
- NTP servers
- OTA update server

## Contribution Guidelines

### Code Standards
- Follow existing code style
- Comment complex logic
- Use meaningful variable names
- Keep functions focused and small
- Test on actual hardware

### Pull Request Process
1. Fork the repository
2. Create feature branch
3. Implement changes with tests
4. Update documentation
5. Submit PR with description
6. Address review comments

### Areas for Contribution
- New API integrations
- UI/UX improvements
- Bug fixes
- Documentation
- Testing
- Translations
- Hardware compatibility

## Long-term Vision

### Goals
- Become the reference Bitcoin dashboard for ESP32
- Support multiple hardware platforms
- Active community contributions
- Regular updates with new features
- Educational resource for embedded development

### Success Metrics
- 1000+ GitHub stars
- Active community contributions
- Regular releases (monthly)
- Comprehensive documentation
- Hardware compatibility across 5+ devices

## Notes

- All features should maintain backwards compatibility where possible
- Performance must not degrade with new features
- Documentation must be updated with each feature
- Community feedback drives priority
- Security and stability over features
