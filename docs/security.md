# Security Policy

## Supported Versions

Currently supported versions of the Bitcoin Dashboard firmware:

| Version | Supported          |
| ------- | ------------------ |
| main    | :white_check_mark: |

## Security Considerations

### Hardware Security

**Physical Access:**
- Device has no physical security mechanisms
- Full flash access via USB connection
- Serial debugging enabled by default
- Consider physical security for deployment environments

**Memory Protection:**
- No secure boot enabled
- Flash encryption not configured
- WiFi credentials stored in plaintext in flash
- Bitcoin data is public information (low sensitivity)

### Network Security

**WiFi Configuration:**
- Credentials hardcoded in source code (main.cpp:10-11)
- Must be changed before deployment
- Device only supports WPA2/WPA3 networks
- No fallback AP mode for configuration

**API Communication:**
- mempool.space API uses HTTPS (TLS encryption)
- No API key required (public endpoints)
- 10-second timeout prevents hanging connections
- No user authentication implemented

**Attack Surface:**
- No incoming network connections accepted
- Device only makes outbound HTTPS requests
- No web server or exposed services
- No OTA update mechanism (reduces remote attack vectors)

### Code Security

**Input Validation:**
- JSON parsing uses ArduinoJson with fixed buffer sizes
- No buffer overflow protection for malformed API responses
- HTTP timeout prevents DoS via slow responses
- No user input validation needed (read-only display)

**Dependencies:**
- External libraries managed via PlatformIO
- No automated dependency scanning configured
- Manual review required for library updates

## Reporting a Vulnerability

### Scope

**In Scope:**
- Memory corruption vulnerabilities
- Network protocol issues
- Dependency vulnerabilities in third-party libraries
- WiFi credential exposure
- Code injection via API responses
- Denial of service vulnerabilities

**Out of Scope:**
- Physical access attacks (device is not physically secured)
- WiFi password cracking (use strong passwords)
- Social engineering
- Issues in third-party APIs (mempool.space)

### Reporting Process

1. **Do NOT** create a public GitHub issue for security vulnerabilities
2. Contact the maintainers directly via:
   - GitHub Security Advisory (preferred)
   - Project maintainer email (check repository)
3. Include in your report:
   - Description of the vulnerability
   - Steps to reproduce
   - Potential impact
   - Suggested fix (if available)
   - Hardware/software version affected

### Response Timeline

- **Initial Response:** Within 7 days
- **Severity Assessment:** Within 14 days
- **Fix Development:** Varies by severity
- **Public Disclosure:** After fix is released or 90 days (whichever comes first)

### Severity Levels

**Critical:**
- Remote code execution
- WiFi credential theft via remote attack
- Complete device compromise

**High:**
- Memory corruption leading to crashes
- Information disclosure of sensitive data
- Network protocol vulnerabilities

**Medium:**
- Denial of service (local only)
- Input validation issues
- Dependency vulnerabilities with no known exploit

**Low:**
- Information disclosure of public data
- Minor configuration issues
- Cosmetic security issues

## Security Best Practices

### For Users

**WiFi Credentials:**
```cpp
// main.cpp:10-11
const char* ssid = "YOUR_NETWORK";        // Change before flashing
const char* password = "YOUR_PASSWORD";    // Use strong password
```

**Network Isolation:**
- Consider deploying on guest/IoT network
- Device does not require LAN access
- Only needs internet access to mempool.space

**Physical Security:**
- Device contains WiFi credentials in plaintext flash
- Secure physical access if credentials are sensitive
- Consider using guest network credentials

### For Developers

**Secure Coding:**
- Validate all JSON data from API responses
- Use fixed buffer sizes (avoid dynamic allocation where possible)
- Check return values from all API calls
- Log errors to serial for debugging

**Dependency Management:**
- Review library updates before applying
- Pin library versions in platformio.ini
- Test thoroughly after dependency updates
- Monitor security advisories for:
  - LovyanGFX
  - ArduinoJson
  - ESP32 Arduino core

**Build Security:**
- Never commit WiFi credentials to repository
- Use environment variables or local config files
- Add sensitive files to .gitignore
- Review code changes for credential leaks

**Future Enhancements:**
- Implement secure boot (ESP32-S3 feature)
- Enable flash encryption for production
- Add WiFi credential configuration via BLE or AP mode
- Implement certificate pinning for mempool.space
- Add OTA updates with signature verification

## Secure Build Configuration

### Recommended platformio.ini Flags

```ini
# Future security enhancements (not yet implemented)
board_build.flash_mode = qio
board_build.partitions = default_16MB.csv

# Potential build flags for production:
# -DCORE_DEBUG_LEVEL=0              # Disable debug logging
# -DENABLE_SECURE_BOOT              # Enable secure boot (requires setup)
# -DENABLE_FLASH_ENCRYPTION         # Encrypt flash (requires setup)
```

### WiFi Credential Management

**Current Implementation (Development):**
```cpp
const char* ssid = "YOUR_NETWORK";
const char* password = "YOUR_PASSWORD";
```

**Recommended for Production:**
1. Use environment variables during build
2. Implement WiFi manager with BLE configuration
3. Store credentials in NVS (Non-Volatile Storage) with encryption
4. Implement factory reset mechanism

## Known Security Limitations

### Current Limitations

1. **Plaintext WiFi Credentials**
   - Stored in source code and flash memory
   - Readable via USB connection
   - No encryption at rest

2. **No Authentication**
   - Device is read-only (display only)
   - No user authentication needed for current features
   - May be required for future interactive features

3. **No Secure Boot**
   - Firmware not signed
   - Flash not encrypted
   - Physical access allows firmware modification

4. **Limited Input Validation**
   - JSON parsing relies on ArduinoJson safety
   - No additional validation of API responses
   - Malformed responses may cause crashes

5. **No OTA Updates**
   - Firmware updates require USB connection
   - No remote update capability
   - Reduces remote attack surface but complicates patching

### Mitigation Strategies

- Deploy on isolated/guest networks
- Use strong WiFi passwords (WPA3 recommended)
- Implement physical security for device
- Monitor serial output for anomalies
- Regular firmware updates via USB

## Compliance and Standards

This project is designed for:
- **Personal use and education**
- **Non-critical applications**
- **Development and testing environments**

Not recommended for:
- Production financial systems
- High-security environments
- Compliance-regulated deployments (without additional hardening)

## Security Roadmap

Planned security enhancements:

- [ ] WiFi credential configuration via BLE
- [ ] NVS encryption for stored credentials
- [ ] Secure boot implementation
- [ ] Flash encryption
- [ ] Certificate pinning for API endpoints
- [ ] OTA updates with signature verification
- [ ] Automated dependency vulnerability scanning
- [ ] Runtime memory protection
- [ ] Audit logging for security events

## Contact

For security concerns or questions:
- GitHub Security Advisory (preferred)
- Project issue tracker (for non-sensitive issues)
- Check repository for maintainer contact information

## Acknowledgments

We appreciate responsible disclosure from security researchers and the community. Contributors who report valid security issues will be acknowledged (with permission) in release notes.
