#include "GeminiClient.h"
#include "../screens/BTCDashboardScreen.h"
#include "../Config.h"

GeminiClient::GeminiClient() {
    // Load API key from global config, fallback to hardcoded
    apiKey = globalConfig.getGeminiApiKey();
    if (apiKey.length() == 0) {
        apiKey = GEMINI_API_KEY;  // Fallback to hardcoded key
        Serial.println("⚠️  Using hardcoded Gemini API key (not configured in settings)");
    } else {
        Serial.println("✓ Using Gemini API key from configuration");
    }
    model = GEMINI_MODEL;
}

GeminiClient::GeminiClient(const String& key, const String& modelName) {
    apiKey = key;
    model = modelName;
}

GeminiClient::~GeminiClient() {
    http.end();
}

String GeminiClient::buildEndpointURL() {
    return String(GEMINI_BASE_URL) + model + ":generateContent?key=" + apiKey;
}

String GeminiClient::buildRequestBody(const String& prompt) {
    // Create JSON request body for Gemini API
    DynamicJsonDocument doc(2048);

    JsonArray contents = doc.createNestedArray("contents");
    JsonObject content = contents.createNestedObject();
    JsonArray parts = content.createNestedArray("parts");
    JsonObject part = parts.createNestedObject();
    part["text"] = prompt;

    // Generation configuration
    JsonObject config = doc.createNestedObject("generationConfig");
    config["temperature"] = GEMINI_TEMPERATURE;
    config["maxOutputTokens"] = GEMINI_MAX_OUTPUT_TOKENS;

    String requestBody;
    serializeJson(doc, requestBody);

    return requestBody;
}

bool GeminiClient::parseResponse(const String& response, String& outputText) {
    // Parse Gemini API response
    DynamicJsonDocument doc(GEMINI_MAX_RESPONSE_SIZE);

    DeserializationError error = deserializeJson(doc, response);
    if (error) {
        Serial.print("JSON parsing failed: ");
        Serial.println(error.c_str());
        return false;
    }

    // Navigate through Gemini's response structure
    // Response format: { "candidates": [{ "content": { "parts": [{ "text": "..." }] } }] }
    if (doc.containsKey("candidates")) {
        JsonArray candidates = doc["candidates"];
        if (candidates.size() > 0) {
            JsonObject candidate = candidates[0];
            if (candidate.containsKey("content")) {
                JsonObject content = candidate["content"];
                if (content.containsKey("parts")) {
                    JsonArray parts = content["parts"];
                    if (parts.size() > 0) {
                        JsonObject part = parts[0];
                        if (part.containsKey("text")) {
                            outputText = part["text"].as<String>();
                            return true;
                        }
                    }
                }
            }
        }
    }

    // Check for error in response
    if (doc.containsKey("error")) {
        JsonObject error = doc["error"];
        Serial.print("Gemini API Error: ");
        Serial.println(error["message"].as<String>());
        outputText = "API Error: " + error["message"].as<String>();
        return false;
    }

    Serial.println("Unexpected response structure from Gemini API");
    return false;
}

String GeminiClient::generatePrompt(const BTCData& data) {
    String prompt = "You are a Bitcoin market analyst. Based on the following real-time data, ";
    prompt += "provide a concise market analysis (max 800 words):\n\n";

    prompt += "CURRENT DATA:\n";
    prompt += "- BTC Price: $" + String(data.priceUSD, 2) + " USD";
    if (data.priceEUR > 0) {
        prompt += " (€" + String(data.priceEUR, 2) + " EUR)";
    }
    prompt += "\n";

    if (data.blockHeight > 0) {
        prompt += "- Latest Block: #" + String(data.blockHeight);
        if (data.blockTxCount > 0) {
            prompt += " (" + String(data.blockTxCount) + " transactions)";
        }
        prompt += "\n";
    }

    if (data.mempoolCount > 0) {
        prompt += "- Mempool: " + String(data.mempoolCount) + " pending transactions";
        if (data.mempoolSize > 0) {
            prompt += " (" + String(data.mempoolSize, 2) + " MB)";
        }
        prompt += "\n";
    }

    if (data.feeFast > 0) {
        prompt += "- Network Fees: Fast=" + String(data.feeFast);
        prompt += " Medium=" + String(data.feeMedium);
        prompt += " Slow=" + String(data.feeSlow) + " sat/vB\n";
    }

    prompt += "\nProvide:\n";
    prompt += "1. 📈 Market Summary (2-3 sentences)\n";
    prompt += "2. 💡 Technical Analysis (key levels, trends)\n";
    prompt += "3. 📊 Market Sentiment (bullish/bearish indicators)\n";
    prompt += "4. ⚠️ Risk Assessment (volatility, warnings)\n";
    prompt += "5. 🎯 Price Outlook (short-term forecast)\n\n";
    prompt += "Format with clear sections using emoji headers. Be concise and actionable.";

    return prompt;
}

bool GeminiClient::fetchBitcoinNews(const BTCData& data, String& newsText) {
    // Check WiFi connection
    if (WiFi.status() != WL_CONNECTED) {
        Serial.println("WiFi not connected");
        newsText = "Error: No WiFi connection. Please connect to WiFi first.";
        return false;
    }

    // Generate prompt from Bitcoin data
    String prompt = generatePrompt(data);
    Serial.println("Generated Prompt:");
    Serial.println(prompt);
    Serial.println("---");

    // Build API endpoint
    String endpoint = buildEndpointURL();
    Serial.print("Gemini API Endpoint: ");
    Serial.println(endpoint);

    // Build request body
    String requestBody = buildRequestBody(prompt);

    // Make HTTP POST request
    http.begin(endpoint);
    http.addHeader("Content-Type", "application/json");
    http.setTimeout(GEMINI_TIMEOUT);

    Serial.println("Sending request to Gemini API...");
    int httpCode = http.POST(requestBody);

    if (httpCode > 0) {
        Serial.printf("HTTP Response code: %d\n", httpCode);

        if (httpCode == HTTP_CODE_OK) {
            String response = http.getString();
            Serial.println("Response received, parsing...");

            bool success = parseResponse(response, newsText);
            http.end();

            if (success) {
                Serial.println("News fetched successfully!");
                Serial.println("---");
                Serial.println(newsText);
                Serial.println("---");
            }

            return success;
        } else {
            String response = http.getString();
            Serial.println("HTTP Error Response:");
            Serial.println(response);
            newsText = "API Error: HTTP " + String(httpCode);
            http.end();
            return false;
        }
    } else {
        Serial.printf("HTTP Request failed: %s\n", http.errorToString(httpCode).c_str());
        newsText = "Network Error: Failed to connect to Gemini API";
        http.end();
        return false;
    }
}

bool GeminiClient::testConnection() {
    if (WiFi.status() != WL_CONNECTED) {
        Serial.println("WiFi not connected");
        return false;
    }

    Serial.println("Testing Gemini API connection...");

    String endpoint = buildEndpointURL();
    String testPrompt = "Say 'Hello from Bitcoin Dashboard!' in one sentence.";
    String requestBody = buildRequestBody(testPrompt);

    http.begin(endpoint);
    http.addHeader("Content-Type", "application/json");
    http.setTimeout(GEMINI_TIMEOUT);

    int httpCode = http.POST(requestBody);
    http.end();

    if (httpCode == HTTP_CODE_OK) {
        Serial.println("Gemini API connection test successful!");
        return true;
    } else {
        Serial.printf("Gemini API connection test failed: %d\n", httpCode);
        return false;
    }
}

void GeminiClient::setApiKey(const String& key) {
    apiKey = key;
}
