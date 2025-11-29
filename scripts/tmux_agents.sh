#!/bin/bash
# Multi-Agent Development Session Manager
# Creates tmux session with multiple agent panes for parallel development

set -e

SESSION_NAME="btc-dashboard-agents"
PROJECT_ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# Agent communication directory
AGENT_COMM_DIR="${PROJECT_ROOT}/.tmp/agent-comm"
mkdir -p "${AGENT_COMM_DIR}"

# Function to print colored messages
print_info() {
    echo -e "${BLUE}[INFO]${NC} $1"
}

print_success() {
    echo -e "${GREEN}[SUCCESS]${NC} $1"
}

print_warning() {
    echo -e "${YELLOW}[WARNING]${NC} $1"
}

print_error() {
    echo -e "${RED}[ERROR]${NC} $1"
}

# Check if tmux is installed
if ! command -v tmux &> /dev/null; then
    print_error "tmux is not installed. Install with: brew install tmux"
    exit 1
fi

# Function to check if session exists
session_exists() {
    tmux has-session -t "${SESSION_NAME}" 2>/dev/null
}

# Function to create agent communication file
create_agent_comm_file() {
    local agent_name=$1
    local comm_file="${AGENT_COMM_DIR}/${agent_name}.log"
    echo "$(date '+%Y-%m-%d %H:%M:%S') - Agent ${agent_name} initialized" > "${comm_file}"
    echo "${comm_file}"
}

# Function to setup agent environment
setup_agent_env() {
    local agent_name=$1
    cat <<EOF
# Agent: ${agent_name}
# Communication: ${AGENT_COMM_DIR}/${agent_name}.log
export AGENT_NAME="${agent_name}"
export AGENT_COMM_DIR="${AGENT_COMM_DIR}"
export PROJECT_ROOT="${PROJECT_ROOT}"

# Agent helper functions
agent_log() {
    echo "\$(date '+%Y-%m-%d %H:%M:%S') - [\${AGENT_NAME}] \$1" >> "\${AGENT_COMM_DIR}/\${AGENT_NAME}.log"
}

agent_broadcast() {
    echo "\$(date '+%Y-%m-%d %H:%M:%S') - [\${AGENT_NAME}] \$1" >> "\${AGENT_COMM_DIR}/broadcast.log"
}

agent_send() {
    local target_agent=\$1
    shift
    echo "\$(date '+%Y-%m-%d %H:%M:%S') - [\${AGENT_NAME} -> \${target_agent}] \$*" >> "\${AGENT_COMM_DIR}/\${target_agent}.log"
}

agent_read() {
    tail -20 "\${AGENT_COMM_DIR}/\${AGENT_NAME}.log"
}

agent_monitor() {
    tail -f "\${AGENT_COMM_DIR}/\${AGENT_NAME}.log"
}

cd "${PROJECT_ROOT}"
clear
echo -e "${GREEN}╔════════════════════════════════════════════════════════════╗${NC}"
echo -e "${GREEN}║${NC}  Agent: ${YELLOW}${agent_name}${NC}"
echo -e "${GREEN}║${NC}  Communication: \${AGENT_COMM_DIR}/${agent_name}.log"
echo -e "${GREEN}║${NC}  Commands: agent_log, agent_broadcast, agent_send"
echo -e "${GREEN}╚════════════════════════════════════════════════════════════╝${NC}"
echo ""
agent_log "Agent started"
EOF
}

# Function to kill existing session
kill_session() {
    if session_exists; then
        print_info "Killing existing session: ${SESSION_NAME}"
        tmux kill-session -t "${SESSION_NAME}"
        print_success "Session killed"
    fi
}

# Function to create new multi-agent session
create_session() {
    print_info "Creating multi-agent development session: ${SESSION_NAME}"

    # Clean communication directory
    rm -f "${AGENT_COMM_DIR}"/*.log

    # Create broadcast log
    echo "$(date '+%Y-%m-%d %H:%M:%S') - Multi-Agent Session Started" > "${AGENT_COMM_DIR}/broadcast.log"

    # Create new tmux session (detached)
    tmux new-session -d -s "${SESSION_NAME}" -n "coordinator"

    # Window 0: Coordinator (main control panel)
    tmux send-keys -t "${SESSION_NAME}:0" "$(setup_agent_env "coordinator")" C-m
    tmux send-keys -t "${SESSION_NAME}:0" "echo -e '${BLUE}Coordinator Agent${NC} - Main control panel'" C-m
    tmux send-keys -t "${SESSION_NAME}:0" "echo 'Type: agent_broadcast \"message\" to broadcast to all agents'" C-m
    tmux send-keys -t "${SESSION_NAME}:0" "tail -f ${AGENT_COMM_DIR}/broadcast.log" C-m

    # Window 1: ESP32 Hardware Specialist (vertical split)
    tmux new-window -t "${SESSION_NAME}:1" -n "esp32-hw"
    tmux send-keys -t "${SESSION_NAME}:1" "$(setup_agent_env "esp32-hardware")" C-m
    create_agent_comm_file "esp32-hardware"

    # Split horizontally for build output
    tmux split-window -t "${SESSION_NAME}:1" -h
    tmux send-keys -t "${SESSION_NAME}:1.1" "cd ${PROJECT_ROOT}" C-m
    tmux send-keys -t "${SESSION_NAME}:1.1" "echo -e '${GREEN}ESP32 Build Monitor${NC}'" C-m

    # Window 2: UI/UX Designer (LVGL specialist)
    tmux new-window -t "${SESSION_NAME}:2" -n "ui-designer"
    tmux send-keys -t "${SESSION_NAME}:2" "$(setup_agent_env "ui-designer")" C-m
    create_agent_comm_file "ui-designer"

    # Split horizontally for UI preview/screenshots
    tmux split-window -t "${SESSION_NAME}:2" -h
    tmux send-keys -t "${SESSION_NAME}:2.1" "cd ${PROJECT_ROOT}" C-m
    tmux send-keys -t "${SESSION_NAME}:2.1" "echo -e '${GREEN}UI Preview/Screenshots${NC}'" C-m

    # Window 3: Bitcoin API Specialist
    tmux new-window -t "${SESSION_NAME}:3" -n "btc-api"
    tmux send-keys -t "${SESSION_NAME}:3" "$(setup_agent_env "bitcoin-api")" C-m
    create_agent_comm_file "bitcoin-api"

    # Split horizontally for API testing
    tmux split-window -t "${SESSION_NAME}:3" -h
    tmux send-keys -t "${SESSION_NAME}:3.1" "cd ${PROJECT_ROOT}" C-m
    tmux send-keys -t "${SESSION_NAME}:3.1" "echo -e '${GREEN}API Test Output${NC}'" C-m

    # Window 4: Embedded Debugger
    tmux new-window -t "${SESSION_NAME}:4" -n "debugger"
    tmux send-keys -t "${SESSION_NAME}:4" "$(setup_agent_env "debugger")" C-m
    create_agent_comm_file "debugger"

    # Split horizontally for serial monitor
    tmux split-window -t "${SESSION_NAME}:4" -h
    tmux send-keys -t "${SESSION_NAME}:4.1" "cd ${PROJECT_ROOT}" C-m
    tmux send-keys -t "${SESSION_NAME}:4.1" "echo -e '${GREEN}Serial Monitor${NC}'" C-m
    tmux send-keys -t "${SESSION_NAME}:4.1" "# Run: make monitor" C-m

    # Window 5: Build Specialist
    tmux new-window -t "${SESSION_NAME}:5" -n "build"
    tmux send-keys -t "${SESSION_NAME}:5" "$(setup_agent_env "build-specialist")" C-m
    create_agent_comm_file "build-specialist"

    # Split horizontally for build commands
    tmux split-window -t "${SESSION_NAME}:5" -h
    tmux send-keys -t "${SESSION_NAME}:5.1" "cd ${PROJECT_ROOT}" C-m
    tmux send-keys -t "${SESSION_NAME}:5.1" "echo -e '${GREEN}Build Commands${NC}'" C-m
    tmux send-keys -t "${SESSION_NAME}:5.1" "echo 'Quick commands: make build, make upload, make flash'" C-m

    # Window 6: Global Monitor (all agent communications)
    tmux new-window -t "${SESSION_NAME}:6" -n "monitor"
    tmux send-keys -t "${SESSION_NAME}:6" "cd ${PROJECT_ROOT}" C-m
    tmux send-keys -t "${SESSION_NAME}:6" "echo -e '${BLUE}╔════════════════════════════════════════════════════════════╗${NC}'" C-m
    tmux send-keys -t "${SESSION_NAME}:6" "echo -e '${BLUE}║${NC}  Multi-Agent Communication Monitor'" C-m
    tmux send-keys -t "${SESSION_NAME}:6" "echo -e '${BLUE}╚════════════════════════════════════════════════════════════╝${NC}'" C-m
    tmux send-keys -t "${SESSION_NAME}:6" "tail -f ${AGENT_COMM_DIR}/*.log" C-m

    # Set default window
    tmux select-window -t "${SESSION_NAME}:0"

    print_success "Multi-agent session created!"
    echo ""
    print_info "Session Layout:"
    echo "  Window 0: Coordinator (main control)"
    echo "  Window 1: ESP32 Hardware Specialist + Build Monitor"
    echo "  Window 2: UI/UX Designer + Preview"
    echo "  Window 3: Bitcoin API Specialist + API Tests"
    echo "  Window 4: Embedded Debugger + Serial Monitor"
    echo "  Window 5: Build Specialist + Commands"
    echo "  Window 6: Global Communication Monitor"
    echo ""
    print_info "Communication logs: ${AGENT_COMM_DIR}"
}

# Function to attach to session
attach_session() {
    if session_exists; then
        print_info "Attaching to session: ${SESSION_NAME}"
        tmux attach-session -t "${SESSION_NAME}"
    else
        print_error "Session does not exist. Create it first with: $0 start"
        exit 1
    fi
}

# Function to show session status
show_status() {
    if session_exists; then
        print_success "Session '${SESSION_NAME}' is running"
        echo ""
        print_info "Windows:"
        tmux list-windows -t "${SESSION_NAME}"
        echo ""
        print_info "Recent communications:"
        if [ -f "${AGENT_COMM_DIR}/broadcast.log" ]; then
            tail -5 "${AGENT_COMM_DIR}/broadcast.log"
        fi
    else
        print_warning "Session '${SESSION_NAME}' is not running"
    fi
}

# Main command handler
case "${1:-}" in
    start)
        if session_exists; then
            print_warning "Session already exists"
            show_status
            echo ""
            read -p "Kill and recreate? (y/N): " -n 1 -r
            echo
            if [[ $REPLY =~ ^[Yy]$ ]]; then
                kill_session
                create_session
                attach_session
            fi
        else
            create_session
            attach_session
        fi
        ;;

    stop)
        kill_session
        ;;

    attach)
        attach_session
        ;;

    status)
        show_status
        ;;

    list)
        print_info "Active tmux sessions:"
        tmux list-sessions 2>/dev/null || print_warning "No active sessions"
        ;;

    logs)
        if [ -d "${AGENT_COMM_DIR}" ]; then
            print_info "Agent communication logs:"
            ls -lh "${AGENT_COMM_DIR}"
        else
            print_warning "No logs found"
        fi
        ;;

    clean)
        print_info "Cleaning agent communication logs..."
        rm -rf "${AGENT_COMM_DIR}"
        mkdir -p "${AGENT_COMM_DIR}"
        print_success "Logs cleaned"
        ;;

    help|*)
        cat <<EOF
${GREEN}Multi-Agent Development Session Manager${NC}

Usage: $0 <command>

Commands:
  start   - Create and attach to multi-agent session
  stop    - Kill the multi-agent session
  attach  - Attach to existing session
  status  - Show session status
  list    - List all tmux sessions
  logs    - Show agent communication logs
  clean   - Clean communication logs
  help    - Show this help message

Session: ${SESSION_NAME}
Communication: ${AGENT_COMM_DIR}

${YELLOW}Quick Start:${NC}
  1. ${BLUE}$0 start${NC}        # Create and enter session
  2. Use ${BLUE}Ctrl-b${NC} then ${BLUE}0-6${NC} to switch between agent windows
  3. Use ${BLUE}Ctrl-b d${NC} to detach (session keeps running)
  4. ${BLUE}$0 attach${NC}       # Reattach to session

${YELLOW}Agent Communication:${NC}
  - ${BLUE}agent_log "message"${NC}              # Log to agent's own log
  - ${BLUE}agent_broadcast "message"${NC}        # Broadcast to all agents
  - ${BLUE}agent_send target_agent "message"${NC} # Send to specific agent
  - ${BLUE}agent_read${NC}                       # Read recent messages
  - ${BLUE}agent_monitor${NC}                    # Live tail agent log

EOF
        ;;
esac
