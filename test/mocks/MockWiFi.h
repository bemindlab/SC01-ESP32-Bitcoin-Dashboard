#ifndef MOCK_WIFI_H
#define MOCK_WIFI_H

#include <string.h>

// WiFi status constants (matching ESP32 WiFi library)
#define WL_IDLE_STATUS      0
#define WL_NO_SSID_AVAIL    1
#define WL_SCAN_COMPLETED   2
#define WL_CONNECTED        3
#define WL_CONNECT_FAILED   4
#define WL_CONNECTION_LOST  5
#define WL_DISCONNECTED     6

/**
 * Mock WiFi for native unit testing
 * Simulates WiFi operations without hardware
 */
class MockWiFiClass {
public:
    MockWiFiClass() : connected(false), currentStatus(WL_DISCONNECTED),
                      scanCount(0), rssi(-70), channel(6) {
        strcpy(connectedSSID, "");
        strcpy(ipAddress, "");
    }

    // WiFi connection
    void begin(const char* ssid, const char* password) {
        strcpy(connectedSSID, ssid);
        strcpy(lastPassword, password);
        // Simulate successful connection by default
        connected = true;
        currentStatus = WL_CONNECTED;
        strcpy(ipAddress, "192.168.1.100");
    }

    int status() {
        return currentStatus;
    }

    void disconnect() {
        connected = false;
        currentStatus = WL_DISCONNECTED;
        strcpy(connectedSSID, "");
        strcpy(ipAddress, "");
    }

    // Network scanning
    int scanNetworks() {
        scanCount = mockNetworkCount;
        return scanCount;
    }

    const char* SSID(int index) {
        if (index >= 0 && index < scanCount) {
            return mockSSIDs[index];
        }
        return "";
    }

    int RSSI(int index) {
        if (index >= 0 && index < scanCount) {
            return mockRSSIs[index];
        }
        return 0;
    }

    int channel(int index) {
        return channel;
    }

    uint8_t encryptionType(int index) {
        if (index >= 0 && index < scanCount) {
            return mockEncryption[index];
        }
        return 0;
    }

    // IP configuration
    const char* localIP() {
        return ipAddress;
    }

    // Mock configuration methods
    void setConnected(bool state) {
        connected = state;
        currentStatus = state ? WL_CONNECTED : WL_DISCONNECTED;
        if (!state) {
            strcpy(ipAddress, "");
        }
    }

    void setStatus(int status) {
        currentStatus = status;
    }

    void setMockNetworks(int count, const char* ssids[], int rssis[], uint8_t encryption[]) {
        mockNetworkCount = count;
        for (int i = 0; i < count && i < 10; i++) {
            mockSSIDs[i] = ssids[i];
            mockRSSIs[i] = rssis[i];
            mockEncryption[i] = encryption[i];
        }
    }

    void setRSSI(int value) { rssi = value; }
    void setChannel(int ch) { channel = ch; }

    // Verification helpers
    const char* getConnectedSSID() { return connectedSSID; }
    const char* getLastPassword() { return lastPassword; }
    int getScanCount() { return scanCount; }
    bool isConnected() { return connected; }

private:
    bool connected;
    int currentStatus;
    char connectedSSID[33];
    char lastPassword[64];
    char ipAddress[16];
    int rssi;
    int channel;

    // Mock scan results
    int scanCount;
    int mockNetworkCount;
    const char* mockSSIDs[10];
    int mockRSSIs[10];
    uint8_t mockEncryption[10];
};

// Global mock WiFi instance
MockWiFiClass WiFi;

#endif // MOCK_WIFI_H
