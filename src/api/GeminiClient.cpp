#include "GeminiClient.h"
#include "BTCData.h"
#include "../Config.h"
#include "../utils/SDLogger.h"

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
    unsigned long startTime = millis();
    int httpCode = http.POST(requestBody);
    unsigned long duration = millis() - startTime;

    if (httpCode > 0) {
        Serial.printf("HTTP Response code: %d\n", httpCode);

        if (httpCode == HTTP_CODE_OK) {
            String response = http.getString();
            size_t responseSize = response.length();

            // Log successful API call
            sdLogger.logAPI("gemini", "/generateContent", httpCode, duration, responseSize);

            Serial.println("Response received, parsing...");

            bool success = parseResponse(response, newsText);
            http.end();

            if (success) {
                Serial.println("News fetched successfully!");
                Serial.println("---");
                Serial.println(newsText);
                Serial.println("---");
            } else {
                sdLogger.logAPIError("gemini", "/generateContent", httpCode, "Response parse error");
            }

            return success;
        } else {
            String response = http.getString();
            Serial.println("HTTP Error Response:");
            Serial.println(response);

            // Log API error
            sdLogger.logAPIError("gemini", "/generateContent", httpCode, "HTTP error");

            newsText = "API Error: HTTP " + String(httpCode);
            http.end();
            return false;
        }
    } else {
        // Log connection failure
        sdLogger.logAPIError("gemini", "/generateContent", httpCode, "Connection failed");

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

    unsigned long startTime = millis();
    int httpCode = http.POST(requestBody);
    unsigned long duration = millis() - startTime;

    if (httpCode == HTTP_CODE_OK) {
        String response = http.getString();
        size_t responseSize = response.length();
        http.end();

        // Log successful test
        sdLogger.logAPI("gemini", "/generateContent (test)", httpCode, duration, responseSize);

        Serial.println("Gemini API connection test successful!");
        return true;
    } else {
        http.end();

        // Log test failure
        sdLogger.logAPIError("gemini", "/generateContent (test)", httpCode, "Connection test failed");

        Serial.printf("Gemini API connection test failed: %d\n", httpCode);
        return false;
    }
}

void GeminiClient::setApiKey(const String& key) {
    apiKey = key;
}

bool GeminiClient::fetchDCARecommendation(const BTCData& data, String& recommendation) {
    // Check WiFi connection
    if (WiFi.status() != WL_CONNECTED) {
        Serial.println("WiFi not connected");
        recommendation = "WAIT";
        return false;
    }

    // Generate DCA-specific prompt
    String prompt = "You are a Bitcoin DCA (Dollar Cost Average) advisor. ";
    prompt += "Based on the following current market data, provide a ONE-WORD recommendation: BUY, SELL, or WAIT.\n\n";

    prompt += "CURRENT DATA:\n";
    prompt += "- BTC Price: $" + String(data.priceUSD, 2) + " USD\n";

    if (data.feeFast > 0) {
        prompt += "- Network Fees: Fast=" + String(data.feeFast) + " sat/vB\n";
    }

    if (data.mempoolCount > 0) {
        prompt += "- Mempool: " + String(data.mempoolCount) + " pending transactions\n";
    }

    prompt += "\nDCA STRATEGY CONSIDERATIONS:\n";
    prompt += "- BUY: Good time to accumulate (favorable price, low volatility, or regular schedule)\n";
    prompt += "- SELL: Consider taking profits (extreme highs, bearish signals)\n";
    prompt += "- WAIT: Hold off on buying (high fees, extreme volatility, or uncertain conditions)\n\n";
    prompt += "Respond with ONLY ONE WORD: BUY, SELL, or WAIT. No explanation needed.";

    Serial.println("DCA Prompt:");
    Serial.println(prompt);

    // Build and send request
    String endpoint = buildEndpointURL();
    String requestBody = buildRequestBody(prompt);

    http.begin(endpoint);
    http.addHeader("Content-Type", "application/json");
    http.setTimeout(GEMINI_TIMEOUT);

    Serial.println("Fetching DCA recommendation from Gemini...");
    unsigned long startTime = millis();
    int httpCode = http.POST(requestBody);
    unsigned long duration = millis() - startTime;

    if (httpCode == HTTP_CODE_OK) {
        String response = http.getString();
        size_t responseSize = response.length();

        sdLogger.logAPI("gemini", "/dca-recommendation", httpCode, duration, responseSize);

        String fullResponse;
        bool success = parseResponse(response, fullResponse);
        http.end();

        if (success) {
            // Extract first word from response
            fullResponse.trim();
            fullResponse.toUpperCase();

            if (fullResponse.indexOf("BUY") >= 0) {
                recommendation = "BUY";
            } else if (fullResponse.indexOf("SELL") >= 0) {
                recommendation = "SELL";
            } else if (fullResponse.indexOf("WAIT") >= 0) {
                recommendation = "WAIT";
            } else {
                recommendation = "WAIT"; // Default to WAIT if unclear
            }

            Serial.printf("DCA Recommendation: %s\n", recommendation.c_str());
            return true;
        } else {
            sdLogger.logAPIError("gemini", "/dca-recommendation", httpCode, "Parse error");
            recommendation = "WAIT";
            return false;
        }
    } else {
        sdLogger.logAPIError("gemini", "/dca-recommendation", httpCode, "HTTP error");
        Serial.printf("DCA request failed: %d\n", httpCode);
        recommendation = "WAIT";
        http.end();
        return false;
    }
}

bool GeminiClient::fetchTradingSignal(const BTCData& data, String& signal) {
    // Check WiFi connection
    if (WiFi.status() != WL_CONNECTED) {
        Serial.println("WiFi not connected");
        signal = "HOLD";
        return false;
    }

    // Generate short-term trading signal prompt
    String prompt = "You are a Bitcoin short-term trading analyst. ";
    prompt += "Based on the following current market data, provide a ONE-WORD trading signal for the 15-minute to 1-hour timeframe: BUY, SELL, or HOLD.\n\n";

    prompt += "CURRENT DATA:\n";
    prompt += "- BTC Price: $" + String(data.priceUSD, 2) + " USD\n";

    if (data.blockHeight > 0) {
        prompt += "- Latest Block: #" + String(data.blockHeight) + "\n";
    }

    if (data.feeFast > 0) {
        prompt += "- Network Fees: Fast=" + String(data.feeFast);
        prompt += " Medium=" + String(data.feeMedium);
        prompt += " Slow=" + String(data.feeSlow) + " sat/vB\n";
    }

    if (data.mempoolCount > 0) {
        prompt += "- Mempool: " + String(data.mempoolCount) + " pending transactions\n";
    }

    prompt += "\nSHORT-TERM TRADING CONSIDERATIONS (15m-1h timeframe):\n";
    prompt += "- BUY: Bullish momentum, accumulation signals, support levels holding\n";
    prompt += "- SELL: Bearish signals, profit-taking opportunity, resistance rejection\n";
    prompt += "- HOLD: Consolidation, unclear direction, wait for confirmation\n\n";
    prompt += "Respond with ONLY ONE WORD: BUY, SELL, or HOLD. No explanation needed.";

    Serial.println("Trading Signal Prompt:");
    Serial.println(prompt);

    // Build and send request
    String endpoint = buildEndpointURL();
    String requestBody = buildRequestBody(prompt);

    http.begin(endpoint);
    http.addHeader("Content-Type", "application/json");
    http.setTimeout(GEMINI_TIMEOUT);

    Serial.println("Fetching trading signal from Gemini...");
    unsigned long startTime = millis();
    int httpCode = http.POST(requestBody);
    unsigned long duration = millis() - startTime;

    if (httpCode == HTTP_CODE_OK) {
        String response = http.getString();
        size_t responseSize = response.length();

        sdLogger.logAPI("gemini", "/trading-signal", httpCode, duration, responseSize);

        String fullResponse;
        bool success = parseResponse(response, fullResponse);
        http.end();

        if (success) {
            // Extract first word from response
            fullResponse.trim();
            fullResponse.toUpperCase();

            if (fullResponse.indexOf("BUY") >= 0) {
                signal = "BUY";
            } else if (fullResponse.indexOf("SELL") >= 0) {
                signal = "SELL";
            } else if (fullResponse.indexOf("HOLD") >= 0) {
                signal = "HOLD";
            } else {
                signal = "HOLD"; // Default to HOLD if unclear
            }

            Serial.printf("Trading Signal (15m-1h): %s\n", signal.c_str());
            return true;
        } else {
            sdLogger.logAPIError("gemini", "/trading-signal", httpCode, "Parse error");
            signal = "HOLD";
            return false;
        }
    } else {
        sdLogger.logAPIError("gemini", "/trading-signal", httpCode, "HTTP error");
        Serial.printf("Trading signal request failed: %d\n", httpCode);
        signal = "HOLD";
        http.end();
        return false;
    }
}
