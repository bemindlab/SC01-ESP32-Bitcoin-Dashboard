#include <Arduino.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <lvgl.h>
#include <TFT_eSPI.h>
#include <FT6X36.h>

// ==================== Configuration ====================
const char* WIFI_SSID = "Home 123_69_2.4G";
const char* WIFI_PASSWORD = "0850971423";

// API Endpoints (mempool.space)
const char* API_PRICE = "https://mempool.space/api/v1/prices";
const char* API_BLOCKS = "https://mempool.space/api/v1/blocks";
const char* API_MEMPOOL = "https://mempool.space/api/mempool";
const char* API_FEES = "https://mempool.space/api/v1/fees/recommended";

// Update intervals (ms)
const unsigned long PRICE_INTERVAL = 30000;      // 30 sec
const unsigned long BLOCK_INTERVAL = 60000;      // 1 min
const unsigned long MEMPOOL_INTERVAL = 30000;    // 30 sec

// ==================== Display Setup ====================
TFT_eSPI tft = TFT_eSPI();
FT6X36 touch(&Wire, -1);  // Wire pointer, no interrupt pin

static const uint16_t screenWidth = 480;
static const uint16_t screenHeight = 320;
static lv_disp_draw_buf_t draw_buf;
static lv_color_t buf[screenWidth * 40];

// ==================== Data Structures ====================
struct BTCData {
    float priceUSD = 0;
    float priceEUR = 0;
    int blockHeight = 0;
    String blockHash = "";
    int blockTxCount = 0;
    uint32_t blockTime = 0;
    int mempoolCount = 0;
    float mempoolSize = 0;  // in MB
    int feeFast = 0;
    int feeMedium = 0;
    int feeSlow = 0;
} btcData;

// ==================== LVGL UI Elements ====================
lv_obj_t* labelPrice;
lv_obj_t* labelBlock;
lv_obj_t* labelMempool;
lv_obj_t* labelFees;
lv_obj_t* labelStatus;

// ==================== Timers ====================
unsigned long lastPriceUpdate = 0;
unsigned long lastBlockUpdate = 0;
unsigned long lastMempoolUpdate = 0;

// ==================== Display Flush ====================
void my_disp_flush(lv_disp_drv_t* disp, const lv_area_t* area, lv_color_t* color_p) {
    uint32_t w = (area->x2 - area->x1 + 1);
    uint32_t h = (area->y2 - area->y1 + 1);
    
    tft.startWrite();
    tft.setAddrWindow(area->x1, area->y1, w, h);
    tft.pushColors((uint16_t*)&color_p->full, w * h, true);
    tft.endWrite();
    
    lv_disp_flush_ready(disp);
}

// ==================== Touch Read ====================
bool touchPressed = false;
uint16_t touchX = 0, touchY = 0;

void touchHandler(TPoint point, TEvent event) {
    if (event != TEvent::None) {
        touchPressed = true;
        touchX = point.x;
        touchY = point.y;
    } else {
        touchPressed = false;
    }
}

void my_touchpad_read(lv_indev_drv_t* indev_driver, lv_indev_data_t* data) {
    if (touchPressed) {
        data->state = LV_INDEV_STATE_PR;
        data->point.x = touchX;
        data->point.y = touchY;
    } else {
        data->state = LV_INDEV_STATE_REL;
    }
}

// ==================== API Functions ====================
bool fetchPrice() {
    HTTPClient http;
    http.begin(API_PRICE);
    http.setTimeout(10000);
    
    int httpCode = http.GET();
    if (httpCode == HTTP_CODE_OK) {
        String payload = http.getString();
        StaticJsonDocument<256> doc;
        DeserializationError error = deserializeJson(doc, payload);
        
        if (!error) {
            btcData.priceUSD = doc["USD"].as<float>();
            btcData.priceEUR = doc["EUR"].as<float>();
            http.end();
            return true;
        }
    }
    http.end();
    return false;
}

bool fetchBlocks() {
    HTTPClient http;
    http.begin(API_BLOCKS);
    http.setTimeout(10000);
    
    int httpCode = http.GET();
    if (httpCode == HTTP_CODE_OK) {
        String payload = http.getString();
        DynamicJsonDocument doc(4096);
        DeserializationError error = deserializeJson(doc, payload);
        
        if (!error && doc.is<JsonArray>() && doc.size() > 0) {
            JsonObject latestBlock = doc[0];
            btcData.blockHeight = latestBlock["height"].as<int>();
            btcData.blockHash = latestBlock["id"].as<String>();
            btcData.blockTxCount = latestBlock["tx_count"].as<int>();
            btcData.blockTime = latestBlock["timestamp"].as<uint32_t>();
            http.end();
            return true;
        }
    }
    http.end();
    return false;
}

bool fetchMempool() {
    HTTPClient http;
    http.begin(API_MEMPOOL);
    http.setTimeout(10000);
    
    int httpCode = http.GET();
    if (httpCode == HTTP_CODE_OK) {
        String payload = http.getString();
        StaticJsonDocument<512> doc;
        DeserializationError error = deserializeJson(doc, payload);
        
        if (!error) {
            btcData.mempoolCount = doc["count"].as<int>();
            btcData.mempoolSize = doc["vsize"].as<float>() / 1000000.0; // bytes to MB
            http.end();
            return true;
        }
    }
    http.end();
    return false;
}

bool fetchFees() {
    HTTPClient http;
    http.begin(API_FEES);
    http.setTimeout(10000);
    
    int httpCode = http.GET();
    if (httpCode == HTTP_CODE_OK) {
        String payload = http.getString();
        StaticJsonDocument<256> doc;
        DeserializationError error = deserializeJson(doc, payload);
        
        if (!error) {
            btcData.feeFast = doc["fastestFee"].as<int>();
            btcData.feeMedium = doc["halfHourFee"].as<int>();
            btcData.feeSlow = doc["hourFee"].as<int>();
            http.end();
            return true;
        }
    }
    http.end();
    return false;
}

// ==================== UI Setup ====================
void setupUI() {
    // Set dark theme
    lv_obj_set_style_bg_color(lv_scr_act(), lv_color_hex(0x1a1a2e), 0);
    
    // Title
    lv_obj_t* title = lv_label_create(lv_scr_act());
    lv_label_set_text(title, LV_SYMBOL_SETTINGS " Bitcoin Dashboard");
    lv_obj_set_style_text_color(title, lv_color_hex(0xf7931a), 0);
    lv_obj_set_style_text_font(title, &lv_font_montserrat_24, 0);
    lv_obj_align(title, LV_ALIGN_TOP_MID, 0, 10);
    
    // Price Card
    lv_obj_t* cardPrice = lv_obj_create(lv_scr_act());
    lv_obj_set_size(cardPrice, 220, 100);
    lv_obj_align(cardPrice, LV_ALIGN_TOP_LEFT, 10, 50);
    lv_obj_set_style_bg_color(cardPrice, lv_color_hex(0x16213e), 0);
    lv_obj_set_style_border_color(cardPrice, lv_color_hex(0xf7931a), 0);
    lv_obj_set_style_radius(cardPrice, 10, 0);
    
    lv_obj_t* priceTitle = lv_label_create(cardPrice);
    lv_label_set_text(priceTitle, "BTC Price");
    lv_obj_set_style_text_color(priceTitle, lv_color_hex(0x888888), 0);
    lv_obj_align(priceTitle, LV_ALIGN_TOP_LEFT, 5, 5);
    
    labelPrice = lv_label_create(cardPrice);
    lv_label_set_text(labelPrice, "$--,---");
    lv_obj_set_style_text_color(labelPrice, lv_color_hex(0x00ff88), 0);
    lv_obj_set_style_text_font(labelPrice, &lv_font_montserrat_28, 0);
    lv_obj_align(labelPrice, LV_ALIGN_CENTER, 0, 10);
    
    // Block Card
    lv_obj_t* cardBlock = lv_obj_create(lv_scr_act());
    lv_obj_set_size(cardBlock, 220, 100);
    lv_obj_align(cardBlock, LV_ALIGN_TOP_RIGHT, -10, 50);
    lv_obj_set_style_bg_color(cardBlock, lv_color_hex(0x16213e), 0);
    lv_obj_set_style_border_color(cardBlock, lv_color_hex(0x0f4c75), 0);
    lv_obj_set_style_radius(cardBlock, 10, 0);
    
    lv_obj_t* blockTitle = lv_label_create(cardBlock);
    lv_label_set_text(blockTitle, "Latest Block");
    lv_obj_set_style_text_color(blockTitle, lv_color_hex(0x888888), 0);
    lv_obj_align(blockTitle, LV_ALIGN_TOP_LEFT, 5, 5);
    
    labelBlock = lv_label_create(cardBlock);
    lv_label_set_text(labelBlock, "Block: ---\nTXs: ---");
    lv_obj_set_style_text_color(labelBlock, lv_color_hex(0x00d4ff), 0);
    lv_obj_align(labelBlock, LV_ALIGN_CENTER, 0, 10);
    
    // Mempool Card
    lv_obj_t* cardMempool = lv_obj_create(lv_scr_act());
    lv_obj_set_size(cardMempool, 220, 100);
    lv_obj_align(cardMempool, LV_ALIGN_BOTTOM_LEFT, 10, -40);
    lv_obj_set_style_bg_color(cardMempool, lv_color_hex(0x16213e), 0);
    lv_obj_set_style_border_color(cardMempool, lv_color_hex(0x9b59b6), 0);
    lv_obj_set_style_radius(cardMempool, 10, 0);
    
    lv_obj_t* mempoolTitle = lv_label_create(cardMempool);
    lv_label_set_text(mempoolTitle, "Mempool");
    lv_obj_set_style_text_color(mempoolTitle, lv_color_hex(0x888888), 0);
    lv_obj_align(mempoolTitle, LV_ALIGN_TOP_LEFT, 5, 5);
    
    labelMempool = lv_label_create(cardMempool);
    lv_label_set_text(labelMempool, "TXs: ---\nSize: --- MB");
    lv_obj_set_style_text_color(labelMempool, lv_color_hex(0xbb86fc), 0);
    lv_obj_align(labelMempool, LV_ALIGN_CENTER, 0, 10);
    
    // Fees Card
    lv_obj_t* cardFees = lv_obj_create(lv_scr_act());
    lv_obj_set_size(cardFees, 220, 100);
    lv_obj_align(cardFees, LV_ALIGN_BOTTOM_RIGHT, -10, -40);
    lv_obj_set_style_bg_color(cardFees, lv_color_hex(0x16213e), 0);
    lv_obj_set_style_border_color(cardFees, lv_color_hex(0xe74c3c), 0);
    lv_obj_set_style_radius(cardFees, 10, 0);
    
    lv_obj_t* feesTitle = lv_label_create(cardFees);
    lv_label_set_text(feesTitle, "Fee Rates (sat/vB)");
    lv_obj_set_style_text_color(feesTitle, lv_color_hex(0x888888), 0);
    lv_obj_align(feesTitle, LV_ALIGN_TOP_LEFT, 5, 5);
    
    labelFees = lv_label_create(cardFees);
    lv_label_set_text(labelFees, "Fast: --  Med: --  Slow: --");
    lv_obj_set_style_text_color(labelFees, lv_color_hex(0xff6b6b), 0);
    lv_obj_align(labelFees, LV_ALIGN_CENTER, 0, 10);
    
    // Status bar
    labelStatus = lv_label_create(lv_scr_act());
    lv_label_set_text(labelStatus, "Connecting...");
    lv_obj_set_style_text_color(labelStatus, lv_color_hex(0x666666), 0);
    lv_obj_align(labelStatus, LV_ALIGN_BOTTOM_MID, 0, -10);
}

// ==================== Update UI ====================
void updateUI() {
    char buf[128];
    
    // Price
    snprintf(buf, sizeof(buf), "$%,.0f", btcData.priceUSD);
    lv_label_set_text(labelPrice, buf);
    
    // Block
    snprintf(buf, sizeof(buf), "Block: %d\nTXs: %d", 
             btcData.blockHeight, btcData.blockTxCount);
    lv_label_set_text(labelBlock, buf);
    
    // Mempool
    snprintf(buf, sizeof(buf), "TXs: %d\nSize: %.1f MB", 
             btcData.mempoolCount, btcData.mempoolSize);
    lv_label_set_text(labelMempool, buf);
    
    // Fees
    snprintf(buf, sizeof(buf), "Fast: %d  Med: %d  Slow: %d", 
             btcData.feeFast, btcData.feeMedium, btcData.feeSlow);
    lv_label_set_text(labelFees, buf);
    
    // Status
    lv_label_set_text(labelStatus, "Connected | mempool.space");
}

// ==================== Setup ====================
void setup() {
    Serial.begin(115200);
    Serial.println("Bitcoin Dashboard Starting...");
    
    // Initialize display
    tft.begin();
    tft.setRotation(1);  // Landscape
    tft.fillScreen(TFT_BLACK);
    
    // Initialize touch
    Wire.begin(18, 19);  // SDA, SCL for SC01 Plus
    touch.begin();
    touch.registerTouchHandler(touchHandler);
    
    // Initialize LVGL
    lv_init();
    lv_disp_draw_buf_init(&draw_buf, buf, NULL, screenWidth * 40);
    
    // Display driver
    static lv_disp_drv_t disp_drv;
    lv_disp_drv_init(&disp_drv);
    disp_drv.hor_res = screenWidth;
    disp_drv.ver_res = screenHeight;
    disp_drv.flush_cb = my_disp_flush;
    disp_drv.draw_buf = &draw_buf;
    lv_disp_drv_register(&disp_drv);
    
    // Touch driver
    static lv_indev_drv_t indev_drv;
    lv_indev_drv_init(&indev_drv);
    indev_drv.type = LV_INDEV_TYPE_POINTER;
    indev_drv.read_cb = my_touchpad_read;
    lv_indev_drv_register(&indev_drv);
    
    // Setup UI
    setupUI();
    
    // Connect WiFi
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
    int attempts = 0;
    while (WiFi.status() != WL_CONNECTED && attempts < 30) {
        delay(500);
        Serial.print(".");
        lv_task_handler();
        attempts++;
    }
    
    if (WiFi.status() == WL_CONNECTED) {
        Serial.println("\nWiFi Connected!");
        Serial.println(WiFi.localIP());
        
        // Initial fetch
        fetchPrice();
        fetchBlocks();
        fetchMempool();
        fetchFees();
        updateUI();
    } else {
        lv_label_set_text(labelStatus, "WiFi Failed!");
    }
}

// ==================== Main Loop ====================
void loop() {
    touch.loop();  // Process touch events
    lv_task_handler();

    unsigned long now = millis();
    
    // Update price
    if (now - lastPriceUpdate >= PRICE_INTERVAL) {
        if (fetchPrice()) {
            lastPriceUpdate = now;
            updateUI();
        }
    }
    
    // Update blocks
    if (now - lastBlockUpdate >= BLOCK_INTERVAL) {
        if (fetchBlocks()) {
            lastBlockUpdate = now;
            updateUI();
        }
    }
    
    // Update mempool & fees
    if (now - lastMempoolUpdate >= MEMPOOL_INTERVAL) {
        fetchMempool();
        fetchFees();
        lastMempoolUpdate = now;
        updateUI();
    }
    
    delay(5);
}
