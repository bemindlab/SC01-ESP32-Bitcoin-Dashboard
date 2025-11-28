#!/usr/bin/env python3
"""
Send API keys to ESP32 device via serial
"""

import serial
import time
import sys

def send_keys(port, keys_file):
    """Send API keys from file to device via serial"""

    # Read keys from file
    keys = {}
    try:
        with open(keys_file, 'r') as f:
            for line in f:
                line = line.strip()
                if '=' in line:
                    key, value = line.split('=', 1)
                    keys[key] = value
    except FileNotFoundError:
        print(f"Error: {keys_file} not found!")
        return False

    if not keys:
        print(f"Error: No keys found in {keys_file}")
        return False

    print(f"Found {len(keys)} keys to send")

    # Open serial connection
    try:
        ser = serial.Serial(port, 115200, timeout=2)
        print(f"Connected to {port}")
        time.sleep(3)  # Wait for device to boot

        # Send each key
        for key_name, key_value in keys.items():
            command = f"SET_{key_name}={key_value}\n"
            print(f"Sending: SET_{key_name}=***{key_value[-8:]}")

            ser.write(command.encode())
            time.sleep(0.5)

            # Read response
            response = ser.read(ser.in_waiting).decode('utf-8', errors='ignore')
            if response:
                print(f"Device response: {response.strip()}")

        ser.close()
        print("\n✓ All keys sent successfully!")
        return True

    except serial.SerialException as e:
        print(f"Error: Could not open serial port {port}")
        print(f"  {e}")
        return False
    except Exception as e:
        print(f"Error sending keys: {e}")
        return False

if __name__ == "__main__":
    if len(sys.argv) < 2:
        print("Usage: python3 send_keys.py <port> [keys_file]")
        print("Example: python3 send_keys.py /dev/cu.usbmodem1101")
        sys.exit(1)

    port = sys.argv[1]
    keys_file = sys.argv[2] if len(sys.argv) > 2 else '.tmp/keys.txt'

    success = send_keys(port, keys_file)
    sys.exit(0 if success else 1)
