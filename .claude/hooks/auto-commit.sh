#!/bin/bash

# Auto-commit hook for Claude Code
# Automatically commits changes after significant code modifications

# Exit if not in a git repository
if ! git rev-parse --git-dir > /dev/null 2>&1; then
    exit 0
fi

# Check if there are any changes to commit
if ! git diff --quiet || ! git diff --cached --quiet; then
    echo "📝 Auto-committing changes..."

    # Get list of changed files
    CHANGED_FILES=$(git diff --name-only && git diff --cached --name-only | sort -u)

    # Create commit message based on changed files
    if echo "$CHANGED_FILES" | grep -q "\.cpp$\|\.h$"; then
        COMMIT_TYPE="code"
    elif echo "$CHANGED_FILES" | grep -q "\.md$"; then
        COMMIT_TYPE="docs"
    elif echo "$CHANGED_FILES" | grep -q "platformio.ini\|Makefile"; then
        COMMIT_TYPE="build"
    else
        COMMIT_TYPE="chore"
    fi

    # Count files
    FILE_COUNT=$(echo "$CHANGED_FILES" | wc -l | tr -d ' ')

    # Generate commit message
    COMMIT_MSG="$COMMIT_TYPE: Auto-commit $FILE_COUNT file(s)

Changes:
$(echo "$CHANGED_FILES" | sed 's/^/  - /')

🤖 Generated with Claude Code
Auto-committed via .claude/hooks/auto-commit.sh"

    # Stage all changes
    git add -A

    # Commit with generated message
    git commit -m "$COMMIT_MSG"

    echo "✓ Changes auto-committed successfully!"
    echo ""
    echo "Files committed:"
    echo "$CHANGED_FILES" | sed 's/^/  ✓ /'
else
    echo "No changes to commit"
fi
