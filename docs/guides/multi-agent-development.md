# Multi-Agent Development System

Comprehensive guide to using the tmux-based multi-agent development system for parallel Bitcoin Dashboard development.

## Overview

The multi-agent system enables **parallel development workflows** where specialized agents work simultaneously on different aspects of the project, communicating through a shared message bus.

### Key Benefits

- ✅ **Parallel Development**: Multiple agents work simultaneously on independent tasks
- ✅ **Real-time Communication**: Agents share progress, dependencies, and milestones
- ✅ **Organized Workflow**: Each agent has dedicated workspace with monitoring
- ✅ **Full Visibility**: Coordinator and monitor windows track all agent activity
- ✅ **Session Persistence**: Detach and reattach without losing context

## Quick Start

```bash
# Start multi-agent session
make agents

# Navigate between agents
Ctrl-b 0  # Coordinator
Ctrl-b 1  # ESP32 Hardware
Ctrl-b 2  # UI Designer
Ctrl-b 3  # Bitcoin API
Ctrl-b 4  # Debugger
Ctrl-b 5  # Build
Ctrl-b 6  # Monitor

# Detach (session keeps running)
Ctrl-b d

# Reattach later
make agents-attach

# Stop session
make agents-stop
```

## Session Architecture

### Window Layout

```
┌─────────────────────────────────────────────────────────────┐
│ Window 0: Coordinator                                       │
│ ┌─────────────────────┬─────────────────────────────────┐  │
│ │ Main Control Panel  │ Broadcast Monitor               │  │
│ │ - Task assignments  │ - All agent broadcasts          │  │
│ │ - Status overview   │ - Milestone tracking            │  │
│ └─────────────────────┴─────────────────────────────────┘  │
└─────────────────────────────────────────────────────────────┘

┌─────────────────────────────────────────────────────────────┐
│ Window 1: ESP32 Hardware Specialist                         │
│ ┌─────────────────────┬─────────────────────────────────┐  │
│ │ Agent Terminal      │ Build Output                    │  │
│ │ - Hardware config   │ - Compilation logs              │  │
│ │ - Memory analysis   │ - Upload status                 │  │
│ └─────────────────────┴─────────────────────────────────┘  │
└─────────────────────────────────────────────────────────────┘

┌─────────────────────────────────────────────────────────────┐
│ Window 2: UI/UX Designer                                    │
│ ┌─────────────────────┬─────────────────────────────────┐  │
│ │ Agent Terminal      │ UI Preview                      │  │
│ │ - LVGL code         │ - Screenshots                   │  │
│ │ - Layout design     │ - Visual testing                │  │
│ └─────────────────────┴─────────────────────────────────┘  │
└─────────────────────────────────────────────────────────────┘

┌─────────────────────────────────────────────────────────────┐
│ Window 3: Bitcoin API Specialist                            │
│ ┌─────────────────────┬─────────────────────────────────┐  │
│ │ Agent Terminal      │ API Test Output                 │  │
│ │ - API integration   │ - HTTP responses                │  │
│ │ - JSON parsing      │ - Endpoint testing              │  │
│ └─────────────────────┴─────────────────────────────────┘  │
└─────────────────────────────────────────────────────────────┘

┌─────────────────────────────────────────────────────────────┐
│ Window 4: Embedded Debugger                                 │
│ ┌─────────────────────┬─────────────────────────────────┐  │
│ │ Agent Terminal      │ Serial Monitor                  │  │
│ │ - Debug analysis    │ - Device output                 │  │
│ │ - Crash handling    │ - Exception logs                │  │
│ └─────────────────────┴─────────────────────────────────┘  │
└─────────────────────────────────────────────────────────────┘

┌─────────────────────────────────────────────────────────────┐
│ Window 5: Build Specialist                                  │
│ ┌─────────────────────┬─────────────────────────────────┐  │
│ │ Agent Terminal      │ Build Commands                  │  │
│ │ - Config analysis   │ - Quick actions                 │  │
│ │ - Optimization      │ - Test execution                │  │
│ └─────────────────────┴─────────────────────────────────┘  │
└─────────────────────────────────────────────────────────────┘

┌─────────────────────────────────────────────────────────────┐
│ Window 6: Global Monitor                                    │
│ ┌───────────────────────────────────────────────────────┐  │
│ │ All Agent Communication Logs                          │  │
│ │ - Real-time message stream                            │  │
│ │ - Cross-agent coordination                            │  │
│ │ - Task completion tracking                            │  │
│ └───────────────────────────────────────────────────────┘  │
└─────────────────────────────────────────────────────────────┘
```

## Agent Communication API

Each agent terminal has access to communication functions:

### Logging Functions

```bash
# Log to your own agent log
agent_log "Starting API endpoint integration"
agent_log "Completed fetchBTCPrice() implementation"

# Broadcast to all agents
agent_broadcast "BTC price API is ready for testing"
agent_broadcast "⚠️ Found memory leak in malloc(), fixing..."
agent_broadcast "✓ Feature complete: DCA recommendation card"

# Send message to specific agent
agent_send ui-designer "New data field: btcData.dcaRecommendation"
agent_send build "Ready for compilation - dependencies updated"
agent_send debugger "Please monitor for API timeout errors"
```

### Monitoring Functions

```bash
# Read recent messages sent to your agent
agent_read

# Live tail of your agent's log
agent_monitor

# View all communication logs
ls -lh .tmp/agent-comm/

# Read specific agent log
cat .tmp/agent-comm/bitcoin-api.log
```

## Common Workflows

### Workflow 1: New Feature Development

**Scenario**: Add 24-hour price change indicator

```bash
# Coordinator (Window 0)
agent_broadcast "New task: 24h price change indicator"
agent_send bitcoin-api "Fetch 24h price history from mempool.space"
agent_send ui-designer "Prepare card for percentage change (green/red)"

# Bitcoin API Agent (Window 3)
cd src/api
# Edit code...
agent_log "Added fetch24hChange() to MainScreen.cpp"
agent_send ui-designer "Data ready: btcData.priceChange24h (percentage)"
agent_send build "New API endpoint: /api/v1/price/24h"

# UI Designer Agent (Window 2)
cd src/screens
# Edit MainScreen.cpp...
agent_log "24h change card added: green if positive, red if negative"
agent_broadcast "UI ready for testing"

# Build Agent (Window 5)
make build
agent_log "Build successful: +1.2KB flash"
agent_send debugger "Deploy and verify 24h change display"

# Debugger Agent (Window 4)
make upload
make monitor
# Verify output...
agent_broadcast "✓ 24h change working: +2.5% shown in green"
```

### Workflow 2: Performance Optimization

**Scenario**: Improve scroll smoothness to 120 FPS

```bash
# Debugger (Window 4) - Identify bottleneck
make monitor
# Observe scroll FPS...
agent_log "Current scroll FPS: 45-50, target: 120"
agent_send esp32-hw "Scroll performance issue - check SPI/DMA config"

# ESP32 Hardware (Window 1) - Hardware optimization
cd platformio.ini
# Review LovyanGFX settings...
agent_log "SPI frequency at 40MHz, can increase to 80MHz"
agent_send ui-designer "Enable batch rendering: startWrite/endWrite"

# UI Designer (Window 2) - Code optimization
cd src/screens/MainScreen.cpp
# Optimize drawCard()...
agent_log "Removed rounded corners and shadows for faster rendering"
agent_log "Changed frame interval from 50ms to 8ms (120 FPS)"
agent_send build "Ready for performance test"

# Build (Window 5) - Test build
make flash
agent_send debugger "Test new firmware FPS"

# Debugger (Window 4) - Verify
agent_monitor
# Watch serial output...
agent_broadcast "✓ Scroll FPS now: 110-120, optimization successful!"
```

### Workflow 3: Bug Fix

**Scenario**: API timeout causing crash

```bash
# Debugger (Window 4) - Identify issue
tail -f .tmp/agent-comm/debugger.log
agent_log "💥 Crash detected: timeout in fetchBTCPrice()"
agent_log "Stack trace shows HTTPClient.cpp timeout handler"
agent_send bitcoin-api "API timeout not handled gracefully"

# Bitcoin API (Window 3) - Fix timeout handling
cd src/api
# Add error handling...
agent_log "Added try-catch for HTTP timeout in fetchBTCPrice()"
agent_log "Timeout now returns false instead of crashing"
agent_send build "Bug fix ready - test with poor network"

# Build (Window 5)
make flash
agent_send debugger "Deployed fix - test with WiFi disconnect"

# Debugger (Window 4) - Verify fix
# Disconnect WiFi temporarily...
agent_log "WiFi disconnected - testing timeout handling"
agent_log "✓ No crash - UI shows stale data as expected"
agent_broadcast "Bug fixed: API timeout handled gracefully"
```

## Best Practices

### 1. Communication Etiquette

**DO:**
- ✅ Log all significant actions: `agent_log "Starting X"`
- ✅ Notify dependencies: `agent_send target "Ready for Y"`
- ✅ Broadcast milestones: `agent_broadcast "✓ Feature complete"`
- ✅ Use emojis for visual scanning: ✓ ✗ ⚠️ 🔧 📊

**DON'T:**
- ❌ Silent work - always communicate progress
- ❌ Spam logs - batch similar messages
- ❌ Ignore incoming messages - check `agent_read` regularly

### 2. Task Coordination

**Coordinator Responsibilities:**
- Broadcast task breakdowns
- Track overall progress
- Resolve conflicts between agents
- Signal completion

**Agent Responsibilities:**
- Check for messages before starting work
- Log progress at key milestones
- Communicate dependencies clearly
- Report completion

### 3. File Coordination

**Avoid conflicts:**
- Each agent works on different files when possible
- Use `agent_broadcast` before editing shared files
- Check git status before committing

**Example:**
```bash
# Before editing MainScreen.cpp
agent_broadcast "Editing MainScreen.cpp for 2 minutes - please wait"
# ... make changes ...
git add src/screens/MainScreen.cpp
git commit -m "Add scroll optimization"
agent_broadcast "MainScreen.cpp changes committed"
```

### 4. Testing Coordination

**Sequential testing flow:**
1. **Build** creates firmware
2. **Build** notifies **Debugger**
3. **Debugger** uploads and monitors
4. **Debugger** reports results to all agents
5. **Coordinator** marks task complete

## Advanced Usage

### Custom Agent Scripts

Create agent-specific automation:

```bash
# .tmp/agent-scripts/api-test.sh
#!/bin/bash
agent_log "Running API integration tests"

# Test price endpoint
curl https://mempool.space/api/v1/prices
agent_log "Price API: OK"

# Test blocks endpoint
curl https://mempool.space/api/v1/blocks
agent_log "Blocks API: OK"

agent_broadcast "All API endpoints tested successfully"
```

### Multi-Session Workflows

For very complex tasks, run multiple sessions:

```bash
# Session 1: Development
make agents

# Session 2: Testing (separate terminal)
tmux new -s btc-testing
make monitor

# Session 3: Documentation (separate terminal)
tmux new -s btc-docs
vim docs/
```

### Agent Log Analysis

Analyze communication patterns:

```bash
# Count messages by agent
for log in .tmp/agent-comm/*.log; do
    echo "$(wc -l < $log) messages: $(basename $log)"
done

# Find critical messages
grep -r "✗\|ERROR\|FAIL" .tmp/agent-comm/

# Timeline of broadcasts
grep "broadcast" .tmp/agent-comm/broadcast.log | tail -20
```

## Troubleshooting

### Session Won't Start

```bash
# Check if tmux is installed
which tmux || brew install tmux

# Kill stuck sessions
tmux kill-server

# Try again
make agents
```

### Can't See Other Agent Logs

```bash
# Check communication directory
ls -la .tmp/agent-comm/

# Manually tail specific log
tail -f .tmp/agent-comm/ui-designer.log

# Window 6 shows all logs combined
Ctrl-b 6
```

### Lost in Tmux

```bash
# List windows
Ctrl-b w

# Show current window number
Ctrl-b i

# Detach and reattach
Ctrl-b d
make agents-attach
```

## Tips & Tricks

### 1. Quick Window Switching

```bash
# Jump to specific window
Ctrl-b 0   # Coordinator
Ctrl-b 3   # Bitcoin API
Ctrl-b 6   # Monitor

# Previous/Next window
Ctrl-b p   # Previous
Ctrl-b n   # Next
```

### 2. Pane Navigation

```bash
# Switch between left/right panes
Ctrl-b o

# Zoom current pane (full screen)
Ctrl-b z   # Toggle zoom
```

### 3. Copy Mode (Scroll back)

```bash
# Enter copy mode
Ctrl-b [

# Navigate with arrow keys or vim keys (j/k)
# Search: / or ?
# Exit: q
```

### 4. Save Session Layout

```bash
# Resurrect plugin (install separately)
Ctrl-b Ctrl-s   # Save session
Ctrl-b Ctrl-r   # Restore session
```

## Summary

The multi-agent system enables **efficient parallel development** with clear communication, organized workspaces, and full visibility into all agent activities.

**Key Commands:**
- `make agents` - Start session
- `Ctrl-b 0-6` - Navigate windows
- `agent_log` / `agent_broadcast` / `agent_send` - Communicate
- `Ctrl-b d` - Detach (keeps running)
- `make agents-attach` - Reattach

**Communication Flow:**
1. Coordinator assigns tasks
2. Agents work independently
3. Agents communicate dependencies
4. Build/Test validates
5. Coordinator confirms completion

Happy multi-agent coding! 🚀
