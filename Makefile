# Makefile for Bitcoin Dashboard (ESP32-S3)
# Provides convenient shortcuts for PlatformIO commands

.PHONY: help build build-single upload upload-single monitor clean test test-native test-hardware devices all update screenshot screenshot-interactive patch-keys install-hooks uninstall-hooks sd-format sd-status sd-enable sd-disable sd-flush sd-memory

# Default target
help:
	@echo "Bitcoin Dashboard - Makefile Commands"
	@echo "======================================"
	@echo ""
	@echo "Building & Uploading:"
	@echo "  make build            - Build project (multi-screen mode)"
	@echo "  make build-single     - Build project (single screen mode - saves ~51KB)"
	@echo "  make upload           - Upload firmware to device"
	@echo "  make upload-single    - Upload single screen firmware"
	@echo "  make all              - Build and upload"
	@echo "  make monitor          - Open serial monitor"
	@echo "  make flash            - Upload and monitor"
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
	@echo "  make patch-keys            - Patch WiFi and API keys from configs/keys.txt"
	@echo ""
	@echo "Git Hooks:"
	@echo "  make install-hooks         - Install pre-commit and pre-push hooks"
	@echo "  make uninstall-hooks       - Remove git hooks"
	@echo ""
	@echo "SD Card:"
	@echo "  make sd-format             - Format SD card (WARNING: Deletes all data!)"
	@echo "  make sd-status             - Check SD card status"
	@echo "  make sd-enable             - Enable SD card logging"
	@echo "  make sd-disable            - Disable SD card logging"
	@echo "  make sd-flush              - Force flush log buffer to SD"
	@echo "  make sd-memory             - Log current memory usage"
	@echo ""

# Build the project (multi-screen mode)
build:
	@echo "Building project (multi-screen mode)..."
	python3 -m platformio run

# Build single screen mode (dashboard only)
build-single:
	@echo "Building project (single screen mode - dashboard only)..."
	@echo "Saves ~51KB flash compared to multi-screen mode"
	python3 -m platformio run -e sc01_plus_single

# Upload firmware to device (multi-screen)
upload:
	@echo "Uploading to device (multi-screen mode)..."
	python3 -m platformio run --target upload

# Upload single screen firmware
upload-single:
	@echo "Uploading to device (single screen mode)..."
	python3 -m platformio run -e sc01_plus_single --target upload

# Build and upload (multi-screen)
all: build upload
	@echo "Build and upload complete!"

# Build and upload single screen
all-single: build-single upload-single
	@echo "Single screen build and upload complete!"

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
	python3 -m platformio test -e test_sc01_plus --upload-port /dev/cu.usbmodem101

# Capture device screenshot
screenshot:
	@echo "Capturing device screenshot..."
	@echo "Make sure firmware is uploaded and device is running!"
	@echo ""
	@python3 scripts/capture_screenshot.py /dev/cu.usbmodem101

# Interactive screenshot (opens serial monitor first)
screenshot-interactive:
	@echo "Opening serial monitor..."
	@echo "Type 'SCREENSHOT' to capture, or Ctrl+C to exit"
	@python3 -m platformio device monitor -b 115200

# Patch API keys and WiFi credentials from file
patch-keys:
	@echo "Patching configuration from configs/keys.txt..."
	@if [ ! -f ./configs/keys.txt ]; then \
		echo "Error: ./configs/keys.txt not found!"; \
		echo ""; \
		echo "Create this file with your configuration:"; \
		echo "  WIFI_SSID=YourNetwork"; \
		echo "  WIFI_PASS=YourPassword"; \
		echo "  GEMINI_KEY=AIza..."; \
		echo "  OPENAI_KEY=sk-proj-..."; \
		echo ""; \
		echo "See configs/keys.txt.template for format"; \
		exit 1; \
	fi
	@echo ""
	@echo "Sending configuration to device via serial..."
	@python3 scripts/patch_keys.py /dev/cu.usbmodem101 ./configs/keys.txt

# Install git hooks
install-hooks:
	@echo "Installing git hooks..."
	@if [ ! -d .git ]; then \
		echo "Error: Not a git repository!"; \
		exit 1; \
	fi
	@mkdir -p .git/hooks
	@cp .githooks/pre-commit .git/hooks/pre-commit
	@cp .githooks/pre-push .git/hooks/pre-push
	@chmod +x .git/hooks/pre-commit
	@chmod +x .git/hooks/pre-push
	@echo "✅ Git hooks installed successfully!"
	@echo ""
	@echo "Hooks installed:"
	@echo "  - pre-commit: Code quality checks"
	@echo "  - pre-push: Test and build validation"
	@echo ""

# Uninstall git hooks
uninstall-hooks:
	@echo "Uninstalling git hooks..."
	@rm -f .git/hooks/pre-commit
	@rm -f .git/hooks/pre-push
	@echo "✅ Git hooks removed successfully!"
	@echo ""

# Format SD card (WARNING: Deletes all data!)
sd-format:
	@echo "⚠️  SD Card Format Utility"
	@echo "============================"
	@echo ""
	@python3 scripts/sd_format.py /dev/cu.usbmodem101

# Check SD card status
sd-status:
	@echo "Checking SD card status..."
	@python3 scripts/send_serial_command.py /dev/cu.usbmodem101 "CHECK_SD_CARD"

# Enable SD card logging
sd-enable:
	@echo "Enabling SD card logging..."
	@python3 scripts/send_serial_command.py /dev/cu.usbmodem101 "LOG_ENABLE"

# Disable SD card logging
sd-disable:
	@echo "Disabling SD card logging..."
	@python3 scripts/send_serial_command.py /dev/cu.usbmodem101 "LOG_DISABLE"

# Force flush log buffer
sd-flush:
	@echo "Flushing log buffer to SD card..."
	@python3 scripts/send_serial_command.py /dev/cu.usbmodem101 "LOG_FLUSH"

# Log memory usage
sd-memory:
	@echo "Logging memory usage to SD card..."
	@python3 scripts/send_serial_command.py /dev/cu.usbmodem101 "LOG_MEMORY"
