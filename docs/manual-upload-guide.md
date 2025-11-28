# Manual Upload Guide for SC01 Plus

When automatic uploads fail with "Failed to connect to ESP32-S3", you need to manually put the device into bootloader mode.

## Quick Method

### **You MUST hold the BOOT button before running the upload command:**

1. **Locate the BOOT button** on your SC01 Plus (usually near the USB port)

2. **Press and HOLD the BOOT button**

3. **While holding BOOT, run:**
   ```bash
   make upload
   # OR
   python3 -m platformio run --target upload
   ```

4. **Watch for "Connecting..." to change to "Writing..."**

5. **Release BOOT button** once you see "Writing at 0x00010000..."

6. **Wait for upload to complete**

## Step-by-Step Visual Guide

```
┌─────────────────────────────────────┐
│  SC01 Plus Board                    │
│                                     │
│  ┌──────┐                           │
│  │ USB  │  ← USB Port               │
│  └──────┘                           │
│                                     │
│  [BOOT]  ← Press and HOLD this     │
│  [RESET] ← Optional                 │
│                                     │
│  ┌──────────────┐                  │
│  │   Display    │                  │
│  │              │                  │
│  └──────────────┘                  │
└─────────────────────────────────────┘
```

## Alternative Method: Reset Sequence

If holding BOOT doesn't work:

1. **Disconnect USB cable**
2. **Hold BOOT button**
3. **While holding BOOT, reconnect USB**
4. **Keep holding BOOT**
5. **Run upload command**
6. **Release BOOT when upload starts**

## Alternative Method 2: BOOT + RESET

1. **Hold BOOT button**
2. **Press and release RESET button (while still holding BOOT)**
3. **Release BOOT button**
4. **Immediately run upload command**

## Why This Happens

The ESP32-S3 on SC01 Plus uses USB-JTAG/Serial which doesn't support automatic DTR/RTS reset like traditional ESP32 boards. The BOOT button manually puts the chip into bootloader mode.

## Verification

After successful upload, you should see:

```
Writing at 0x00010000... (2 %)
Writing at 0x00017d86... (4 %)
...
Writing at 0x00126514... (100 %)
Wrote 1148976 bytes (743266 compressed)...
Hash of data verified.

Leaving...
Hard resetting via RTS pin...
========================= [SUCCESS] =========================
```

## Troubleshooting

### "Port is busy"
```bash
# Find and kill the process
lsof /dev/cu.usbmodem1101
kill <PID>

# Or disconnect/reconnect USB
```

### "Port doesn't exist"
```bash
# List available ports
python3 -m platformio device list

# Device should show as:
# /dev/cu.usbmodem1101
# Hardware ID: USB VID:PID=303A:1001
```

### Still won't connect

1. **Try a different USB cable** (must support data, not charge-only)
2. **Try a different USB port** on your computer
3. **Check device is powered** (screen should light up)
4. **Try the reset sequence** above

## Quick Reference

| Command | Description |
|---------|-------------|
| `make upload` | Upload with BOOT button |
| `make monitor` | View serial output |
| `make flash` | Upload + monitor |
| `make devices` | List connected devices |

## After Upload Success

The device will automatically reset and start running the new firmware with updated WiFi credentials.

To verify it's working:
```bash
make monitor
```

You should see:
```
Bitcoin Dashboard Starting...
WiFi Connected!
192.168.x.x
```

Press `Ctrl+C` to exit the monitor.
