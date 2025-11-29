#!/usr/bin/env python3
"""
Patch API keys and WiFi credentials to ESP32 via serial commands.
Reads configuration from a file and sends appropriate serial commands.
"""

import sys
import time
import serial
import os

def send_command(ser, command, wait_time=1.0):
    """Send a command to the device and wait for response."""
    print(f"Sending: {command}")
    ser.write(f"{command}\n".encode())
    ser.flush()
    time.sleep(wait_time)
    
    # Read response
    response = ""
    while ser.in_waiting > 0:
        response += ser.read(ser.in_waiting).decode('utf-8', errors='ignore')
        time.sleep(0.1)
    
    if response:
        print(response)
    return response

def parse_config_file(filepath):
    """Parse the configuration file and return a dict of settings."""
    config = {}
    
    if not os.path.exists(filepath):
        print(f"Error: Configuration file not found: {filepath}")
        return None
    
    with open(filepath, 'r') as f:
        for line in f:
            line = line.strip()
            
            # Skip comments and empty lines
            if not line or line.startswith('#'):
                continue
            
            # Handle SET_WIFI format
            if line.startswith('SET_WIFI='):
                config['wifi'] = line.split('=', 1)[1]
            
            # Handle GEMINI_KEY format
            elif line.startswith('GEMINI_KEY='):
                config['gemini'] = line.split('=', 1)[1]
            
            # Handle OPENAI_KEY format
            elif line.startswith('OPENAI_KEY='):
                config['openai'] = line.split('=', 1)[1]
    
    return config

def main():
    if len(sys.argv) != 3:
        print("Usage: python3 patch_keys.py <serial_port> <config_file>")
        print("Example: python3 patch_keys.py /dev/cu.usbmodem101 configs/keys.txt")
        sys.exit(1)
    
    serial_port = sys.argv[1]
    config_file = sys.argv[2]
    
    print(f"Bitcoin Dashboard - Configuration Patcher")
    print(f"=========================================")
    print(f"Serial Port: {serial_port}")
    print(f"Config File: {config_file}")
    print()
    
    # Parse configuration file
    config = parse_config_file(config_file)
    if config is None:
        sys.exit(1)
    
    if not config:
        print("Warning: No valid configuration found in file!")
        print("Make sure the file contains at least one of:")
        print("  SET_WIFI=SSID,Password")
        print("  GEMINI_KEY=AIza...")
        print("  OPENAI_KEY=sk-proj-...")
        sys.exit(1)
    
    print(f"Found configuration:")
    if 'wifi' in config:
        ssid = config['wifi'].split(',')[0]
        print(f"  ✓ WiFi SSID: {ssid}")
    if 'gemini' in config:
        print(f"  ✓ Gemini API Key: {config['gemini'][:10]}...")
    if 'openai' in config:
        print(f"  ✓ OpenAI API Key: {config['openai'][:10]}...")
    print()
    
    try:
        # Open serial connection
        print(f"Connecting to {serial_port}...")
        ser = serial.Serial(serial_port, 115200, timeout=2)
        time.sleep(2)  # Wait for device to be ready
        
        # Clear any pending data
        ser.reset_input_buffer()
        
        print("Connected!")
        print()
        
        # Send WiFi credentials
        if 'wifi' in config:
            print("Setting WiFi credentials...")
            send_command(ser, f"SET_WIFI={config['wifi']}", 2.0)
            print()
        
        # Send Gemini API key
        if 'gemini' in config:
            print("Setting Gemini API key...")
            send_command(ser, f"SET_GEMINI_KEY={config['gemini']}", 2.0)
            print()
        
        # Send OpenAI API key
        if 'openai' in config:
            print("Setting OpenAI API key...")
            send_command(ser, f"SET_OPENAI_KEY={config['openai']}", 2.0)
            print()
        
        # Show final status
        print("Checking configuration...")
        send_command(ser, "STATUS", 2.0)
        
        print()
        print("✓ Configuration patched successfully!")
        print()
        print("Next steps:")
        print("  1. Restart the device (press reset button)")
        print("  2. Device will auto-connect to WiFi on boot")
        print("  3. Monitor with: make monitor")
        
        ser.close()
        
    except serial.SerialException as e:
        print(f"Error: Could not open serial port {serial_port}")
        print(f"Details: {e}")
        print()
        print("Available ports:")
        os.system("python3 -m platformio device list")
        sys.exit(1)
    except KeyboardInterrupt:
        print("\nInterrupted by user")
        sys.exit(1)
    except Exception as e:
        print(f"Error: {e}")
        sys.exit(1)

if __name__ == "__main__":
    main()
