#include <unity.h>
#include <ArduinoJson.h>

// BTCData structure (copied from BTCDashboardScreen.h for testing)
struct BTCData {
    float priceUSD = 0;
    float priceEUR = 0;
    int blockHeight = 0;
    const char* blockHash = "";
    int blockTxCount = 0;
    uint32_t blockTime = 0;
    int mempoolCount = 0;
    float mempoolSize = 0;
    int feeFast = 0;
    int feeMedium = 0;
    int feeSlow = 0;
};

// Test: Parse valid price JSON
void test_parse_valid_price_json() {
    const char* json = "{\"USD\":91396,\"EUR\":85000,\"GBP\":72000}";
    StaticJsonDocument<256> doc;
    DeserializationError error = deserializeJson(doc, json);

    TEST_ASSERT_FALSE(error);

    BTCData data;
    data.priceUSD = doc["USD"].as<float>();
    data.priceEUR = doc["EUR"].as<float>();

    TEST_ASSERT_EQUAL_FLOAT(91396.0, data.priceUSD);
    TEST_ASSERT_EQUAL_FLOAT(85000.0, data.priceEUR);
}

// Test: Parse price JSON with large values
void test_parse_price_large_values() {
    const char* json = "{\"USD\":999999,\"EUR\":888888}";
    StaticJsonDocument<256> doc;
    DeserializationError error = deserializeJson(doc, json);

    TEST_ASSERT_FALSE(error);

    BTCData data;
    data.priceUSD = doc["USD"].as<float>();
    data.priceEUR = doc["EUR"].as<float>();

    TEST_ASSERT_EQUAL_FLOAT(999999.0, data.priceUSD);
    TEST_ASSERT_EQUAL_FLOAT(888888.0, data.priceEUR);
}

// Test: Parse blocks JSON array
void test_parse_valid_blocks_json() {
    const char* json = "[{\"height\":867095,\"tx_count\":3527,\"timestamp\":1702396800,\"id\":\"00000000000000000002a23d\"}]";
    DynamicJsonDocument doc(4096);
    DeserializationError error = deserializeJson(doc, json);

    TEST_ASSERT_FALSE(error);

    JsonArray blocks = doc.as<JsonArray>();
    TEST_ASSERT_TRUE(blocks.size() > 0);

    JsonObject latestBlock = blocks[0];
    BTCData data;
    data.blockHeight = latestBlock["height"];
    data.blockTxCount = latestBlock["tx_count"];
    data.blockTime = latestBlock["timestamp"];

    TEST_ASSERT_EQUAL_INT(867095, data.blockHeight);
    TEST_ASSERT_EQUAL_INT(3527, data.blockTxCount);
    TEST_ASSERT_EQUAL_UINT32(1702396800, data.blockTime);
}

// Test: Parse empty blocks array
void test_parse_empty_blocks_array() {
    const char* json = "[]";
    DynamicJsonDocument doc(4096);
    DeserializationError error = deserializeJson(doc, json);

    TEST_ASSERT_FALSE(error);

    JsonArray blocks = doc.as<JsonArray>();
    TEST_ASSERT_EQUAL_INT(0, blocks.size());
}

// Test: Parse valid mempool JSON
void test_parse_valid_mempool_json() {
    const char* json = "{\"count\":47853,\"vsize\":85432156}";
    StaticJsonDocument<512> doc;
    DeserializationError error = deserializeJson(doc, json);

    TEST_ASSERT_FALSE(error);

    BTCData data;
    data.mempoolCount = doc["count"];
    data.mempoolSize = doc["vsize"].as<float>() / 1000000.0; // Convert to MB

    TEST_ASSERT_EQUAL_INT(47853, data.mempoolCount);
    TEST_ASSERT_FLOAT_WITHIN(0.01, 85.43, data.mempoolSize);
}

// Test: Parse mempool with zero transactions
void test_parse_mempool_zero_txs() {
    const char* json = "{\"count\":0,\"vsize\":0}";
    StaticJsonDocument<512> doc;
    DeserializationError error = deserializeJson(doc, json);

    TEST_ASSERT_FALSE(error);

    BTCData data;
    data.mempoolCount = doc["count"];
    data.mempoolSize = doc["vsize"].as<float>() / 1000000.0;

    TEST_ASSERT_EQUAL_INT(0, data.mempoolCount);
    TEST_ASSERT_EQUAL_FLOAT(0.0, data.mempoolSize);
}

// Test: Parse valid fees JSON
void test_parse_valid_fees_json() {
    const char* json = "{\"fastestFee\":25,\"halfHourFee\":18,\"hourFee\":12}";
    StaticJsonDocument<256> doc;
    DeserializationError error = deserializeJson(doc, json);

    TEST_ASSERT_FALSE(error);

    BTCData data;
    data.feeFast = doc["fastestFee"];
    data.feeMedium = doc["halfHourFee"];
    data.feeSlow = doc["hourFee"];

    TEST_ASSERT_EQUAL_INT(25, data.feeFast);
    TEST_ASSERT_EQUAL_INT(18, data.feeMedium);
    TEST_ASSERT_EQUAL_INT(12, data.feeSlow);
}

// Test: Parse fees with high values
void test_parse_fees_high_values() {
    const char* json = "{\"fastestFee\":500,\"halfHourFee\":350,\"hourFee\":200}";
    StaticJsonDocument<256> doc;
    DeserializationError error = deserializeJson(doc, json);

    TEST_ASSERT_FALSE(error);

    BTCData data;
    data.feeFast = doc["fastestFee"];
    data.feeMedium = doc["halfHourFee"];
    data.feeSlow = doc["hourFee"];

    TEST_ASSERT_EQUAL_INT(500, data.feeFast);
    TEST_ASSERT_EQUAL_INT(350, data.feeMedium);
    TEST_ASSERT_EQUAL_INT(200, data.feeSlow);
}

// Test: Handle malformed JSON gracefully
void test_handle_malformed_json() {
    const char* json = "{\"USD\":91396,\"EUR\":"; // Incomplete JSON
    StaticJsonDocument<256> doc;
    DeserializationError error = deserializeJson(doc, json);

    TEST_ASSERT_TRUE(error); // Should detect error
    TEST_ASSERT_EQUAL_INT(DeserializationError::IncompleteInput, error.code());
}

// Test: Handle missing fields
void test_handle_missing_price_field() {
    const char* json = "{\"GBP\":72000}"; // Missing USD and EUR
    StaticJsonDocument<256> doc;
    DeserializationError error = deserializeJson(doc, json);

    TEST_ASSERT_FALSE(error); // JSON is valid, just missing fields

    BTCData data;
    data.priceUSD = doc["USD"].as<float>(); // Will be 0 (default)
    data.priceEUR = doc["EUR"].as<float>();

    TEST_ASSERT_EQUAL_FLOAT(0.0, data.priceUSD);
    TEST_ASSERT_EQUAL_FLOAT(0.0, data.priceEUR);
}

// Test: Handle null values
void test_handle_null_values() {
    const char* json = "{\"USD\":null,\"EUR\":85000}";
    StaticJsonDocument<256> doc;
    DeserializationError error = deserializeJson(doc, json);

    TEST_ASSERT_FALSE(error);

    BTCData data;
    data.priceUSD = doc["USD"].as<float>(); // Will be 0
    data.priceEUR = doc["EUR"].as<float>();

    TEST_ASSERT_EQUAL_FLOAT(0.0, data.priceUSD);
    TEST_ASSERT_EQUAL_FLOAT(85000.0, data.priceEUR);
}

// Test: Handle extreme values
void test_handle_extreme_mempool_size() {
    const char* json = "{\"count\":999999,\"vsize\":9999999999}";
    StaticJsonDocument<512> doc;
    DeserializationError error = deserializeJson(doc, json);

    TEST_ASSERT_FALSE(error);

    BTCData data;
    data.mempoolCount = doc["count"];
    data.mempoolSize = doc["vsize"].as<float>() / 1000000.0;

    TEST_ASSERT_EQUAL_INT(999999, data.mempoolCount);
    TEST_ASSERT_FLOAT_WITHIN(0.01, 9999.99, data.mempoolSize);
}

void setUp(void) {
    // Set up before each test (if needed)
}

void tearDown(void) {
    // Clean up after each test (if needed)
}

int main(int argc, char **argv) {
    UNITY_BEGIN();

    // Price parsing tests
    RUN_TEST(test_parse_valid_price_json);
    RUN_TEST(test_parse_price_large_values);

    // Blocks parsing tests
    RUN_TEST(test_parse_valid_blocks_json);
    RUN_TEST(test_parse_empty_blocks_array);

    // Mempool parsing tests
    RUN_TEST(test_parse_valid_mempool_json);
    RUN_TEST(test_parse_mempool_zero_txs);

    // Fees parsing tests
    RUN_TEST(test_parse_valid_fees_json);
    RUN_TEST(test_parse_fees_high_values);

    // Error handling tests
    RUN_TEST(test_handle_malformed_json);
    RUN_TEST(test_handle_missing_price_field);
    RUN_TEST(test_handle_null_values);
    RUN_TEST(test_handle_extreme_mempool_size);

    return UNITY_END();
}
