# Scripts Directory

Automation scripts for the Bitcoin Dashboard project.

## Available Scripts

### 📸 capture_screenshot.py

Captures screenshots from the SC01 Plus device via serial connection.

**Features:**
- Captures 480x320 RGB565 display buffer
- Converts to viewable PPM image format
- Saves both raw and image formats
- Progress indicator during capture
- Automatic timestamping

**Usage:**

```bash
# Using Makefile (recommended)
make screenshot

# Direct usage
python3 scripts/capture_screenshot.py /dev/cu.usbmodem1101

# Interactive mode (manual commands)
make screenshot-interactive
```

**Requirements:**
- Python 3.x
- pyserial library: `pip3 install pyserial`
- Firmware with screenshot support (already implemented in main.cpp)
- Device connected via USB

**Output Files:**
- `.tmp/screenshot_YYYYMMDD_HHMMSS.raw` - Raw RGB565 data
- `.tmp/screenshot_YYYYMMDD_HHMMSS.ppm` - PPM image (viewable)

**View Screenshots:**
```bash
# Open PPM directly (macOS)
open .tmp/screenshot_20251128_153000.ppm

# Convert to PNG (requires ImageMagick)
convert .tmp/screenshot_20251128_153000.ppm .tmp/screenshot.png
```

**Serial Commands:**
The device accepts these commands via serial:
- `SCREENSHOT` - Capture display buffer and send via serial
- `STATUS` - Show device status (WiFi, memory, uptime)
- `HELP` - Show available commands

## How Screenshot Works

1. **Device Side (main.cpp):**
   - Listens for "SCREENSHOT" command on serial
   - Reads display framebuffer (480x320 RGB565)
   - Sends binary data via serial with markers
   - Format: `SCREENSHOT_START` + pixel data + `SCREENSHOT_END`

2. **Script Side (capture_screenshot.py):**
   - Sends "SCREENSHOT\n" command
   - Waits for start marker
   - Reads 307,200 bytes of pixel data
   - Converts RGB565 to RGB888 for PPM
   - Saves both raw and image formats

## Display Format

**Specifications:**
- Resolution: 480x320 pixels (landscape)
- Color Format: RGB565 (16-bit)
- Byte Order: Little-endian
- Total Size: 307,200 bytes (480 × 320 × 2)

**RGB565 Format:**
```
15 14 13 12 11 | 10 09 08 07 06 05 | 04 03 02 01 00
    Red (5)    |    Green (6)      |    Blue (5)
```

## Troubleshooting

**Problem: Script hangs waiting for start marker**
- Solution: Make sure firmware is uploaded with screenshot support
- Check: Device should respond to serial commands
- Verify: Open serial monitor and type "HELP" - should show commands

**Problem: Timeout during capture**
- Solution: Increase timeout in script (default 30s)
- Check: USB cable quality and connection
- Verify: Serial monitor shows device output

**Problem: Partial data received**
- Solution: Try slower baud rate or add delays
- Check: Serial buffer overrun messages
- Verify: Device has enough free heap memory

**Problem: PPM file won't open**
- Solution: Check file size (should be ~921,600 bytes)
- Verify: File contains PPM header (P6 480 320 255)
- Try: Convert to PNG using ImageMagick

## Example Session

```bash
$ make screenshot
Capturing device screenshot...
Make sure firmware is uploaded and device is running!

SC01 Plus Screenshot Capture Tool
============================================================
Connecting to /dev/cu.usbmodem1101 at 115200 baud...
Sending screenshot command...
Waiting for device response...
Looking for start marker...
Found start marker!
Reading 307200 bytes of pixel data...
Progress: 100.0%
Received 307200 bytes
Found end marker!
Saved raw RGB565 data to: .tmp/screenshot_20251128_153027.raw
Saved PPM image to: .tmp/screenshot_20251128_153027.ppm

Screenshot captured successfully!

Files saved:
  Raw (RGB565): .tmp/screenshot_20251128_153027.raw
  Image (PPM):  .tmp/screenshot_20251128_153027.ppm

To view PPM: open .tmp/screenshot_20251128_153027.ppm
To convert to PNG: convert screenshot.ppm screenshot.png
```

## Future Enhancements

- [ ] Add PNG conversion (requires PIL/Pillow)
- [ ] Support for different resolutions
- [ ] Compression for faster transfer
- [ ] Screenshot via hardware button press
- [ ] Automatic screenshot on error/crash
- [ ] Screenshot comparison tool
- [ ] Batch screenshot capture
- [ ] Real-time display streaming

## Related Documentation

- [Main README](../README.md) - Project overview
- [CLAUDE.md](../CLAUDE.md) - Development guidelines
- [Debugging Guide](../docs/guides/debugging-guide.md) - Serial debugging
