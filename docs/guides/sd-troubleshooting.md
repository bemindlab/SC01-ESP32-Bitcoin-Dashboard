# SD Card Troubleshooting Guide

## SD Card Not Detected

If `make sd-status` shows "Not initialized or unavailable", follow these steps:

### Step 1: Verify SD Card is Inserted

The Panlee SC01 Plus has an SD card slot on the back of the board. Make sure:
- SD card is fully inserted (you should hear/feel a click)
- Card is oriented correctly (contacts facing the PCB)
- Card is not damaged

### Step 2: Check SD Card Format

The ESP32-S3 SD library requires **FAT32** format:

**On macOS:**
```bash
# Find SD card device (usually /dev/disk2 or /dev/disk3)
diskutil list

# Format as FAT32
diskutil eraseDisk FAT32 SDCARD MBRFormat /dev/diskX
```

**On Windows:**
- Right-click SD card in File Explorer
- Choose "Format"
- File system: FAT32 (MS-DOS)
- Allocation unit size: Default
- Click "Start"

**On Linux:**
```bash
# Find SD card device
lsblk

# Format as FAT32
sudo mkfs.vfat -F 32 /dev/sdX1
```

### Step 3: Try Different SD Card

Some SD cards are incompatible:
- ✅ **Recommended:** SanDisk, Kingston, Samsung (4GB-32GB)
- ⚠️ **May work:** Generic brands (test first)
- ❌ **Not supported:** Cards > 32GB may have issues
- ❌ **Not supported:** SDXC cards (use SDHC instead)

### Step 4: Use Diagnostic Commands

**Check detailed status:**
```bash
make sd-status
```

**Or via serial:**
```
CHECK_SD_CARD    # Shows pin configuration and diagnostics
REINIT_SD        # Try to reinitialize without restart
```

### Step 5: Hardware Check

If still not working, verify hardware:

**Pin Configuration (WT32-SC01 Plus / Panlee SC01 Plus):**
- CS (Chip Select): GPIO 41
- MOSI (Data Out): GPIO 40
- MISO (Data In): GPIO 38
- CLK (Clock): GPIO 39

**Physical inspection:**
- Check SD card slot for debris or damage
- Ensure SD card contacts are clean (use isopropyl alcohol)
- Verify no bent pins in the SD slot

## Common Error Messages

### "SD card initialization failed"
**Cause:** SD card not detected at boot
**Fix:**
1. Insert SD card
2. Press reset button on device
3. Or send `REINIT_SD` command

### "No SD card attached"
**Cause:** Card removed or not making contact
**Fix:**
1. Remove and reinsert SD card firmly
2. Try different SD card
3. Check slot for damage

### "Card type UNKNOWN"
**Cause:** Incompatible or damaged card
**Fix:**
1. Use different SD card brand
2. Format as FAT32
3. Use card ≤ 32GB

## Hot-Swap Support

The Bitcoin Dashboard supports SD card hot-swapping:

```bash
# While device is running:
1. Remove SD card    # Logging automatically stops
2. Insert SD card    # Wait 5 seconds
3. Send: REINIT_SD   # Or restart device
```

The system will:
- Detect card removal within 5 seconds
- Automatically reinitialize when reinserted
- Resume logging after reinitialization

## Working Configuration Example

```
=== SD Card Status ===
Logger Ready: Yes
Logger Enabled: Yes
Card Present: Yes
Status: Ready
Free Space: 14.23 GB
Total Space: 14.85 GB
Log Files: 5
```

## Still Not Working?

If SD card still doesn't work after trying all steps:

1. **Verify hardware version**: Confirm you have WT32-SC01 Plus (not WT32-SC01)
2. **Check serial output at boot**: Look for "SD Card Initialization" messages
3. **Test on computer**: Verify SD card works on your computer
4. **Report issue**: If hardware is confirmed OK, report on GitHub with:
   - SD card brand and size
   - Serial output during boot
   - Output of `CHECK_SD_CARD` command

## Quick Reference

| Command | Description |
|---------|-------------|
| `make sd-status` | Check SD card status |
| `CHECK_SD_CARD` | Detailed diagnostics |
| `REINIT_SD` | Reinitialize SD card |
| `make sd-format` | Format SD card (deletes all data) |

## See Also

- [SD Card Commands Guide](sd-commands.md) - All available SD commands
- [SD Format Guide](sd-format.md) - How to format SD cards properly

---

**Sources:**
- [WT32-SC01 Plus GitHub](https://github.com/Cesarbautista10/WT32-SC01-Plus-ESP32)
- [RIOT-OS WT32-SC01 Plus Documentation](https://doc.riot-os.org/group__boards__esp32s3__wt32__sc01__plus.html)
- [Smart Panlee Product Page](https://www.tinytronics.nl/en/development-boards/microcontroller-boards/with-wi-fi/smart-panlee-sc01-plus-esp32-s3-with-3.5-inch-ips-tft-display-with-touchscreen)
