# Navigation Diagram - Bitcoin Dashboard

## Circular Swipe Carousel

The three main screens form a continuous loop where you can swipe infinitely in either direction:

```
┌─────────────────────────────────────────────────────────┐
│                   CIRCULAR SWIPE LOOP                    │
│                                                          │
│         ┌───────────┐      ┌───────────┐      ┌───────────┐
│         │           │      │           │      │           │
│         │ Dashboard │◄────►│  Trading  │◄────►│   News    │
│         │           │      │Suggestion │      │   (AI)    │
│         └───────────┘      └───────────┘      └───────────┘
│              ▲                   ▲                   ▲
│              │                   │                   │
│              └───────────────────┴───────────────────┘
│                        (Loops back)
│
│  Swipe Left  (←): Dashboard → News → Trading → Dashboard
│  Swipe Right (→): Dashboard → Trading → News → Dashboard
└─────────────────────────────────────────────────────────┘
```

## Detailed Navigation Flow

### From Dashboard

```
┌─────────────────┐
│   Dashboard     │
│   (Main View)   │
└─────────────────┘
         │
    Swipe Left (←)  → News Screen
    Swipe Right (→) → Trading Screen
    Tap Gear (⚙)    → Settings
    Tap WiFi (📡)   → WiFi Scan
```

### From Trading Suggestions

```
┌─────────────────┐
│    Trading      │
│  Suggestions    │
└─────────────────┘
         │
    Swipe Left (←)  → Dashboard (completes loop!)
    Swipe Right (→) → News Screen
    Tap Gear (⚙)    → Settings
    Tap WiFi (📡)   → WiFi Scan
    Tap Refresh (🔄) → Update analysis
```

### From News Screen

```
┌─────────────────┐
│    BTC News     │
│   (Gemini AI)   │
└─────────────────┘
         │
    Swipe Left (←)  → Trading Screen
    Swipe Right (→) → Dashboard
    Tap @ Button    → Refresh news
    Tap Gear (⚙)    → Settings
    Tap WiFi (📡)   → WiFi Scan
```

## Swipe Direction Guide

### Clockwise Navigation (Swipe Right →)

```
    Start: Dashboard
      ↓ Swipe Right
    Trading Suggestions
      ↓ Swipe Right
    News Screen
      ↓ Swipe Right
    Dashboard (back to start!)
```

### Counter-Clockwise Navigation (Swipe Left ←)

```
    Start: Dashboard
      ↓ Swipe Left
    News Screen
      ↓ Swipe Left
    Trading Suggestions
      ↓ Swipe Left
    Dashboard (back to start!)
```

## Complete Navigation Map

```
                         ┌────────────┐
                         │   WiFi     │
                         │   Scan     │
                         └──────┬─────┘
                                │ Tap WiFi Icon
          ┌─────────────────────┼─────────────────────┐
          │                     │                     │
    ┌─────▼──────┐       ┌─────▼──────┐       ┌─────▼──────┐
    │ Dashboard  │◄─────►│  Trading   │◄─────►│    News    │
    │            │       │ Suggestion │       │            │
    └─────┬──────┘       └─────┬──────┘       └─────┬──────┘
          │                     │                     │
          └─────────────────────┼─────────────────────┘
                                │ Tap Gear Icon
                         ┌──────▼─────┐
                         │ Settings   │
                         └────────────┘

Legend:
◄────► = Horizontal swipe (left/right)
   │   = Tap icon/button
```

## Infinite Loop Behavior

The carousel never "ends" - you can keep swiping in one direction forever:

**Swipe Left (←) Loop:**
```
Dashboard → News → Trading → Dashboard → News → Trading → ...
```

**Swipe Right (→) Loop:**
```
Dashboard → Trading → News → Dashboard → Trading → News → ...
```

## User Experience Benefits

1. **No Dead Ends:** Every screen connects to every other screen
2. **Intuitive:** Natural swipe gestures (left/right)
3. **Efficient:** Maximum 2 swipes to reach any screen
4. **Consistent:** Same swipe behavior across all screens
5. **Explorable:** Users can freely explore by swiping

## Implementation Details

### Swipe Detection Parameters

```cpp
#define SWIPE_MIN_DISTANCE 80    // Minimum pixels for valid swipe
#define SWIPE_MAX_TIME 500       // Maximum time (ms) for swipe
#define SWIPE_THRESHOLD_Y 50     // Max vertical deviation
```

### Screen Transition States

```
Current Screen → Swipe Direction → Next Screen
─────────────────────────────────────────────────
Dashboard      → Left (←)        → News
Dashboard      → Right (→)       → Trading
News           → Left (←)        → Trading
News           → Right (→)       → Dashboard
Trading        → Left (←)        → Dashboard  (loop!)
Trading        → Right (→)       → News
```

## Testing Scenarios

### Scenario 1: Full Left Loop
1. Start at Dashboard
2. Swipe left → News
3. Swipe left → Trading
4. Swipe left → Dashboard ✓ (back to start)

### Scenario 2: Full Right Loop
1. Start at Dashboard
2. Swipe right → Trading
3. Swipe right → News
4. Swipe right → Dashboard ✓ (back to start)

### Scenario 3: Quick Navigation
- Dashboard → News: 1 swipe left
- Dashboard → Trading: 1 swipe right
- News → Trading: 1 swipe left
- Trading → News: 1 swipe right

### Scenario 4: Reverse Direction
1. Start at Trading
2. Swipe left → Dashboard
3. Swipe right → Trading ✓ (can reverse immediately)

---

**Version:** 2.0 (Circular Loop)
**Date:** 2025-11-28
**Status:** Planned Implementation
