#!/bin/bash
# Hook: Runs when updating documentation files
# Validates documentation updates and maintains consistency

FILE_PATH="$1"

# Check if file is a documentation file
if [[ "$FILE_PATH" =~ \.(md|txt|pdf|doc|docx|rst)$ ]]; then
    FILENAME=$(basename "$FILE_PATH")

    # Check if documentation file exists in root but should be in docs/
    if [[ "$FILE_PATH" =~ ^\./ ]] && [[ "$FILENAME" != "README.md" && "$FILENAME" != "LICENSE" && "$FILENAME" != "CLAUDE.md" ]]; then
        echo "WARNING: Documentation file in root directory: $FILE_PATH"
        echo "Consider moving to: docs/$FILENAME"
    fi

    echo "✓ Updating documentation: $FILE_PATH"

    # If updating a markdown file, remind about cross-references
    if [[ "$FILE_PATH" =~ \.md$ ]]; then
        echo "  Remember to check for broken cross-references"
    fi
fi

exit 0
