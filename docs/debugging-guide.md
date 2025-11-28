# Debugging Guide

Complete guide for debugging the Bitcoin Dashboard on ESP32-S3.

## Quick Start

### Basic Serial Monitoring

```bash
# Start serial monitor (115200 baud)
pio device monitor

# Monitor with exception decoder (decodes crash stack traces)
pio device monitor --filter esp32_exception_decoder

# Monitor with custom baud rate
pio device monitor -b 115200
```

### List Connected Devices

```bash
# Show all connected serial devices
pio device list
```

## Common Issues

### 1. Display Not Working

**Symptoms:**
- Black screen
- No backlight
- Garbled display

**Debug Steps:**

1. **Check Serial Output**
   ```bash
   pio device monitor
   ```
   Look for initialization messages.

2. **Verify GPIO Pins**
   - All TFT pins are defined in `platformio.ini` build_flags
   - Check lines 26-33 in platformio.ini:
     ```
     -DTFT_MOSI=13
     -DTFT_SCLK=14
     -DTFT_CS=15
     -DTFT_DC=21
     -DTFT_RST=22
     -DTFT_BL=23
     ```

3. **Try Different Rotation**
   Edit `src/main.cpp:303`:
   ```cpp
   tft.setRotation(1);  // Try 0, 1, 2, or 3
   ```

4. **Check PSRAM**
   Serial output should show PSRAM detected:
   ```
   PSRAM found
   ```

**Common Fixes:**
- Verify hardware connections match GPIO pins
- Ensure 3.3V power supply is stable
- Check for loose ribbon cable connections

### 2. WiFi Connection Failed

**Symptoms:**
- "WiFi Failed!" on status bar
- No data updates
- Serial shows connection timeout

**Debug Steps:**

1. **Check Credentials**
   Verify in `src/main.cpp:10-11`:
   ```cpp
   const char* WIFI_SSID = "YOUR_WIFI_SSID";
   const char* WIFI_PASSWORD = "YOUR_WIFI_PASSWORD";
   ```

2. **Monitor Connection Process**
   ```bash
   pio device monitor
   ```
   Look for:
   ```
   WiFi Connected!
   192.168.x.x
   ```

3. **Verify Network Type**
   - ESP32-S3 **only supports 2.4GHz** networks
   - 5GHz WiFi will not work

4. **Check Signal Strength**
   Add to `setup()` after WiFi.begin():
   ```cpp
   Serial.print("RSSI: ");
   Serial.println(WiFi.RSSI());
   ```
   Strong signal: -30 to -67 dBm
   Weak signal: -68 to -80 dBm

**Common Fixes:**
- Use 2.4GHz WiFi network
- Move closer to router
- Check for typos in SSID/password
- Disable MAC filtering on router
- Increase timeout in `src/main.cpp:336`

### 3. Data Not Updating

**Symptoms:**
- Display shows "---" or stale data
- Status shows "Connected" but no updates
- One or more cards not refreshing

**Debug Steps:**

1. **Monitor API Requests**
   Watch serial output for HTTP errors:
   ```bash
   pio device monitor
   ```

2. **Check Internet Connectivity**
   Add debug logging to fetch functions:
   ```cpp
   bool fetchPrice() {
       Serial.println("Fetching price...");
       HTTPClient http;
       http.begin(API_PRICE);
       int httpCode = http.GET();
       Serial.print("HTTP Code: ");
       Serial.println(httpCode);
       // ... rest of code
   }
   ```

3. **Verify API Endpoints**
   Test endpoints manually:
   - https://mempool.space/api/v1/prices
   - https://mempool.space/api/v1/blocks
   - https://mempool.space/api/mempool
   - https://mempool.space/api/v1/fees/recommended

4. **Check for Rate Limiting**
   - Default intervals should avoid rate limits
   - If custom intervals < 10s, may be throttled

**Common Fixes:**
- Verify internet connection is working
- Check if mempool.space is accessible
- Increase update intervals in `src/main.cpp:19-22`
- Restart device to clear stuck state

### 4. Memory Issues

**Symptoms:**
- Random crashes
- Display corruption
- Watchdog timer resets
- Boot loops

**Debug Steps:**

1. **Monitor Heap Memory**
   Add to `loop()`:
   ```cpp
   if (millis() % 10000 == 0) {  // Every 10 seconds
       Serial.print("Free heap: ");
       Serial.println(ESP.getFreeHeap());
       Serial.print("Free PSRAM: ");
       Serial.println(ESP.getFreePsram());
   }
   ```

2. **Check for Memory Leaks**
   - Watch free heap over time
   - Should remain stable, not decreasing
   - Typical free heap: ~200KB+
   - Typical free PSRAM: ~8MB

3. **Analyze Stack Traces**
   ```bash
   pio device monitor --filter esp32_exception_decoder
   ```
   Crashes will show decoded function names

**Common Fixes:**
- Reduce LVGL buffer size in `src/main.cpp:31`
- Use smaller ArduinoJson documents
- Avoid String concatenation in loops
- Clear HTTP objects properly

### 5. Touch Not Working

**Symptoms:**
- No response to touch
- Erratic touch behavior
- Wrong touch coordinates

**Debug Steps:**

1. **Check I2C Connection**
   Touch uses I2C (SDA: GPIO18, SCL: GPIO19)

2. **Add Touch Debugging**
   In `my_touchpad_read()`:
   ```cpp
   void my_touchpad_read(lv_indev_drv_t* indev_driver, lv_indev_data_t* data) {
       if (touch.read()) {
           data->state = LV_INDEV_STATE_PR;
           data->point.x = touch.getX();
           data->point.y = touch.getY();
           Serial.print("Touch: ");
           Serial.print(data->point.x);
           Serial.print(", ");
           Serial.println(data->point.y);
       } else {
           data->state = LV_INDEV_STATE_REL;
       }
   }
   ```

3. **Verify FT6X36 Initialization**
   Check serial for touch init errors

**Common Fixes:**
- Verify I2C pins (SDA=18, SCL=19)
- Check touch panel connection
- Calibrate touch coordinates if needed

## Advanced Debugging

### Exception Decoder

Decode crash stack traces automatically:

```bash
pio device monitor --filter esp32_exception_decoder
```

This will show actual function names instead of addresses.

### Core Dump Analysis

Enable core dumps in `platformio.ini`:
```ini
build_flags =
    -DCORE_DEBUG_LEVEL=5
```

### JTAG Debugging

For hardware-level debugging, use JTAG with ESP-PROG or similar.

## Logging Levels

Add to `platformio.ini` for verbose output:
```ini
build_flags =
    -DCORE_DEBUG_LEVEL=5  ; VERBOSE
    ; Levels: 0=None, 1=Error, 2=Warn, 3=Info, 4=Debug, 5=Verbose
```

## Performance Profiling

### Measure Function Execution Time

```cpp
unsigned long start = micros();
// Your code here
unsigned long duration = micros() - start;
Serial.print("Execution time: ");
Serial.print(duration);
Serial.println(" us");
```

### Monitor Loop Time

```cpp
void loop() {
    static unsigned long lastLoop = 0;
    unsigned long loopTime = millis() - lastLoop;
    if (loopTime > 100) {
        Serial.print("Slow loop: ");
        Serial.print(loopTime);
        Serial.println(" ms");
    }
    lastLoop = millis();

    // Regular loop code
    lv_task_handler();
    // ...
}
```

## Useful Serial Commands

### Check Compile Info

Add to `setup()`:
```cpp
Serial.println("=== Bitcoin Dashboard ===");
Serial.print("Compile Date: ");
Serial.println(__DATE__);
Serial.print("Compile Time: ");
Serial.println(__TIME__);
Serial.print("SDK Version: ");
Serial.println(ESP.getSdkVersion());
```

### Network Diagnostics

```cpp
Serial.print("IP Address: ");
Serial.println(WiFi.localIP());
Serial.print("MAC Address: ");
Serial.println(WiFi.macAddress());
Serial.print("Gateway: ");
Serial.println(WiFi.gatewayIP());
Serial.print("DNS: ");
Serial.println(WiFi.dnsIP());
```

## Getting Help

1. **Check Serial Output First** - Most issues show errors in serial monitor
2. **Search for HTTP Error Codes** - 404, 429, 500, etc.
3. **Test API Endpoints** - Use browser or curl to verify
4. **Check GitHub Issues** - Common problems may be documented
5. **Hardware Verification** - Use multimeter to check connections

## Debug Checklist

- [ ] Serial monitor running at 115200 baud
- [ ] WiFi credentials correct and network is 2.4GHz
- [ ] All GPIO pins match hardware connections
- [ ] API endpoints accessible from browser
- [ ] Free heap memory staying stable
- [ ] Display rotation set correctly
- [ ] PlatformIO libraries up to date
- [ ] USB cable supports data (not charge-only)
