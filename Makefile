# Makefile for Bitcoin Dashboard (ESP32-S3)
# Provides convenient shortcuts for PlatformIO commands

.PHONY: help build upload monitor clean test devices all update screenshot

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
	@echo "Maintenance:"
	@echo "  make clean          - Clean build files"
	@echo "  make devices        - List connected devices"
	@echo "  make update         - Update PlatformIO and libraries"
	@echo ""
	@echo "Development:"
	@echo "  make debug-monitor  - Monitor with exception decoder"
	@echo "  make erase          - Erase flash memory"
	@echo "  make check          - Check configuration"
	@echo "  make screenshot     - Capture device screen to .tmp/"
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

# Capture device screenshot
screenshot:
	@echo "Capturing device screen..."
	@mkdir -p /Users/lps/Solutions/bemind-embeded/btc-dashboard/.tmp
	@TIMESTAMP=$$(date +%Y%m%d_%H%M%S); \
	echo "Screenshot will be saved to: .tmp/screenshot_$$TIMESTAMP.raw"; \
	echo ""; \
	echo "Note: Screen capture requires custom firmware support."; \
	echo "To enable screenshots:"; \
	echo "  1. Add screen capture code to src/main.cpp"; \
	echo "  2. Send display buffer data via Serial when triggered"; \
	echo "  3. Use this command to save the data"; \
	echo ""; \
	echo "For now, you can manually trigger a screen dump via serial commands."; \
	echo "Output directory: .tmp/"
