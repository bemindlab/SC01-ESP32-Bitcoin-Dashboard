#!/usr/bin/env python3
"""
Debug Screen Capture Tool - Captures all screens automatically

Works with the DEBUG_SCREENS command to capture all screen layouts.
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

def save_ppm(data, output_path):
    """Save as PPM (Portable Pixmap) format."""
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

    return True

def capture_one_screenshot(ser, screen_name, output_dir, timestamp):
    """Capture a single screenshot."""
    print(f"\n--- Capturing {screen_name} ---")

    # Look for start marker
    start_marker = b'SCREENSHOT_START'
    end_marker = b'SCREENSHOT_END'

    # Read until we find start marker
    buffer = bytearray()
    found_start = False

    start_time = time.time()
    while not found_start and (time.time() - start_time) < 10:
        if ser.in_waiting > 0:
            chunk = ser.read(min(ser.in_waiting, 1000))
            buffer.extend(chunk)
            if start_marker in buffer:
                found_start = True
                # Find and skip newline after marker
                idx = buffer.index(start_marker) + len(start_marker)
                # Skip any newlines
                while idx < len(buffer) and buffer[idx] in (ord('\n'), ord('\r')):
                    idx += 1
                buffer = buffer[idx:]
                break
        time.sleep(0.01)

    if not found_start:
        print(f"Error: Start marker not found for {screen_name}")
        return None

    # Read pixel data
    pixel_data = bytearray(buffer)

    while len(pixel_data) < BUFFER_SIZE:
        remaining = BUFFER_SIZE - len(pixel_data)
        chunk_size = min(2048, remaining)

        chunk = ser.read(chunk_size)
        if not chunk:
            print(f"Timeout: Only received {len(pixel_data)}/{BUFFER_SIZE} bytes")
            break

        pixel_data.extend(chunk)

        # Progress
        progress = (len(pixel_data) / BUFFER_SIZE) * 100
        print(f"Progress: {progress:.1f}%", end='\r')

    print(f"\nReceived {len(pixel_data)} bytes")

    # Trim to exact size
    pixel_data = pixel_data[:BUFFER_SIZE]

    # Save as PPM
    filename = f'{timestamp}_{screen_name.lower().replace(" ", "_")}.ppm'
    output_path = output_dir / filename

    if save_ppm(pixel_data, output_path):
        print(f"✓ Saved: {output_path}")
        return output_path

    return None

def main():
    if len(sys.argv) < 2:
        print("Usage: python3 capture_debug_screens.py <serial_port>")
        print("Example: python3 capture_debug_screens.py /dev/cu.usbmodem101")
        sys.exit(1)

    port = sys.argv[1]

    # Create output directory
    output_dir = Path(__file__).parent.parent / '.tmp'
    output_dir.mkdir(parents=True, exist_ok=True)

    # Generate timestamp
    timestamp = datetime.now().strftime('%Y%m%d_%H%M%S')

    print("=" * 60)
    print("SC01 Plus Debug Screen Capture Tool")
    print("=" * 60)
    print(f"Port: {port}")
    print(f"Output: {output_dir}")
    print("=" * 60)

    try:
        # Connect to serial
        print(f"\nConnecting to {port}...")
        ser = serial.Serial(port, 115200, timeout=2)
        time.sleep(2)

        # Clear buffer
        ser.reset_input_buffer()

        # Send DEBUG_SCREENS command
        print("Sending DEBUG_SCREENS command...")
        ser.write(b'DEBUG_SCREENS\n')
        ser.flush()

        # Expected screens in order
        screens = ['Dashboard', 'News', 'Trading', 'Settings', 'Bitcoin Clock']
        captured = []

        # Capture each screen
        for screen_name in screens:
            result = capture_one_screenshot(ser, screen_name, output_dir, timestamp)
            if result:
                captured.append(result)

        ser.close()

        # Summary
        print("\n" + "=" * 60)
        print("CAPTURE COMPLETE")
        print("=" * 60)
        print(f"\nCaptured {len(captured)}/{len(screens)} screens:")
        for path in captured:
            print(f"  ✓ {path.name}")

        print(f"\nAll files saved to: {output_dir}")
        print("\nTo view: open <filename>.ppm")
        print("To convert: convert <filename>.ppm <filename>.png")

    except serial.SerialException as e:
        print(f"Serial error: {e}")
        sys.exit(1)
    except Exception as e:
        print(f"Error: {e}")
        import traceback
        traceback.print_exc()
        sys.exit(1)

if __name__ == '__main__':
    main()
