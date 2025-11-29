#!/usr/bin/env python3
import serial
import time
import sys

# Open serial port
try:
    ser = serial.Serial('/dev/cu.usbmodem101', 115200, timeout=1)
    print("Connected to serial port. Reading output...")
    print("=" * 80)

    # Read for 30 seconds or until Ctrl+C
    start_time = time.time()
    while time.time() - start_time < 30:
        if ser.in_waiting > 0:
            line = ser.readline().decode('utf-8', errors='ignore').strip()
            if line:
                print(line)
                sys.stdout.flush()
        time.sleep(0.01)

    ser.close()

except KeyboardInterrupt:
    print("\nStopped by user")
    ser.close()
except Exception as e:
    print(f"Error: {e}")
