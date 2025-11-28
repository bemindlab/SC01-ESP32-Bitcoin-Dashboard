#!/usr/bin/env python3
"""
Screenshot Capture Tool for SC01 Plus Bitcoin Dashboard

Captures display buffer from device via serial and converts to image format.
Display: 480x320 RGB565 (16-bit color)
"""

import sys
import serial
import struct
import time
from pathlib import Path
from datetime import datetime

# Display configuration
WIDTH = 480
HEIGHT = 320
BYTES_PER_PIXEL = 2  # RGB565 format
BUFFER_SIZE = WIDTH * HEIGHT * BYTES_PER_PIXEL

def rgb565_to_rgb888(rgb565):
    """Convert RGB565 (16-bit) to RGB888 (24-bit)."""
    r5 = (rgb565 >> 11) & 0x1F
    g6 = (rgb565 >> 5) & 0x3F
    b5 = rgb565 & 0x1F

    # Scale to 8-bit
    r8 = (r5 * 255) // 31
    g8 = (g6 * 255) // 63
    b8 = (b5 * 255) // 31

    return (r8, g8, b8)

def capture_screenshot(port, baudrate=115200, timeout=30):
    """
    Capture screenshot from device.

    Args:
        port: Serial port (e.g., /dev/cu.usbmodem1101)
        baudrate: Serial baud rate
        timeout: Timeout in seconds

    Returns:
        bytes: Raw RGB565 buffer data
    """
    print(f"Connecting to {port} at {baudrate} baud...")

    try:
        ser = serial.Serial(port, baudrate, timeout=timeout)
        time.sleep(2)  # Wait for connection

        # Clear any existing data
        ser.reset_input_buffer()

        # Send screenshot command
        print("Sending screenshot command...")
        ser.write(b'SCREENSHOT\n')
        ser.flush()

        # Wait for response
        print("Waiting for device response...")
        time.sleep(1)

        # Look for start marker
        start_marker = b'SCREENSHOT_START'
        end_marker = b'SCREENSHOT_END'

        # Read until we find start marker
        buffer = bytearray()
        found_start = False

        print("Looking for start marker...")
        while not found_start and len(buffer) < 10000:
            chunk = ser.read(100)
            if not chunk:
                break
            buffer.extend(chunk)
            if start_marker in buffer:
                found_start = True
                # Remove everything before start marker
                idx = buffer.index(start_marker) + len(start_marker)
                buffer = buffer[idx:]
                print("Found start marker!")
                break

        if not found_start:
            print("Error: Start marker not found")
            print(f"Received: {buffer[:200]}")
            return None

        # Read the pixel data
        print(f"Reading {BUFFER_SIZE} bytes of pixel data...")
        pixel_data = bytearray()

        while len(pixel_data) < BUFFER_SIZE:
            remaining = BUFFER_SIZE - len(pixel_data)
            chunk_size = min(1024, remaining)
            chunk = ser.read(chunk_size)

            if not chunk:
                print(f"Timeout: Only received {len(pixel_data)}/{BUFFER_SIZE} bytes")
                break

            pixel_data.extend(chunk)

            # Progress indicator
            progress = (len(pixel_data) / BUFFER_SIZE) * 100
            print(f"Progress: {progress:.1f}%", end='\r')

        print(f"\nReceived {len(pixel_data)} bytes")

        # Look for end marker
        if end_marker in pixel_data[-50:]:
            print("Found end marker!")
            # Remove end marker
            idx = pixel_data.index(end_marker)
            pixel_data = pixel_data[:idx]

        ser.close()

        return bytes(pixel_data)

    except serial.SerialException as e:
        print(f"Serial error: {e}")
        return None
    except Exception as e:
        print(f"Error: {e}")
        return None

def save_raw(data, output_path):
    """Save raw RGB565 data."""
    with open(output_path, 'wb') as f:
        f.write(data)
    print(f"Saved raw RGB565 data to: {output_path}")

def save_ppm(data, output_path):
    """
    Save as PPM (Portable Pixmap) format.
    Simple RGB format that can be viewed with many tools.
    """
    if len(data) != BUFFER_SIZE:
        print(f"Warning: Expected {BUFFER_SIZE} bytes, got {len(data)}")
        return False

    with open(output_path, 'wb') as f:
        # PPM header
        header = f"P6\n{WIDTH} {HEIGHT}\n255\n"
        f.write(header.encode('ascii'))

        # Convert RGB565 to RGB888
        for i in range(0, len(data), 2):
            if i + 1 < len(data):
                rgb565 = struct.unpack('<H', data[i:i+2])[0]
                r, g, b = rgb565_to_rgb888(rgb565)
                f.write(bytes([r, g, b]))

    print(f"Saved PPM image to: {output_path}")
    return True

def main():
    if len(sys.argv) < 2:
        print("Usage: python3 capture_screenshot.py <serial_port>")
        print("Example: python3 capture_screenshot.py /dev/cu.usbmodem1101")
        sys.exit(1)

    port = sys.argv[1]

    # Create output directory
    output_dir = Path(__file__).parent.parent / '.tmp'
    output_dir.mkdir(parents=True, exist_ok=True)

    # Generate timestamp
    timestamp = datetime.now().strftime('%Y%m%d_%H%M%S')

    # Capture screenshot
    print("=" * 60)
    print("SC01 Plus Screenshot Capture Tool")
    print("=" * 60)

    data = capture_screenshot(port)

    if data:
        # Save raw format
        raw_path = output_dir / f'screenshot_{timestamp}.raw'
        save_raw(data, raw_path)

        # Save PPM format
        ppm_path = output_dir / f'screenshot_{timestamp}.ppm'
        if save_ppm(data, ppm_path):
            print("\nScreenshot captured successfully!")
            print(f"\nFiles saved:")
            print(f"  Raw (RGB565): {raw_path}")
            print(f"  Image (PPM):  {ppm_path}")
            print(f"\nTo view PPM: open {ppm_path}")
            print("To convert to PNG: convert screenshot.ppm screenshot.png")
    else:
        print("\nScreenshot capture failed!")
        sys.exit(1)

if __name__ == '__main__':
    main()
