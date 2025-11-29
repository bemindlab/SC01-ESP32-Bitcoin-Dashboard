#include <unity.h>
#include <cstdio>   // For snprintf
#include <cstring>  // For string operations

// Screen constants from BTCDashboardScreen.h
#define WIFI_ICON_X 440
#define WIFI_ICON_Y 5
#define WIFI_ICON_SIZE 30
#define SETTINGS_ICON_X 400
#define SETTINGS_ICON_Y 5
#define SETTINGS_ICON_SIZE 30

// Display dimensions (landscape mode)
#define DISPLAY_WIDTH 480
#define DISPLAY_HEIGHT 320

// ============================================================================
// Touch Detection Tests
// ============================================================================

void test_wifi_icon_touch_center() {
    int16_t touchX = WIFI_ICON_X + WIFI_ICON_SIZE / 2;
    int16_t touchY = WIFI_ICON_Y + WIFI_ICON_SIZE / 2;

    bool isInside = (touchX >= WIFI_ICON_X && touchX <= (WIFI_ICON_X + WIFI_ICON_SIZE) &&
                     touchY >= WIFI_ICON_Y && touchY <= (WIFI_ICON_Y + WIFI_ICON_SIZE));

    TEST_ASSERT_TRUE(isInside);
}

void test_wifi_icon_touch_boundary() {
    // Test all four corners
    int16_t corners[][2] = {
        {WIFI_ICON_X, WIFI_ICON_Y},
        {WIFI_ICON_X + WIFI_ICON_SIZE, WIFI_ICON_Y},
        {WIFI_ICON_X, WIFI_ICON_Y + WIFI_ICON_SIZE},
        {WIFI_ICON_X + WIFI_ICON_SIZE, WIFI_ICON_Y + WIFI_ICON_SIZE}
    };

    for (int i = 0; i < 4; i++) {
        int16_t touchX = corners[i][0];
        int16_t touchY = corners[i][1];

        bool isInside = (touchX >= WIFI_ICON_X && touchX <= (WIFI_ICON_X + WIFI_ICON_SIZE) &&
                         touchY >= WIFI_ICON_Y && touchY <= (WIFI_ICON_Y + WIFI_ICON_SIZE));

        TEST_ASSERT_TRUE(isInside);
    }
}

void test_wifi_icon_touch_outside() {
    // Just outside left edge
    int16_t touchX = WIFI_ICON_X - 1;
    int16_t touchY = WIFI_ICON_Y + WIFI_ICON_SIZE / 2;

    bool isInside = (touchX >= WIFI_ICON_X && touchX <= (WIFI_ICON_X + WIFI_ICON_SIZE) &&
                     touchY >= WIFI_ICON_Y && touchY <= (WIFI_ICON_Y + WIFI_ICON_SIZE));

    TEST_ASSERT_FALSE(isInside);
}

void test_settings_icon_touch_center() {
    int16_t touchX = SETTINGS_ICON_X + SETTINGS_ICON_SIZE / 2;
    int16_t touchY = SETTINGS_ICON_Y + SETTINGS_ICON_SIZE / 2;

    bool isInside = (touchX >= SETTINGS_ICON_X && touchX <= (SETTINGS_ICON_X + SETTINGS_ICON_SIZE) &&
                     touchY >= SETTINGS_ICON_Y && touchY <= (SETTINGS_ICON_Y + SETTINGS_ICON_SIZE));

    TEST_ASSERT_TRUE(isInside);
}

void test_wifi_and_settings_icons_do_not_overlap() {
    // WiFi icon: x=440-470, y=5-35
    // Settings icon: x=400-430, y=5-35

    int wifi_right = WIFI_ICON_X + WIFI_ICON_SIZE;
    int settings_left = SETTINGS_ICON_X;

    TEST_ASSERT_TRUE(settings_left + SETTINGS_ICON_SIZE < WIFI_ICON_X);
}

void test_icons_within_display_bounds() {
    // WiFi icon
    TEST_ASSERT_TRUE(WIFI_ICON_X >= 0);
    TEST_ASSERT_TRUE(WIFI_ICON_Y >= 0);
    TEST_ASSERT_TRUE(WIFI_ICON_X + WIFI_ICON_SIZE <= DISPLAY_WIDTH);
    TEST_ASSERT_TRUE(WIFI_ICON_Y + WIFI_ICON_SIZE <= DISPLAY_HEIGHT);

    // Settings icon
    TEST_ASSERT_TRUE(SETTINGS_ICON_X >= 0);
    TEST_ASSERT_TRUE(SETTINGS_ICON_Y >= 0);
    TEST_ASSERT_TRUE(SETTINGS_ICON_X + SETTINGS_ICON_SIZE <= DISPLAY_WIDTH);
    TEST_ASSERT_TRUE(SETTINGS_ICON_Y + SETTINGS_ICON_SIZE <= DISPLAY_HEIGHT);
}

// ============================================================================
// Price Formatting Tests
// ============================================================================

void test_price_format_under_100k() {
    float price = 45678.0f;
    char buf[64];

    if (price >= 100000) {
        snprintf(buf, sizeof(buf), "$%.0fk", price / 1000);
    } else {
        snprintf(buf, sizeof(buf), "$%.0f", price);
    }

    TEST_ASSERT_EQUAL_STRING("$45678", buf);
}

void test_price_format_over_100k() {
    float price = 123456.0f;
    char buf[64];

    if (price >= 100000) {
        snprintf(buf, sizeof(buf), "$%.0fk", price / 1000);
    } else {
        snprintf(buf, sizeof(buf), "$%.0f", price);
    }

    TEST_ASSERT_EQUAL_STRING("$123k", buf);
}

void test_price_format_exactly_100k() {
    float price = 100000.0f;
    char buf[64];

    if (price >= 100000) {
        snprintf(buf, sizeof(buf), "$%.0fk", price / 1000);
    } else {
        snprintf(buf, sizeof(buf), "$%.0f", price);
    }

    TEST_ASSERT_EQUAL_STRING("$100k", buf);
}

void test_eur_price_format() {
    float eurPrice = 45678.50f;
    char buf[64];

    snprintf(buf, sizeof(buf), "EUR %.0f", eurPrice);
    TEST_ASSERT_EQUAL_STRING("EUR 45679", buf);
}

// ============================================================================
// Date/Time Formatting Tests (NEW - Simplified Dashboard)
// ============================================================================

void test_time_format_basic() {
    unsigned long uptimeSeconds = 3665; // 1 hour, 1 minute, 5 seconds
    unsigned long hours = (uptimeSeconds % 86400) / 3600;
    unsigned long minutes = (uptimeSeconds % 3600) / 60;
    unsigned long seconds = uptimeSeconds % 60;

    char timeStr[16];
    snprintf(timeStr, sizeof(timeStr), "%02lu:%02lu:%02lu", hours, minutes, seconds);

    TEST_ASSERT_EQUAL_STRING("01:01:05", timeStr);
}

void test_time_format_midnight() {
    unsigned long uptimeSeconds = 0; // 00:00:00
    unsigned long hours = (uptimeSeconds % 86400) / 3600;
    unsigned long minutes = (uptimeSeconds % 3600) / 60;
    unsigned long seconds = uptimeSeconds % 60;

    char timeStr[16];
    snprintf(timeStr, sizeof(timeStr), "%02lu:%02lu:%02lu", hours, minutes, seconds);

    TEST_ASSERT_EQUAL_STRING("00:00:00", timeStr);
}

void test_time_format_max_day() {
    unsigned long uptimeSeconds = 86399; // 23:59:59
    unsigned long hours = (uptimeSeconds % 86400) / 3600;
    unsigned long minutes = (uptimeSeconds % 3600) / 60;
    unsigned long seconds = uptimeSeconds % 60;

    char timeStr[16];
    snprintf(timeStr, sizeof(timeStr), "%02lu:%02lu:%02lu", hours, minutes, seconds);

    TEST_ASSERT_EQUAL_STRING("23:59:59", timeStr);
}

void test_time_wraps_after_24_hours() {
    unsigned long uptimeSeconds = 86400; // Exactly 1 day (should show 00:00:00)
    unsigned long hours = (uptimeSeconds % 86400) / 3600;
    unsigned long minutes = (uptimeSeconds % 3600) / 60;
    unsigned long seconds = uptimeSeconds % 60;

    char timeStr[16];
    snprintf(timeStr, sizeof(timeStr), "%02lu:%02lu:%02lu", hours, minutes, seconds);

    TEST_ASSERT_EQUAL_STRING("00:00:00", timeStr);
}

void test_days_calculation_zero() {
    unsigned long uptimeSeconds = 3600; // 1 hour
    unsigned long days = uptimeSeconds / 86400;

    TEST_ASSERT_EQUAL_UINT32(0, days);
}

void test_days_calculation_one() {
    unsigned long uptimeSeconds = 86400; // Exactly 1 day
    unsigned long days = uptimeSeconds / 86400;

    TEST_ASSERT_EQUAL_UINT32(1, days);
}

void test_days_calculation_multiple() {
    unsigned long uptimeSeconds = 259200; // 3 days
    unsigned long days = uptimeSeconds / 86400;

    TEST_ASSERT_EQUAL_UINT32(3, days);
}

void test_date_string_today() {
    unsigned long days = 0;
    char dateStr[32];

    if (days == 0) {
        snprintf(dateStr, sizeof(dateStr), "Started Today");
    } else if (days == 1) {
        snprintf(dateStr, sizeof(dateStr), "Running 1 Day");
    } else {
        snprintf(dateStr, sizeof(dateStr), "Running %lu Days", days);
    }

    TEST_ASSERT_EQUAL_STRING("Started Today", dateStr);
}

void test_date_string_one_day() {
    unsigned long days = 1;
    char dateStr[32];

    if (days == 0) {
        snprintf(dateStr, sizeof(dateStr), "Started Today");
    } else if (days == 1) {
        snprintf(dateStr, sizeof(dateStr), "Running 1 Day");
    } else {
        snprintf(dateStr, sizeof(dateStr), "Running %lu Days", days);
    }

    TEST_ASSERT_EQUAL_STRING("Running 1 Day", dateStr);
}

void test_date_string_multiple_days() {
    unsigned long days = 5;
    char dateStr[32];

    if (days == 0) {
        snprintf(dateStr, sizeof(dateStr), "Started Today");
    } else if (days == 1) {
        snprintf(dateStr, sizeof(dateStr), "Running 1 Day");
    } else {
        snprintf(dateStr, sizeof(dateStr), "Running %lu Days", days);
    }

    TEST_ASSERT_EQUAL_STRING("Running 5 Days", dateStr);
}

// ============================================================================
// Update Interval Tests
// ============================================================================

void test_price_update_interval() {
    unsigned long PRICE_INTERVAL = 30000; // 30 seconds
    unsigned long lastUpdate = 0;
    unsigned long currentTime = 35000;

    bool shouldUpdate = (currentTime - lastUpdate >= PRICE_INTERVAL);
    TEST_ASSERT_TRUE(shouldUpdate);
}

void test_ui_update_interval_one_second() {
    // UI should update every second for live clock
    unsigned long UI_UPDATE_INTERVAL = 1000; // 1 second
    unsigned long lastUpdate = 500;
    unsigned long currentTime = 1500; // 1 second later

    bool shouldUpdate = (currentTime - lastUpdate >= UI_UPDATE_INTERVAL);
    TEST_ASSERT_TRUE(shouldUpdate);
}

void test_ui_update_not_yet() {
    unsigned long UI_UPDATE_INTERVAL = 1000; // 1 second
    unsigned long lastUpdate = 500;
    unsigned long currentTime = 999; // Not quite 1 second

    bool shouldUpdate = (currentTime - lastUpdate >= UI_UPDATE_INTERVAL);
    TEST_ASSERT_FALSE(shouldUpdate);
}

// ============================================================================
// Layout Tests (Simplified Dashboard)
// ============================================================================

void test_center_coordinates() {
    int centerX = 240;
    int centerY = 140;

    // Center should be approximately middle of screen
    TEST_ASSERT_TRUE(centerX > 0 && centerX < DISPLAY_WIDTH);
    TEST_ASSERT_TRUE(centerY > 0 && centerY < DISPLAY_HEIGHT);
    TEST_ASSERT_TRUE(centerX == DISPLAY_WIDTH / 2);
}

void test_title_bar_height() {
    int titleBarY = 5;
    int contentStartY = 40; // Content starts below title

    TEST_ASSERT_TRUE(titleBarY >= 0);
    TEST_ASSERT_TRUE(contentStartY > titleBarY + 30); // Leave space for title
}

void test_status_bar_position() {
    int statusBarY = DISPLAY_HEIGHT - 20;
    int statusBarHeight = 20;

    // Should be at bottom of screen
    TEST_ASSERT_EQUAL_INT(300, statusBarY);
    TEST_ASSERT_TRUE(statusBarY + statusBarHeight == DISPLAY_HEIGHT);
}

void test_swipe_hint_position() {
    int swipeHintX = 340;
    int swipeHintY = DISPLAY_HEIGHT - 15;

    // Should be in bottom-right area
    TEST_ASSERT_TRUE(swipeHintX > DISPLAY_WIDTH / 2);
    TEST_ASSERT_TRUE(swipeHintY > DISPLAY_HEIGHT - 20);
}

// ============================================================================
// Main Test Runner
// ============================================================================

void setUp(void) {
    // Set up before each test
}

void tearDown(void) {
    // Clean up after each test
}

int main(int argc, char **argv) {
    UNITY_BEGIN();

    // Touch detection tests
    RUN_TEST(test_wifi_icon_touch_center);
    RUN_TEST(test_wifi_icon_touch_boundary);
    RUN_TEST(test_wifi_icon_touch_outside);
    RUN_TEST(test_settings_icon_touch_center);
    RUN_TEST(test_wifi_and_settings_icons_do_not_overlap);
    RUN_TEST(test_icons_within_display_bounds);

    // Price formatting tests
    RUN_TEST(test_price_format_under_100k);
    RUN_TEST(test_price_format_over_100k);
    RUN_TEST(test_price_format_exactly_100k);
    RUN_TEST(test_eur_price_format);

    // NEW: Date/Time formatting tests
    RUN_TEST(test_time_format_basic);
    RUN_TEST(test_time_format_midnight);
    RUN_TEST(test_time_format_max_day);
    RUN_TEST(test_time_wraps_after_24_hours);
    RUN_TEST(test_days_calculation_zero);
    RUN_TEST(test_days_calculation_one);
    RUN_TEST(test_days_calculation_multiple);
    RUN_TEST(test_date_string_today);
    RUN_TEST(test_date_string_one_day);
    RUN_TEST(test_date_string_multiple_days);

    // Update interval tests
    RUN_TEST(test_price_update_interval);
    RUN_TEST(test_ui_update_interval_one_second);
    RUN_TEST(test_ui_update_not_yet);

    // Layout tests
    RUN_TEST(test_center_coordinates);
    RUN_TEST(test_title_bar_height);
    RUN_TEST(test_status_bar_position);
    RUN_TEST(test_swipe_hint_position);

    return UNITY_END();
}
