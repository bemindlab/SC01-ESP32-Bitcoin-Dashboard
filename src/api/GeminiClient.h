#ifndef GEMINI_CLIENT_H
#define GEMINI_CLIENT_H

#include <Arduino.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>

// Gemini API Configuration
// IMPORTANT: Replace with your actual API key before building
// Get your free key at: https://makersuite.google.com/app/apikey
#define GEMINI_API_KEY "your-api-key-here"
#define GEMINI_MODEL "gemini-1.5-flash"
#define GEMINI_BASE_URL "https://generativelanguage.googleapis.com/v1beta/models/"

// API Settings
#define GEMINI_TIMEOUT 30000          // 30 seconds
#define GEMINI_MAX_RESPONSE_SIZE 4096 // 4KB response buffer
#define GEMINI_MAX_OUTPUT_TOKENS 1024 // Limit response length
#define GEMINI_TEMPERATURE 0.7        // Creativity level (0.0-1.0)

#include "BTCData.h"

class GeminiClient {
private:
    HTTPClient http;
    String apiKey;
    String model;

    // Build the full API endpoint URL
    String buildEndpointURL();

    // Create the JSON request body
    String buildRequestBody(const String& prompt);

    // Parse Gemini API response
    bool parseResponse(const String& response, String& outputText);

public:
    GeminiClient();
    GeminiClient(const String& key, const String& modelName = GEMINI_MODEL);
    ~GeminiClient();

    // Main method to get Bitcoin news/analysis
    bool fetchBitcoinNews(const BTCData& data, String& newsText);

    // Get DCA (Dollar Cost Average) recommendation
    bool fetchDCARecommendation(const BTCData& data, String& recommendation);

    // Get short-term trading signal (15m-1h timeframe)
    bool fetchTradingSignal(const BTCData& data, String& signal);

    // Generate prompt from Bitcoin data
    String generatePrompt(const BTCData& data);

    // Test API connectivity
    bool testConnection();

    // Set custom API key
    void setApiKey(const String& key);
};

#endif
