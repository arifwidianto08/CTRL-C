#!/bin/bash

echo "$(pwd)"

MIGRATION_DIR="db/migrations"
TEMPLATE_FILE="db/migrations/migration_template.c"

# Ensure a migration name is provided
if [ $# -eq 0 ]; then
    echo "Usage: $0 <migration_name>"
    exit 1
fi

MIGRATION_NAME=$1

# Format current date as Unix timestamp
CURRENT_DATE=$(date +"%s")
MIGRATION_FILE="${CURRENT_DATE}_${MIGRATION_NAME}.c"

# Check if the migrations directory exists, create if not
mkdir -p "$MIGRATION_DIR"

# Copy the template migration file to the new migration file
cp "$TEMPLATE_FILE" "$MIGRATION_DIR/$MIGRATION_FILE"

echo "New migration file created: $MIGRATION_DIR/$MIGRATION_FILE"
