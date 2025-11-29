#!/usr/bin/env python3
"""
SD Card Format Script
Sends FORMAT_SD_CARD command to Bitcoin Dashboard device via serial.

WARNING: This will DELETE ALL DATA on the SD card!

Usage:
    python3 scripts/sd_format.py [port]

    If port is not specified, will use /dev/cu.usbmodem1101
"""

import serial
import sys
import time

# Default serial port for SC01 Plus
DEFAULT_PORT = '/dev/cu.usbmodem1101'
BAUD_RATE = 115200

def format_sd_card(port):
    """Send FORMAT_SD_CARD command to device"""
    print(f"Connecting to {port} at {BAUD_RATE} baud...")

    try:
        ser = serial.Serial(port, BAUD_RATE, timeout=2)
        time.sleep(2)  # Wait for connection to stabilize

        print("\n⚠️  WARNING: This will DELETE ALL DATA on the SD card!")
        print("Are you sure you want to continue? (yes/no): ", end='')
        confirmation = input().strip().lower()

        if confirmation != 'yes':
            print("✗ Format cancelled")
            ser.close()
            return

        print("\nSending FORMAT_SD_CARD command...")
        ser.write(b'FORMAT_SD_CARD\n')
        ser.flush()

        print("\nDevice response:")
        print("-" * 50)

        # Read response for 10 seconds
        start_time = time.time()
        while time.time() - start_time < 10:
            if ser.in_waiting > 0:
                line = ser.readline().decode('utf-8', errors='ignore').strip()
                if line:
                    print(line)

        print("-" * 50)
        ser.close()
        print("\n✓ Command sent successfully")

    except serial.SerialException as e:
        print(f"✗ Serial error: {e}")
        print(f"\nTroubleshooting:")
        print(f"  1. Check that device is connected")
        print(f"  2. Try different port (use 'make devices' to list)")
        print(f"  3. Close other programs using the serial port")
        sys.exit(1)
    except KeyboardInterrupt:
        print("\n✗ Interrupted by user")
        sys.exit(1)

def main():
    port = sys.argv[1] if len(sys.argv) > 1 else DEFAULT_PORT
    format_sd_card(port)

if __name__ == '__main__':
    main()
