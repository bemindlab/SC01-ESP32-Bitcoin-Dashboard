#!/bin/bash
# Hook: Runs when deleting documentation files
# Warns about potential broken references

FILE_PATH="$1"

# Check if file is a documentation file
if [[ "$FILE_PATH" =~ \.(md|txt|pdf|doc|docx|rst)$ ]]; then
    FILENAME=$(basename "$FILE_PATH")

    echo "⚠ Deleting documentation: $FILE_PATH"
    echo ""
    echo "Before deleting, please verify:"
    echo "  1. Check README.md for references to this file"
    echo "  2. Search docs/ directory for cross-references"
    echo "  3. Consider archiving instead of deleting"
    echo ""

    # Search for potential references in other markdown files
    if command -v grep &> /dev/null; then
        echo "Searching for references to '$FILENAME'..."
        grep -r "$FILENAME" . --include="*.md" 2>/dev/null | grep -v "^$FILE_PATH:" || echo "  No references found"
    fi

    echo ""
    read -p "Continue with deletion? (y/N): " -n 1 -r
    echo
    if [[ ! $REPLY =~ ^[Yy]$ ]]; then
        echo "Deletion cancelled"
        exit 1
    fi
fi

exit 0
