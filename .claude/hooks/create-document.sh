#!/bin/bash
# Hook: Runs when creating documentation files
# Ensures documents are placed in the correct directory

FILE_PATH="$1"

# Check if file is a documentation file (markdown, txt, pdf, etc.)
if [[ "$FILE_PATH" =~ \.(md|txt|pdf|doc|docx|rst)$ ]]; then
    # Check if file is being created in root (excluding allowed files)
    FILENAME=$(basename "$FILE_PATH")
    DIRNAME=$(dirname "$FILE_PATH")

    if [[ "$DIRNAME" == "." || "$DIRNAME" == "/" || "$DIRNAME" == "/Users/lps/Solutions/bemind-embeded/btc-dashboard" ]]; then
        # Allow specific files in root
        if [[ "$FILENAME" != "README.md" && "$FILENAME" != "LICENSE" && "$FILENAME" != "CLAUDE.md" ]]; then
            echo "ERROR: Documentation files must be created in the docs/ directory"
            echo "File: $FILE_PATH"
            echo "Please create this file in: docs/$FILENAME"
            exit 1
        fi
    fi

    # If creating in docs/, verify docs/ directory exists
    if [[ "$FILE_PATH" =~ ^docs/ ]]; then
        if [ ! -d "docs" ]; then
            mkdir -p docs
            echo "Created docs/ directory"
        fi
        echo "✓ Creating documentation in correct location: $FILE_PATH"
    fi
fi

exit 0
