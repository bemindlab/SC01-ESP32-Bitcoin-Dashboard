# Testing in LVGL Simulator

Guide for testing the Bitcoin Dashboard UI using LVGL PC simulator before deploying to hardware.

## Overview

While there's no simulator for the complete Panlee SC01 Plus hardware, you can test the LVGL UI components on your PC. This speeds up development by allowing you to iterate on the UI without constantly uploading to the device.

## Important Limitations

**What the simulator CAN test:**
- ✅ LVGL UI layout and design
- ✅ Widget positioning and sizing
- ✅ Colors and themes
- ✅ Font rendering
- ✅ Touch interactions (via mouse)
- ✅ Label updates and text formatting

**What the simulator CANNOT test:**
- ❌ ESP32-S3 specific features
- ❌ WiFi connectivity
- ❌ HTTP/HTTPS requests
- ❌ ST7796 display driver
- ❌ FT6X36 touch controller
- ❌ GPIO pins
- ❌ PSRAM operations
- ❌ Actual Bitcoin API integration

**Recommendation:** Use simulator for UI development, then test full integration on real hardware.

## Setup Options

### Option 1: Visual Studio (Windows - Recommended)

**Requirements:**
- Visual Studio 2022 (version 17.13 or later)
- Windows 10/11

**Steps:**

1. **Clone LVGL PC Simulator Project**
   ```bash
   git clone https://github.com/lvgl/lv_port_pc_visual_studio.git
   cd lv_port_pc_visual_studio
   ```

2. **Open in Visual Studio**
   - Open `LVGL.Simulator.sln`
   - Visual Studio will automatically configure dependencies

3. **Copy Your UI Code**
   - Copy `src/main.cpp` UI functions to the simulator project
   - Extract only the LVGL-related code (setupUI, updateUI)
   - Remove ESP32-specific includes

4. **Create Test Data**
   - Mock the btcData structure
   - Simulate data updates with hardcoded values

5. **Build and Run**
   - Press F5 to build and run
   - UI will appear in a desktop window

### Option 2: VSCode (Cross-platform)

**Requirements:**
- VSCode
- CMake
- SDL2
- C/C++ compiler (GCC/Clang)

**Steps:**

1. **Install SDL2**

   **Linux:**
   ```bash
   sudo apt-get update
   sudo apt-get install libsdl2-dev
   ```

   **macOS:**
   ```bash
   brew install sdl2
   ```

   **Windows (MSYS2):**
   ```bash
   pacman -S mingw-w64-x86_64-SDL2
   ```

2. **Clone LVGL VSCode Project**
   ```bash
   git clone https://github.com/lvgl/lv_port_pc_vscode.git
   cd lv_port_pc_vscode
   ```

3. **Open in VSCode**
   ```bash
   code .
   ```

4. **Install VSCode Extensions**
   - C/C++
   - CMake Tools

5. **Configure and Build**
   - Press `Ctrl+Shift+P` → "CMake: Configure"
   - Press `F7` to build
   - Press `F5` to run

### Option 3: Eclipse (Linux/Mac)

**Requirements:**
- Eclipse CDT
- SDL2
- GCC

**Steps:**

1. **Install SDL2** (see Option 2)

2. **Clone Eclipse Project**
   ```bash
   git clone https://github.com/lvgl/lv_port_pc_eclipse.git
   cd lv_port_pc_eclipse
   ```

3. **Import to Eclipse**
   - File → Import → Existing Projects
   - Select the cloned directory
   - Build and run

## Adapting Your Code for Simulator

### Step 1: Create Simulator Branch

```bash
cd /path/to/btc-dashboard
git checkout -b simulator-testing
```

### Step 2: Extract UI Code

Create a simulator-compatible version of your UI:

**File: `simulator/btc_ui.c`**
```c
#include "lvgl/lvgl.h"

// Data structure (same as main.cpp)
struct BTCData {
    float priceUSD;
    float priceEUR;
    int blockHeight;
    char blockHash[65];
    int blockTxCount;
    uint32_t blockTime;
    int mempoolCount;
    float mempoolSize;
    int feeFast;
    int feeMedium;
    int feeSlow;
} btcData;

// LVGL UI Elements (same as main.cpp)
lv_obj_t* labelPrice;
lv_obj_t* labelBlock;
lv_obj_t* labelMempool;
lv_obj_t* labelFees;
lv_obj_t* labelStatus;

// setupUI() - copied from main.cpp:178-267
void setupUI(void) {
    // Set dark theme
    lv_obj_set_style_bg_color(lv_scr_act(), lv_color_hex(0x1a1a2e), 0);

    // Title
    lv_obj_t* title = lv_label_create(lv_scr_act());
    lv_label_set_text(title, LV_SYMBOL_SETTINGS " Bitcoin Dashboard");
    lv_obj_set_style_text_color(title, lv_color_hex(0xf7931a), 0);
    lv_obj_set_style_text_font(title, &lv_font_montserrat_24, 0);
    lv_obj_align(title, LV_ALIGN_TOP_MID, 0, 10);

    // ... rest of UI setup code from main.cpp ...
}

// updateUI() - copied from main.cpp:270-294
void updateUI(void) {
    char buf[128];

    // Price
    snprintf(buf, sizeof(buf), "$%,.0f", btcData.priceUSD);
    lv_label_set_text(labelPrice, buf);

    // ... rest of update code from main.cpp ...
}

// Simulate data updates
void simulateDataUpdate(void) {
    static int counter = 0;

    // Simulate price changes
    btcData.priceUSD = 45000 + (counter * 100);
    btcData.priceEUR = 42000 + (counter * 90);

    // Simulate block info
    btcData.blockHeight = 820000 + counter;
    btcData.blockTxCount = 2500 + (counter % 1000);

    // Simulate mempool
    btcData.mempoolCount = 150000 + (counter * 100);
    btcData.mempoolSize = 250.5 + (counter * 0.5);

    // Simulate fees
    btcData.feeFast = 50 + (counter % 30);
    btcData.feeMedium = 30 + (counter % 20);
    btcData.feeSlow = 10 + (counter % 10);

    counter++;
    updateUI();
}
```

### Step 3: Integrate with Simulator

**In simulator's `main.c`:**
```c
#include "lvgl/lvgl.h"
#include "btc_ui.c"  // Your UI code

int main(void) {
    // Initialize LVGL
    lv_init();

    // ... SDL2 setup code (provided by simulator template) ...

    // Setup your UI
    setupUI();

    // Create timer for data simulation
    lv_timer_t* timer = lv_timer_create(simulateDataUpdate, 2000, NULL);

    // Main loop
    while(1) {
        lv_timer_handler();
        lv_tick_inc(5);
        SDL_Delay(5);
    }

    return 0;
}
```

## Testing Workflow

### 1. UI Layout Testing

Test different screen sizes:
```c
// In simulator, you can easily change resolution
#define HOR_RES 480
#define VER_RES 320
```

Verify:
- Card positions and sizes
- Text alignment
- Color scheme
- Font readability
- Touch target sizes

### 2. Dynamic Data Testing

Create test scenarios:
```c
// Test extreme values
btcData.priceUSD = 999999.99;  // Max value
btcData.mempoolCount = 0;       // Empty mempool
btcData.feeFast = 9999;         // High fees

updateUI();
```

### 3. Animation Testing

Test smooth transitions:
```c
// Gradually change price to test number formatting
for (int i = 0; i < 100; i++) {
    btcData.priceUSD = 40000 + i * 100;
    updateUI();
    lv_timer_handler();
    SDL_Delay(50);
}
```

### 4. Touch Interaction Testing

Add touch event handlers:
```c
static void price_card_clicked(lv_event_t* e) {
    printf("Price card clicked!\n");
    // Test touch functionality
}

// In setupUI()
lv_obj_add_event_cb(cardPrice, price_card_clicked, LV_EVENT_CLICKED, NULL);
```

## Debugging in Simulator

### Enable LVGL Logging

In `lv_conf.h`:
```c
#define LV_USE_LOG 1
#define LV_LOG_LEVEL LV_LOG_LEVEL_TRACE
```

### Memory Monitoring

```c
void print_memory_info(void) {
    lv_mem_monitor_t mon;
    lv_mem_monitor(&mon);
    printf("Memory used: %d%% (%d/%d)\n",
           mon.used_pct, mon.used_size, mon.total_size);
}
```

### Performance Profiling

```c
void measure_render_time(void) {
    uint32_t start = lv_tick_get();
    lv_refr_now(NULL);  // Force refresh
    uint32_t duration = lv_tick_get() - start;
    printf("Render time: %d ms\n", duration);
}
```

## Common Issues

### Issue 1: Fonts Not Found

**Problem:** Montserrat fonts missing in simulator

**Solution:** Copy font definitions or use default fonts:
```c
lv_obj_set_style_text_font(title, &lv_font_montserrat_24, 0);
// If not available, use:
// lv_obj_set_style_text_font(title, LV_FONT_DEFAULT, 0);
```

### Issue 2: Colors Look Different

**Problem:** Color depth mismatch

**Solution:** Match `lv_conf.h` settings:
```c
#define LV_COLOR_DEPTH 16
#define LV_COLOR_16_SWAP 1  // Match your hardware
```

### Issue 3: Touch Events Not Working

**Problem:** Mouse clicks not registering

**Solution:** Ensure SDL2 input is configured:
```c
// Simulator should handle this, but verify in main loop
SDL_Event event;
while(SDL_PollEvent(&event)) {
    // Handle events
}
```

## Transitioning to Hardware

After simulator testing:

1. **Copy tested UI code back to `src/main.cpp`**
2. **Re-add ESP32-specific code:**
   - WiFi initialization
   - HTTP requests
   - Hardware drivers
   - Serial debugging

3. **Build and upload:**
   ```bash
   pio run --target upload
   ```

4. **Verify on hardware:**
   ```bash
   pio device monitor
   ```

## Example: Complete Simulator Project Structure

```
btc-dashboard-simulator/
├── CMakeLists.txt
├── lv_conf.h              # LVGL configuration
├── main.c                 # Simulator entry point
├── btc_ui.c               # Your UI code (extracted from main.cpp)
├── btc_ui.h               # UI function declarations
└── README.md              # Simulator-specific notes
```

## Resources

### Official LVGL Simulators
- [Visual Studio Project](https://github.com/lvgl/lv_port_pc_visual_studio) - Windows
- [VSCode Project](https://github.com/lvgl/lv_port_pc_vscode) - Cross-platform
- [Eclipse Project](https://github.com/lvgl/lv_port_pc_eclipse) - Linux/Mac

### Documentation
- [LVGL PC Simulator Docs](https://docs.lvgl.io/master/details/integration/ide/pc-simulator.html)
- [Setting Up LVGL on Windows](https://www.dmcinfo.com/blog/15846/setting-up-a-light-and-versatile-graphics-library-lvgl-simulator-on-your-windows-pc-using-msys2/)

## Best Practices

1. **Keep UI code separate** - Makes simulator integration easier
2. **Use mock data structures** - Same format as real data
3. **Test edge cases** - Extreme values, empty data, long strings
4. **Match hardware settings** - Color depth, resolution, fonts
5. **Iterate quickly** - Simulator allows rapid UI changes
6. **Always verify on hardware** - Final testing must be on real device

## Limitations Summary

The simulator is **excellent for**:
- UI design and layout
- Color and theme testing
- Widget behavior
- Basic touch interactions

The simulator **cannot replace**:
- Hardware integration testing
- Network connectivity testing
- Performance on embedded system
- Display driver specifics
- Power consumption analysis

**Use both:** Simulator for rapid UI development, hardware for complete testing.
