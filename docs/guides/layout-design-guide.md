# Layout Design Guide

Quick reference for designing UI layouts optimized for the 480x320 display.

## Display Constraints

```
Total Resolution: 480x320 pixels (landscape)
Usable Area: 480x292 pixels (excluding 28px header)
Font: FreeSans/FreeSansBold 9pt (primary)
```

## Standard Layout Components

### Header (28-30px)

```cpp
// Standard header pattern
lcd->fillRect(0, 0, 480, 28, 0x1A1A1A);
lcd->drawFastHLine(0, 27, 480, 0x2A2A2A);  // Shadow

// Title
lcd->setFont(&fonts::FreeSansBold9pt7b);
lcd->setCursor(50, 18);
lcd->print("Screen Name");

// Accent line
lcd->fillRect(0, 28, 480, 1, COLOR_BTC_ORANGE);
```

### Status Cards (50px height)

```cpp
// Two-column card layout
drawStatusCard(6, y, 232, 50, "Label", value, color);      // Left
drawStatusCard(244, y, 230, 50, "Label", value, color);    // Right

// Card implementation
void drawStatusCard(int x, int y, int w, int h,
                    const char* label, const String& value, uint32_t color) {
    // Shadow
    lcd->fillRoundRect(x + 2, y + 2, w, h, 10, 0x050505);
    // Background
    lcd->fillRoundRect(x, y, w, h, 10, 0x0A0A0A);
    // Borders
    lcd->drawRoundRect(x, y, w, h, 10, 0x2A2A2A);
    lcd->drawRoundRect(x + 1, y + 1, w - 2, h - 2, 9, 0x1A1A1A);

    // Label (top)
    lcd->setFont(&fonts::FreeSans9pt7b);
    lcd->setCursor(x + 10, y + 16);
    lcd->print(label);

    // Value (bottom)
    lcd->setFont(&fonts::FreeSansBold9pt7b);
    lcd->setCursor(x + 10, y + 36);
    lcd->print(value);
}
```

### Scrollable Content

```cpp
// Content area calculation
int headerHeight = 30;
int cardTopMargin = 8;
int cardY = headerHeight + cardTopMargin;  // 38
int cardH = 268;  // 320 - 38 - 14 (bottom margin)

// Scrolling parameters
int lineHeight = 18;
int y = cardY + 18 - scrollOffset;  // Start with top padding

// Render text
for (each line) {
    if (y >= cardY && y < cardY + cardH) {
        lcd->setCursor(x, y);
        lcd->print(line);
    }
    y += lineHeight;
}

// Scroll calculation
int visibleHeight = cardH - 36;  // Card height minus padding
int totalHeight = lineCount * lineHeight;
maxScrollOffset = max(0, totalHeight - visibleHeight);
```

### Touch Buttons

```cpp
// Button constants
static const int BACK_BTN_X = 10;
static const int BACK_BTN_Y = 4;    // For 28-30px header
static const int BACK_BTN_SIZE = 30;

static const int REFRESH_BTN_X = 420;
static const int REFRESH_BTN_Y = 4;
static const int REFRESH_BTN_SIZE = 30;

// Touch detection
if (x >= BACK_BTN_X && x <= BACK_BTN_X + BACK_BTN_SIZE &&
    y >= BACK_BTN_Y && y <= BACK_BTN_Y + BACK_BTN_SIZE) {
    // Button tapped
}
```

## Spacing Standards

### Vertical Spacing
```
Header:              28-30px
Header to content:   6-8px
Card to card:        6px
Card internal:       10-12px padding
Row spacing:         56px (card + spacing)
Line height:         18px (body text)
Bottom margin:       14-20px
```

### Horizontal Spacing
```
Screen edges:        6-8px margin
Two-column cards:    12px gap (244 - 232)
Card padding:        10-12px internal
Text indentation:    4-6px from card edge
```

## Layout Templates

### Dashboard Layout (Centered Content)
```
┌─────────────────────────────┐ 0px
│ Header (28px)               │
├─────────────────────────────┤ 28px
│                             │
│  Centered Time Display      │ 52px
│                             │
│  Centered Price (Large)     │ 150px
│                             │
│  Block Info  │  Mempool     │ 220px
│                             │
├─────────────────────────────┤ 302px
│ Status Bar (18px)           │
└─────────────────────────────┘ 320px
```

### Grid Layout (Settings)
```
┌─────────────────────────────┐ 0px
│ Header (28px)               │
├─────────────────────────────┤ 28px
│ 8px margin                  │
│ ┌────────┐  ┌────────┐     │ 36px
│ │Card 1  │  │Card 2  │     │ (50px)
│ └────────┘  └────────┘     │ 86px
│ 6px spacing                 │
│ ┌────────┐  ┌────────┐     │ 92px
│ │Card 3  │  │Card 4  │     │ (50px)
│ └────────┘  └────────┘     │ 142px
│ 6px spacing                 │
│ ┌────────┐  ┌────────┐     │ 148px
│ │Card 5  │  │Card 6  │     │ (50px)
│ └────────┘  └────────┘     │ 198px
│                             │
│ ┌─────────────────────┐    │ 204px
│ │   Footer (40px)     │    │
│ └─────────────────────┘    │ 244px
└─────────────────────────────┘ 320px
```

### Scrollable Content Layout
```
┌─────────────────────────────┐ 0px
│ Header (30px)               │
├─────────────────────────────┤ 30px
│ 8px margin                  │
│ ┌─────────────────────────┐ │ 38px
│ │                         │ │
│ │  Scrollable Content     │ │ (268px)
│ │  with scroll indicator  │ │
│ │                         │ │
│ └─────────────────────────┘ │ 306px
│ ┌─────────────────────────┐ │ 307px
│ │  Navigation Hint (13px) │ │
│ └─────────────────────────┘ │ 320px
└─────────────────────────────┘
```

## Common Patterns

### Clear Content Area
```cpp
// Preserve header, clear content
lcd->fillRect(0, 29, 480, 291, COLOR_BG);
```

### Draw Bordered Card
```cpp
// Multi-layer card with depth
lcd->fillRoundRect(x+2, y+2, w, h, radius, 0x050505);  // Shadow
lcd->fillRoundRect(x, y, w, h, radius, 0x0A0A0A);      // Background
lcd->drawRoundRect(x, y, w, h, radius, borderColor);   // Outer border
lcd->drawRoundRect(x+1, y+1, w-2, h-2, radius-1, 0x1A1A1A);  // Inner border
```

### Word Wrapping
```cpp
int charsPerLine = 55;  // For 9pt font
int startPos = 0;

while (startPos < text.length()) {
    int endPos = min((int)text.length(), startPos + charsPerLine);

    // Try to break at space
    if (endPos < text.length()) {
        int spacePos = text.lastIndexOf(' ', endPos);
        if (spacePos > startPos) {
            endPos = spacePos;
        }
    }

    String line = text.substring(startPos, endPos);
    lcd->print(line);

    y += lineHeight;
    startPos = endPos + 1;
}
```

### Scroll Bar
```cpp
if (maxScrollOffset > 0) {
    int scrollBarH = cardH - 20;
    int scrollBarY = cardY + 10;
    int scrollBarX = cardX + cardW - 10;
    int thumbY = scrollBarY + (scrollOffset * (scrollBarH - 35) / (maxScrollOffset + 1));

    // Track
    lcd->fillRoundRect(scrollBarX, scrollBarY, 6, scrollBarH, 3, 0x0A0A0A);
    lcd->drawRoundRect(scrollBarX, scrollBarY, 6, scrollBarH, 3, 0x1A1A1A);

    // Thumb
    lcd->fillRoundRect(scrollBarX, thumbY, 6, 35, 3, COLOR_BTC_ORANGE);
}
```

## Font Reference

### Available Fonts
```cpp
&fonts::FreeSans9pt7b         // Body text, labels
&fonts::FreeSansBold9pt7b     // Headers, emphasis
&fonts::FreeSans12pt7b        // Larger body text (rarely used)
&fonts::FreeSansBold12pt7b    // Subheaders, time display
&fonts::FreeSansBold18pt7b    // Large price display only
```

### Character Counts (Approximate)
```
9pt font:  ~55 characters per 460px line
12pt font: ~40 characters per 460px line
18pt font: ~25 characters per 460px line
```

### Line Heights
```
9pt body:   18px (optimized)
9pt header: 20px
12pt:       22px
18pt:       28px
```

## Color Usage

### Primary Colors
```cpp
COLOR_BTC_ORANGE    = 0xFF9500  // Accent, highlights, active states
COLOR_TEXT_WHITE    = 0xFFFFFF  // Primary readable text
COLOR_TEXT_LIGHT    = 0xCCCCCC  // Secondary text
COLOR_TEXT_DIM      = 0x999999  // Hints, subtle text
```

### State Colors
```cpp
COLOR_SUCCESS       = 0x00FF00  // Positive, connected, healthy
COLOR_WARNING       = 0xFFAA00  // Caution, moderate
COLOR_ERROR         = 0xFF3333  // Error, failed, critical
COLOR_LOADING       = 0xFFBF00  // Loading, pending
```

### Background Colors
```cpp
COLOR_BG            = 0x000000  // Screen background (black)
COLOR_CARD_BG       = 0x0C0C0C  // Card background (dark gray)
COLOR_HEADER_BG     = 0x1A1A1A  // Header background (lighter gray)
```

## Performance Tips

1. **Minimize Full Redraws:** Clear only the content area, preserve header
2. **Use Appropriate Fill:** `fillRect` is faster than `fillRoundRect` for simple areas
3. **Batch Text Operations:** Set font/color once before multiple prints
4. **Optimize Scrolling:** Only render visible content within viewport
5. **Avoid Transparency:** Use solid colors for better performance

## Debugging Layout

### Visual Grid Overlay
```cpp
// Draw pixel grid for alignment (development only)
void drawGrid() {
    for (int x = 0; x < 480; x += 20) {
        lcd->drawFastVLine(x, 0, 320, 0x222222);
    }
    for (int y = 0; y < 320; y += 20) {
        lcd->drawFastHLine(0, y, 480, 0x222222);
    }
}
```

### Measure Content
```cpp
// Log content dimensions
Serial.printf("Content area: %d x %d\n", width, height);
Serial.printf("Visible lines: %d\n", visibleHeight / lineHeight);
Serial.printf("Total lines: %d\n", totalHeight / lineHeight);
```

## Checklist for New Screens

- [ ] Header is 28-30px with accent line
- [ ] Back button at (10, 4, 30x30)
- [ ] Content starts 6-8px below header
- [ ] Cards have 6-8px margins from screen edges
- [ ] Card spacing is 6px between rows
- [ ] Line height is 18px for body text
- [ ] Font is FreeSans9pt7b for body, FreeSansBold9pt7b for headers
- [ ] Colors follow the standard palette
- [ ] Touch targets are minimum 30x30px
- [ ] Scrolling calculates maxScrollOffset correctly
- [ ] Text truncates or wraps for long content
- [ ] Bottom margin is 14-20px

## Examples

See optimized implementations in:
- `/src/screens/BTCDashboardScreen.cpp` - Centered layout
- `/src/screens/SettingsScreen.cpp` - Grid layout
- `/src/screens/BTCNewsScreen.cpp` - Scrollable content
- `/src/screens/BTCTradingSuggestionScreen.cpp` - Complex mixed layout
