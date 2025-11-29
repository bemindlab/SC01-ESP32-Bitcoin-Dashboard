#include "OpenAIClient.h"
#include "../utils/SDLogger.h"

OpenAIClient::OpenAIClient() {
    model = "gpt-3.5-turbo";  // Default to cost-effective model
}

OpenAIClient::OpenAIClient(const String& key, const String& modelName) {
    apiKey = key;
    model = modelName;
}

void OpenAIClient::setApiKey(const String& key) {
    apiKey = key;
}

void OpenAIClient::setModel(const String& modelName) {
    model = modelName;
}

String OpenAIClient::generateTradingPrompt(const BTCData& data) {
    char prompt[1024];
    snprintf(prompt, sizeof(prompt),
        "You are a professional Bitcoin trading analyst. Analyze the following Bitcoin market data and provide a concise trading suggestion.\n\n"
        "Market Data:\n"
        "- Current Price: $%.0f USD\n"
        "- Block Height: %d\n"
        "- Mempool: %d pending transactions\n"
        "- Mempool Size: %.1f MB\n"
        "- Fee Rates: Fast %d sat/vB, Medium %d sat/vB, Slow %d sat/vB\n\n"
        "Provide your analysis in this format:\n\n"
        "Signal: [STRONG_BUY|BUY|HOLD|SELL|STRONG_SELL]\n"
        "Confidence: [0-100]%%\n\n"
        "Recommendation:\n"
        "[2-3 sentences with actionable advice, including entry points, stop-loss, and targets if applicable]\n\n"
        "Key Factors:\n"
        "- [Factor 1]\n"
        "- [Factor 2]\n"
        "- [Factor 3]\n\n"
        "Keep your response under 400 words. Focus on actionable insights.",
        data.priceUSD,
        data.blockHeight,
        data.mempoolCount,
        data.mempoolSize,
        data.feeFast,
        data.feeMedium,
        data.feeSlow
    );

    return String(prompt);
}

String OpenAIClient::buildRequestBody(const BTCData& data) {
    DynamicJsonDocument doc(4096);

    doc["model"] = model;
    doc["max_tokens"] = 500;
    doc["temperature"] = 0.7;

    JsonArray messages = doc.createNestedArray("messages");

    // System message
    JsonObject systemMsg = messages.createNestedObject();
    systemMsg["role"] = "system";
    systemMsg["content"] = "You are a professional Bitcoin trading analyst. Provide concise, actionable trading suggestions based on market data.";

    // User message with market data
    JsonObject userMsg = messages.createNestedObject();
    userMsg["role"] = "user";
    userMsg["content"] = generateTradingPrompt(data);

    String requestBody;
    serializeJson(doc, requestBody);

    return requestBody;
}

TradingSignal OpenAIClient::parseSignal(const String& signalText) {
    String signal = signalText;
    signal.toUpperCase();
    signal.trim();

    if (signal.indexOf("STRONG_BUY") >= 0 || signal.indexOf("STRONG BUY") >= 0) {
        return SIGNAL_STRONG_BUY;
    } else if (signal.indexOf("STRONG_SELL") >= 0 || signal.indexOf("STRONG SELL") >= 0) {
        return SIGNAL_STRONG_SELL;
    } else if (signal.indexOf("BUY") >= 0) {
        return SIGNAL_BUY;
    } else if (signal.indexOf("SELL") >= 0) {
        return SIGNAL_SELL;
    } else if (signal.indexOf("HOLD") >= 0) {
        return SIGNAL_HOLD;
    }

    return SIGNAL_UNCERTAIN;
}

bool OpenAIClient::parseResponse(const String& response, TradingSuggestion& suggestion) {
    DynamicJsonDocument doc(8192);
    DeserializationError error = deserializeJson(doc, response);

    if (error) {
        Serial.print("JSON parse error: ");
        Serial.println(error.c_str());
        return false;
    }

    // Check for API errors
    if (doc.containsKey("error")) {
        Serial.print("OpenAI API error: ");
        Serial.println(doc["error"]["message"].as<String>());
        return false;
    }

    // Extract content from response
    if (!doc.containsKey("choices") || doc["choices"].size() == 0) {
        Serial.println("No choices in response");
        return false;
    }

    String content = doc["choices"][0]["message"]["content"].as<String>();
    if (content.length() == 0) {
        Serial.println("Empty content in response");
        return false;
    }

    Serial.println("\n=== OpenAI Response ===");
    Serial.println(content);
    Serial.println("======================\n");

    // Parse the structured response
    int signalIdx = content.indexOf("Signal:");
    int confidenceIdx = content.indexOf("Confidence:");
    int recommendationIdx = content.indexOf("Recommendation:");
    int keyFactorsIdx = content.indexOf("Key Factors:");

    // Extract signal
    if (signalIdx >= 0) {
        int lineEnd = content.indexOf('\n', signalIdx);
        String signalLine = content.substring(signalIdx + 7, lineEnd);
        suggestion.signal = parseSignal(signalLine);
    }

    // Extract confidence
    if (confidenceIdx >= 0) {
        int lineEnd = content.indexOf('\n', confidenceIdx);
        String confLine = content.substring(confidenceIdx + 11, lineEnd);
        confLine.trim();
        // Remove % sign if present
        confLine.replace("%", "");
        suggestion.confidence = confLine.toInt();
        // Clamp to 0-100
        if (suggestion.confidence < 0) suggestion.confidence = 0;
        if (suggestion.confidence > 100) suggestion.confidence = 100;
    }

    // Extract recommendation
    if (recommendationIdx >= 0 && keyFactorsIdx >= 0) {
        suggestion.recommendation = content.substring(recommendationIdx + 15, keyFactorsIdx);
        suggestion.recommendation.trim();
    } else if (recommendationIdx >= 0) {
        suggestion.recommendation = content.substring(recommendationIdx + 15);
        suggestion.recommendation.trim();
    }

    // Extract key factors
    if (keyFactorsIdx >= 0) {
        String factorsSection = content.substring(keyFactorsIdx + 12);
        suggestion.keyFactorCount = 0;

        int pos = 0;
        while (suggestion.keyFactorCount < 5) {
            int dashPos = factorsSection.indexOf('-', pos);
            if (dashPos < 0) break;

            int nextDashPos = factorsSection.indexOf('-', dashPos + 1);
            String factor;
            if (nextDashPos < 0) {
                factor = factorsSection.substring(dashPos + 1);
            } else {
                factor = factorsSection.substring(dashPos + 1, nextDashPos);
            }

            factor.trim();
            if (factor.length() > 0) {
                suggestion.keyFactors[suggestion.keyFactorCount++] = factor;
            }

            if (nextDashPos < 0) break;
            pos = nextDashPos;
        }
    }

    suggestion.timestamp = millis();
    suggestion.isValid = (suggestion.signal != SIGNAL_UNCERTAIN &&
                         suggestion.confidence > 0 &&
                         suggestion.recommendation.length() > 0);

    return suggestion.isValid;
}

bool OpenAIClient::fetchTradingSuggestion(const BTCData& data, TradingSuggestion& suggestion) {
    if (apiKey.length() == 0) {
        Serial.println("Error: OpenAI API key not set");
        return false;
    }

    HTTPClient http;
    http.begin(API_URL);
    http.setTimeout(30000);  // 30 second timeout

    // Set headers
    http.addHeader("Content-Type", "application/json");
    http.addHeader("Authorization", "Bearer " + apiKey);

    // Build request body
    String requestBody = buildRequestBody(data);

    Serial.println("\n=== Fetching Trading Suggestion ===");
    Serial.println("Model: " + model);
    Serial.println("Request size: " + String(requestBody.length()) + " bytes");

    // Make POST request
    unsigned long startTime = millis();
    int httpCode = http.POST(requestBody);
    unsigned long duration = millis() - startTime;

    if (httpCode == HTTP_CODE_OK) {
        String response = http.getString();
        size_t responseSize = response.length();

        // Log successful API call
        sdLogger.logAPI("openai", "/v1/chat/completions", httpCode, duration, responseSize);

        http.end();

        // Parse response
        bool success = parseResponse(response, suggestion);
        if (!success) {
            sdLogger.logAPIError("openai", "/v1/chat/completions", httpCode, "Response parse error");
        }
        return success;
    } else {
        // Log API error
        sdLogger.logAPIError("openai", "/v1/chat/completions", httpCode,
                           httpCode > 0 ? "HTTP error" : "Connection failed");

        Serial.print("HTTP error: ");
        Serial.println(httpCode);
        if (httpCode > 0) {
            Serial.println(http.getString());
        }
        http.end();
        return false;
    }
}

bool OpenAIClient::testConnection() {
    HTTPClient http;
    http.begin(API_URL);
    http.setTimeout(10000);

    http.addHeader("Content-Type", "application/json");
    http.addHeader("Authorization", "Bearer " + apiKey);

    // Minimal test request
    String testBody = "{\"model\":\"" + model + "\",\"messages\":[{\"role\":\"user\",\"content\":\"test\"}],\"max_tokens\":5}";

    unsigned long startTime = millis();
    int httpCode = http.POST(testBody);
    unsigned long duration = millis() - startTime;

    if (httpCode == HTTP_CODE_OK) {
        String response = http.getString();
        size_t responseSize = response.length();
        http.end();

        // Log successful test
        sdLogger.logAPI("openai", "/v1/chat/completions (test)", httpCode, duration, responseSize);

        return true;
    } else {
        http.end();

        // Log test failure
        sdLogger.logAPIError("openai", "/v1/chat/completions (test)", httpCode, "Connection test failed");

        return false;
    }
}
