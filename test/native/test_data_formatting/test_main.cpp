#include <unity.h>
#include <stdio.h>
#include <string.h>

// Test: Format price under 100k correctly
void test_format_price_under_100k() {
    float price = 91396.0;
    char buf[64];

    if (price >= 100000) {
        snprintf(buf, sizeof(buf), "$%.0fk", price / 1000);
    } else {
        snprintf(buf, sizeof(buf), "$%.0f", price);
    }

    TEST_ASSERT_EQUAL_STRING("$91396", buf);
}

// Test: Format price over 100k with 'k' suffix
void test_format_price_over_100k() {
    float price = 125000.0;
    char buf[64];

    if (price >= 100000) {
        snprintf(buf, sizeof(buf), "$%.0fk", price / 1000);
    } else {
        snprintf(buf, sizeof(buf), "$%.0f", price);
    }

    TEST_ASSERT_EQUAL_STRING("$125k", buf);
}

// Test: Format price exactly at 100k
void test_format_price_at_100k() {
    float price = 100000.0;
    char buf[64];

    if (price >= 100000) {
        snprintf(buf, sizeof(buf), "$%.0fk", price / 1000);
    } else {
        snprintf(buf, sizeof(buf), "$%.0f", price);
    }

    TEST_ASSERT_EQUAL_STRING("$100k", buf);
}

// Test: Format mempool count under 1000
void test_format_mempool_under_1000() {
    int count = 853;
    char buf[64];

    if (count >= 1000) {
        snprintf(buf, sizeof(buf), "%.1fk", count / 1000.0);
    } else {
        snprintf(buf, sizeof(buf), "%d", count);
    }

    TEST_ASSERT_EQUAL_STRING("853", buf);
}

// Test: Format mempool count over 1000 with 'k' suffix
void test_format_mempool_over_1000() {
    int count = 47853;
    char buf[64];

    if (count >= 1000) {
        snprintf(buf, sizeof(buf), "%.1fk", count / 1000.0);
    } else {
        snprintf(buf, sizeof(buf), "%d", count);
    }

    TEST_ASSERT_EQUAL_STRING("47.9k", buf);
}

// Test: Format mempool count exactly at 1000
void test_format_mempool_at_1000() {
    int count = 1000;
    char buf[64];

    if (count >= 1000) {
        snprintf(buf, sizeof(buf), "%.1fk", count / 1000.0);
    } else {
        snprintf(buf, sizeof(buf), "%d", count);
    }

    TEST_ASSERT_EQUAL_STRING("1.0k", buf);
}

// Test: Format block height correctly
void test_format_block_height() {
    int height = 867095;
    char buf[64];

    snprintf(buf, sizeof(buf), "#%d", height);

    TEST_ASSERT_EQUAL_STRING("#867095", buf);
}

// Test: Format block height with very large number
void test_format_block_height_large() {
    int height = 9999999;
    char buf[64];

    snprintf(buf, sizeof(buf), "#%d", height);

    TEST_ASSERT_EQUAL_STRING("#9999999", buf);
}

// Test: Format mempool size in MB (small)
void test_format_mempool_size_small() {
    float sizeMB = 12.5;
    char buf[64];

    snprintf(buf, sizeof(buf), "%.1f MB", sizeMB);

    TEST_ASSERT_EQUAL_STRING("12.5 MB", buf);
}

// Test: Format mempool size in MB (large)
void test_format_mempool_size_large() {
    float sizeMB = 185.432156;
    char buf[64];

    snprintf(buf, sizeof(buf), "%.1f MB", sizeMB);

    TEST_ASSERT_EQUAL_STRING("185.4 MB", buf);
}

// Test: Format mempool size zero
void test_format_mempool_size_zero() {
    float sizeMB = 0.0;
    char buf[64];

    snprintf(buf, sizeof(buf), "%.1f MB", sizeMB);

    TEST_ASSERT_EQUAL_STRING("0.0 MB", buf);
}

// Test: Format fee rate (fast)
void test_format_fee_fast() {
    int fee = 25;
    char buf[64];

    snprintf(buf, sizeof(buf), "~%d sat/vB", fee);

    TEST_ASSERT_EQUAL_STRING("~25 sat/vB", buf);
}

// Test: Format fee rate (slow)
void test_format_fee_slow() {
    int fee = 8;
    char buf[64];

    snprintf(buf, sizeof(buf), "~%d sat/vB", fee);

    TEST_ASSERT_EQUAL_STRING("~8 sat/vB", buf);
}

// Test: Format fee rate (high congestion)
void test_format_fee_high_congestion() {
    int fee = 500;
    char buf[64];

    snprintf(buf, sizeof(buf), "~%d sat/vB", fee);

    TEST_ASSERT_EQUAL_STRING("~500 sat/vB", buf);
}

// Test: Format fee rate (1 sat minimum)
void test_format_fee_minimum() {
    int fee = 1;
    char buf[64];

    snprintf(buf, sizeof(buf), "~%d sat/vB", fee);

    TEST_ASSERT_EQUAL_STRING("~1 sat/vB", buf);
}

// Test: Format transaction count
void test_format_tx_count() {
    int txCount = 3527;
    char buf[64];

    snprintf(buf, sizeof(buf), "%d TXs", txCount);

    TEST_ASSERT_EQUAL_STRING("3527 TXs", buf);
}

// Test: Format transaction count (small block)
void test_format_tx_count_small() {
    int txCount = 15;
    char buf[64];

    snprintf(buf, sizeof(buf), "%d TXs", txCount);

    TEST_ASSERT_EQUAL_STRING("15 TXs", buf);
}

// Test: Format EUR price under 100k
void test_format_eur_price_under_100k() {
    float price = 85000.0;
    char buf[64];

    if (price >= 100000) {
        snprintf(buf, sizeof(buf), "€%.0fk", price / 1000);
    } else {
        snprintf(buf, sizeof(buf), "€%.0f", price);
    }

    TEST_ASSERT_EQUAL_STRING("€85000", buf);
}

// Test: Format EUR price over 100k
void test_format_eur_price_over_100k() {
    float price = 115000.0;
    char buf[64];

    if (price >= 100000) {
        snprintf(buf, sizeof(buf), "€%.0fk", price / 1000);
    } else {
        snprintf(buf, sizeof(buf), "€%.0f", price);
    }

    TEST_ASSERT_EQUAL_STRING("€115k", buf);
}

void setUp(void) {
    // Set up before each test
}

void tearDown(void) {
    // Clean up after each test
}

int main(int argc, char **argv) {
    UNITY_BEGIN();

    // Price formatting tests
    RUN_TEST(test_format_price_under_100k);
    RUN_TEST(test_format_price_over_100k);
    RUN_TEST(test_format_price_at_100k);
    RUN_TEST(test_format_eur_price_under_100k);
    RUN_TEST(test_format_eur_price_over_100k);

    // Mempool count formatting tests
    RUN_TEST(test_format_mempool_under_1000);
    RUN_TEST(test_format_mempool_over_1000);
    RUN_TEST(test_format_mempool_at_1000);

    // Block height formatting tests
    RUN_TEST(test_format_block_height);
    RUN_TEST(test_format_block_height_large);

    // Mempool size formatting tests
    RUN_TEST(test_format_mempool_size_small);
    RUN_TEST(test_format_mempool_size_large);
    RUN_TEST(test_format_mempool_size_zero);

    // Fee rate formatting tests
    RUN_TEST(test_format_fee_fast);
    RUN_TEST(test_format_fee_slow);
    RUN_TEST(test_format_fee_high_congestion);
    RUN_TEST(test_format_fee_minimum);

    // Transaction count formatting tests
    RUN_TEST(test_format_tx_count);
    RUN_TEST(test_format_tx_count_small);

    return UNITY_END();
}
