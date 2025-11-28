# Makefile for Bitcoin Dashboard (ESP32-S3)
# Provides convenient shortcuts for PlatformIO commands

.PHONY: help build upload monitor clean test test-native test-hardware devices all update screenshot screenshot-interactive

# Default target
help:
	@echo "Bitcoin Dashboard - Makefile Commands"
	@echo "======================================"
	@echo ""
	@echo "Building & Uploading:"
	@echo "  make build          - Build the project"
	@echo "  make upload         - Upload firmware to device"
	@echo "  make all            - Build and upload"
	@echo "  make monitor        - Open serial monitor"
	@echo "  make flash          - Upload and monitor"
	@echo ""
	@echo "Testing:"
	@echo "  make test           - Run all tests (native + hardware)"
	@echo "  make test-native    - Run native tests only (desktop)"
	@echo "  make test-hardware  - Run hardware tests (on device)"
	@echo ""
	@echo "Maintenance:"
	@echo "  make clean          - Clean build files"
	@echo "  make devices        - List connected devices"
	@echo "  make update         - Update PlatformIO and libraries"
	@echo ""
	@echo "Development:"
	@echo "  make debug-monitor         - Monitor with exception decoder"
	@echo "  make erase                 - Erase flash memory"
	@echo "  make check                 - Check configuration"
	@echo "  make screenshot            - Capture device screen to .tmp/"
	@echo "  make screenshot-interactive - Open serial monitor for manual commands"
	@echo ""

# Build the project
build:
	@echo "Building project..."
	python3 -m platformio run

# Upload firmware to device
upload:
	@echo "Uploading to device..."
	python3 -m platformio run --target upload

# Build and upload
all: build upload
	@echo "Build and upload complete!"

# Open serial monitor
monitor:
	@echo "Opening serial monitor (115200 baud)..."
	@echo "Press Ctrl+C to exit"
	python3 -m platformio device monitor

# Upload and monitor
flash: upload monitor

# Monitor with exception decoder (for crash analysis)
debug-monitor:
	@echo "Opening monitor with exception decoder..."
	python3 -m platformio device monitor --filter esp32_exception_decoder

# Clean build artifacts
clean:
	@echo "Cleaning build files..."
	python3 -m platformio run --target clean
	@echo "Clean complete!"

# List connected devices
devices:
	@echo "Connected serial devices:"
	python3 -m platformio device list

# Update PlatformIO core and libraries
update:
	@echo "Updating PlatformIO..."
	python3 -m pip install --upgrade platformio
	@echo "Updating libraries..."
	python3 -m platformio lib update
	@echo "Update complete!"

# Erase flash (useful for troubleshooting)
erase:
	@echo "Erasing flash memory..."
	python3 -m platformio run --target erase

# Check environment and configuration
check:
	@echo "Checking PlatformIO configuration..."
	python3 -m platformio check
	@echo ""
	@echo "Project information:"
	python3 -m platformio project config

# Show library dependencies
libs:
	@echo "Installed libraries:"
	python3 -m platformio lib list

# Show platform information
info:
	@echo "Platform information:"
	python3 -m platformio platform show espressif32

# Run all tests
test: test-native
	@echo "All tests complete!"

# Run native tests (desktop)
test-native:
	@echo "Running native unit tests..."
	python3 -m platformio test -e native

# Run hardware tests (on device)
test-hardware:
	@echo "Running hardware integration tests..."
	@echo "Make sure device is connected!"
	python3 -m platformio test -e test_sc01_plus --upload-port /dev/cu.usbmodem1101

# Capture device screenshot
screenshot:
	@echo "Capturing device screenshot..."
	@echo "Make sure firmware is uploaded and device is running!"
	@echo ""
	@python3 scripts/capture_screenshot.py /dev/cu.usbmodem1101

# Interactive screenshot (opens serial monitor first)
screenshot-interactive:
	@echo "Opening serial monitor..."
	@echo "Type 'SCREENSHOT' to capture, or Ctrl+C to exit"
	@python3 -m platformio device monitor -b 115200
