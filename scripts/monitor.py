#!/usr/bin/env python3
"""Simple serial monitor for Bitcoin Dashboard"""

import serial
import sys
import time

PORT = '/dev/cu.usbmodem1101'
BAUD = 115200

def monitor(duration=10):
    """Monitor serial output for specified duration"""
    try:
        ser = serial.Serial(PORT, BAUD, timeout=1)
        print(f"Monitoring {PORT} at {BAUD} baud for {duration} seconds...")
        print("=" * 60)

        start = time.time()
        while time.time() - start < duration:
            if ser.in_waiting:
                line = ser.readline().decode('utf-8', errors='ignore').rstrip()
                if line:
                    print(line)

        ser.close()
        print("=" * 60)

    except serial.SerialException as e:
        print(f"Error: {e}")
        sys.exit(1)
    except KeyboardInterrupt:
        print("\nInterrupted by user")
        sys.exit(0)

if __name__ == "__main__":
    duration = int(sys.argv[1]) if len(sys.argv) > 1 else 10
    monitor(duration)
