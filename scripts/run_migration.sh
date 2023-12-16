#!/bin/bash

MIGRATION_DIR="db/migrations"
LOG_FILE="logs/migration_log.txt"

# Create the log file if it doesn't exist
touch "$LOG_FILE"

# Get a list of migration files sorted by datetime
FILES=$(ls -1 "$MIGRATION_DIR" | sort)

# Read the log file to get the list of already executed migrations
executed_migrations=$(cat "$LOG_FILE")

# Function to check if a migration has already been executed
migration_executed() {
    grep -q "$1" <<<"$executed_migrations"
}

# Iterate through each migration file and execute if not already executed
for FILE in $FILES; do
    FULL_PATH="$MIGRATION_DIR/$FILE"

    # if [[ $FILE == "migration_template.c" ]] || [[ $FILE = *'.c'* ]]; then
    if [[ $FILE == "migration_template.c" ]]; then
        continue
    else
        # elif [ -f "${FULL_PATH%.c}" ] && [ -x "${FULL_PATH%.c}" ]; then
        # Check if the migration has already been executed
        if migration_executed "$FILE"; then
            # Determine whether to run "up" or "rollback" based on the log
            if grep -q "up" <<<"$executed_migrations" && [[ "$1" == "rollback" ]]; then
                echo "Running rollback for: $FILE"
                # Compile the C file
                gcc -o "${FULL_PATH%.c}" "$FULL_PATH" src/migrator.c src/csv.c -I./include

                if [ $? -eq 0 ]; then
                    echo "Compiling $FILE succeeded"
                    echo "Running up migration: $FILE"
                    ./"${FULL_PATH%.c}" rollback
                    echo "$FILE up" >>"$LOG_FILE"

                    # Delete the compiled migration file
                    rm -f "${FULL_PATH%.c}"
                    echo "Deleted compiled migration file: ${FULL_PATH%.c}"
                else
                    echo "Compiling $FILE failed"
                fi

                sed -i "/$FILE/d" "$LOG_FILE"
            else
                echo "Skipping already executed migration: $FILE"
            fi
        elif [[ "$1" == "up" ]]; then
            # Compile the C file
            gcc -o "${FULL_PATH%.c}" "$FULL_PATH" src/migrator.c src/csv.c -I./include

            if [ $? -eq 0 ]; then
                echo "Compiling $FILE succeeded"
                echo "Running up migration: $FILE"
                ./"${FULL_PATH%.c}" up
                echo "$FILE up" >>"$LOG_FILE"

                # Delete the compiled migration file
                rm -f "${FULL_PATH%.c}"
                echo "Deleted compiled migration file: ${FULL_PATH%.c}"
            else
                echo "Compiling $FILE failed"
            fi
        fi
    # else
    #     echo "Skipping non-executable file: $FILE"
    fi
done
