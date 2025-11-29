#!/usr/bin/env python3
"""
Send serial command to Bitcoin Dashboard device
Usage: python3 send_serial_command.py <port> <command>
Example: python3 send_serial_command.py /dev/cu.usbmodem1101 "STATUS"
"""

import sys
import serial
import time

def send_command(port, command):
    """Send a command to the device and display the response"""
    try:
        # Open serial connection
        ser = serial.Serial(port, 115200, timeout=1)
        time.sleep(0.5)  # Wait for connection

        # Send command
        print(f"Sending command: {command}")
        ser.write(f'{command}\n'.encode('utf-8'))
        ser.flush()
        time.sleep(0.5)

        # Read response (up to 5 seconds)
        print("\n=== Device Response ===")
        start_time = time.time()
        while time.time() - start_time < 5:
            if ser.in_waiting:
                line = ser.readline().decode('utf-8', errors='ignore').strip()
                if line:
                    print(line)
            time.sleep(0.05)

        ser.close()
        print("=== End Response ===\n")

    except serial.SerialException as e:
        print(f"Error: Could not open serial port {port}")
        print(f"Details: {e}")
        print("\nTry:")
        print("  - Check device is connected")
        print("  - Run 'make devices' to list available ports")
        sys.exit(1)
    except Exception as e:
        print(f"Error: {e}")
        sys.exit(1)

if __name__ == "__main__":
    if len(sys.argv) != 3:
        print("Usage: python3 send_serial_command.py <port> <command>")
        print("Example: python3 send_serial_command.py /dev/cu.usbmodem1101 STATUS")
        sys.exit(1)

    port = sys.argv[1]
    command = sys.argv[2]

    send_command(port, command)
