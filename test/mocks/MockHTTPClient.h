#ifndef MOCK_HTTP_CLIENT_H
#define MOCK_HTTP_CLIENT_H

#include <string.h>

/**
 * Mock HTTPClient for native unit testing
 * Simulates HTTP requests without network
 */
class MockHTTPClient {
public:
    MockHTTPClient() : mockCode(200), mockPayload(""), requestCount(0) {}

    // Set mock response for testing
    void setMockResponse(int code, const char* payload) {
        mockCode = code;
        mockPayload = payload;
    }

    // Simulate HTTP operations
    bool begin(const char* url) {
        lastURL = url;
        return true;
    }

    int GET() {
        requestCount++;
        return mockCode;
    }

    const char* getString() {
        return mockPayload;
    }

    void end() {
        // Cleanup (no-op for mock)
    }

    void setTimeout(int timeout) {
        timeoutMs = timeout;
    }

    // Verification helpers
    const char* getLastURL() { return lastURL; }
    int getRequestCount() { return requestCount; }
    int getTimeout() { return timeoutMs; }

    // Preset common responses
    void setMockPriceResponse() {
        setMockResponse(200, "{\"USD\":91396,\"EUR\":85000,\"GBP\":72000,\"CAD\":127000,\"CHF\":81000,\"AUD\":141000,\"JPY\":14150000}");
    }

    void setMockBlocksResponse() {
        setMockResponse(200, "[{\"id\":\"00000000000000000002a23d6df20eecec15b21d32c75833cce28f113de888b7\",\"height\":867095,\"version\":536870912,\"timestamp\":1702396800,\"tx_count\":3527,\"size\":1582070,\"weight\":3992865,\"merkle_root\":\"abc123\",\"previousblockhash\":\"000000000000000000026e0e699089cfce2d0a9c0b8e9f9f5c7c7f2f0b6c1234\",\"mediantime\":1702394200,\"nonce\":123456789,\"bits\":\"170e95e1\",\"difficulty\":72049044147000.03}]");
    }

    void setMockMempoolResponse() {
        setMockResponse(200, "{\"count\":47853,\"vsize\":85432156,\"total_fee\":147853269,\"fee_histogram\":[[1,250000],[2,500000],[3,750000]]}");
    }

    void setMockFeesResponse() {
        setMockResponse(200, "{\"fastestFee\":25,\"halfHourFee\":18,\"hourFee\":12,\"economyFee\":8,\"minimumFee\":1}");
    }

    void setMockError404() {
        setMockResponse(404, "Not Found");
    }

    void setMockError500() {
        setMockResponse(500, "Internal Server Error");
    }

    void setMockTimeout() {
        setMockResponse(-1, "");
    }

private:
    int mockCode;
    const char* mockPayload;
    const char* lastURL;
    int requestCount;
    int timeoutMs;
};

#endif // MOCK_HTTP_CLIENT_H
