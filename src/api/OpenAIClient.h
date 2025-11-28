#ifndef OPENAI_CLIENT_H
#define OPENAI_CLIENT_H

#include <Arduino.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include "../screens/BTCDashboardScreen.h"

// Trading signal types
enum TradingSignal {
    SIGNAL_STRONG_BUY,
    SIGNAL_BUY,
    SIGNAL_HOLD,
    SIGNAL_SELL,
    SIGNAL_STRONG_SELL,
    SIGNAL_UNCERTAIN
};

// Trading suggestion data structure
struct TradingSuggestion {
    TradingSignal signal = SIGNAL_UNCERTAIN;
    int confidence = 0;           // 0-100%
    String recommendation = "";    // Main analysis text
    String keyFactors[5];         // Up to 5 bullet points
    int keyFactorCount = 0;
    float targetPrice = 0;
    float stopLoss = 0;
    unsigned long timestamp = 0;
    bool isValid = false;
};

/**
 * OpenAI API Client for trading suggestions
 * Uses GPT-3.5 Turbo or GPT-4 for Bitcoin trading analysis
 */
class OpenAIClient {
private:
    String apiKey;
    String model;

    // API endpoint
    static constexpr const char* API_URL = "https://api.openai.com/v1/chat/completions";

    // Helper methods
    String buildRequestBody(const BTCData& data);
    bool parseResponse(const String& response, TradingSuggestion& suggestion);
    TradingSignal parseSignal(const String& signalText);

public:
    OpenAIClient();
    OpenAIClient(const String& key, const String& modelName = "gpt-3.5-turbo");

    // Main method to fetch trading suggestion
    bool fetchTradingSuggestion(const BTCData& data, TradingSuggestion& suggestion);

    // Generate trading analysis prompt
    String generateTradingPrompt(const BTCData& data);

    // Test API connectivity
    bool testConnection();

    // Set custom API key
    void setApiKey(const String& key);

    // Set model (gpt-3.5-turbo or gpt-4-turbo-preview)
    void setModel(const String& modelName);
};

#endif // OPENAI_CLIENT_H
