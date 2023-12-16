#include "csv.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>

#define MAX_ROWS 1000
#define MAX_COLS 50
#define MAX_FIELD_LENGTH 50

void check_column_exists(const char *filename, char *new_column_name)
{
    FILE *fp = fopen(filename, "r");
    if (!fp)
    {
        fprintf(stderr, "%s can't open in %s\n", filename, __func__);
        perror("fopen");
        exit(EXIT_FAILURE);
    }

    // Read the header
    char *line = NULL;
    size_t len = 0;
    __ssize_t read = getline(&line, &len, fp);

    // Check if the new column already exists
    if (strstr(line, new_column_name) != NULL)
    {
        printf("Error: Column '%s' already exists.\n", new_column_name);
        fclose(fp);
        free(line);
        exit(EXIT_FAILURE);
    }
}

int add_column(const char *filename, char *new_column_name, char *default_value)
{
    // Validate if column already exists
    check_column_exists(filename, new_column_name);

    size_t rows = 0, cols = 0;
    char ***mat = read_csv(filename, &rows, &cols);

    // Check for valid rows and columns
    if (cols == 0)
    {

        FILE *fp = fopen(filename, "w+");
        if (!fp)
        {
            fprintf(stderr, "%s can't open in %s\n", filename, __func__);
            perror("fopen");
            return 1;
        }

        // Initiate the first column
        fprintf(fp, "%s", new_column_name);
        fclose(fp);

        return 0;
    }

    // Modify the header
    mat[0] = realloc(mat[0], (cols + 1) * sizeof(*mat[0]));
    if (!mat[0])
    {
        fprintf(stderr, "realloc failed in %s\n", __func__);
        free(mat);
        exit(EXIT_FAILURE);
    }
    mat[0][cols] = strdup(new_column_name);

    // Modify the rest of the CSV data
    for (size_t r = 1; r < rows; ++r)
    {
        // Check if row exists before accessing elements
        if (r >= rows)
        {
            break;
        }

        // Output existing columns
        for (size_t c = 0; c < cols; ++c)
        {
            // Check if column exists before accessing elements
            if (c >= cols)
            {
                break;
            }

            if (rows > 0)
            {
                mat[r] = realloc(mat[r], (cols + 1) * sizeof(*mat[r]));
                mat[r][cols] = strdup(trim(default_value));
            }
        }
    }

    // Update the CSV file
    write_csv(filename, mat, rows, cols + 1);

    return 0;
}

void rename_column(const char *filename, const char *old_column_name, const char *new_column_name)
{
    size_t rows = 0, cols = 0;
    char ***mat = read_csv(filename, &rows, &cols);

    // Find the index of the column to rename
    size_t col_index = cols;
    for (size_t c = 0; c < cols; ++c)
    {
        if (mat[0][c] && strcmp(mat[0][c], old_column_name) == 0)
        {
            col_index = c;
            break;
        }
    }

    // Check if the column to rename was found
    if (col_index == cols)
    {
        printf("Error: Column '%s' not found.\n", old_column_name);
        free(mat); // Free allocated memory
        exit(EXIT_FAILURE);
    }

    // Free memory for the old column name in the header
    free(mat[0][col_index]);

    // Assign the new column name to the header
    mat[0][col_index] = strdup(new_column_name);

    // Update the CSV file
    write_csv(filename, mat, rows, cols);

    // Free the entire matrix
    free(mat);
}

void drop_column(const char *filename, const char *columnName)
{
    size_t rows = 0, cols = 0;
    char ***mat = read_csv(filename, &rows, &cols);

    // Find the index of the column to drop
    size_t col_index = cols;
    for (size_t c = 0; c < cols; ++c)
    {
        if (mat[0][c] && strcmp(mat[0][c], columnName) == 0)
        {
            col_index = c;
            break;
        }
    }

    // Check if the column to drop was found
    if (col_index == cols)
    {
        printf("Error: Column '%s' not found.\n", columnName);
        free(mat); // Free allocated memory
        exit(EXIT_FAILURE);
    }

    // Free memory for the dropped column in the header
    free(mat[0][col_index]);

    // Shift columns to the left to fill the gap of the dropped column
    for (size_t r = 0; r < rows; ++r)
    {
        for (size_t c = col_index; c < cols - 1; ++c)
        {
            mat[r][c] = mat[r][c + 1];
        }

        // Resize the row after shifting columns
        mat[r] = realloc(mat[r], (cols - 1) * sizeof(*mat[r]));
    }

    // Update the CSV file
    write_csv(filename, mat, rows, cols - 1);

    // Free the entire matrix
    free(mat);
}

// Function to create a CSV file with a specific structure
void create_table(const char *filename)
{
    FILE *file = fopen(filename, "w, ccs=UTF-8");
    if (file == NULL)
    {
        perror("Error creating file");
        return;
    }

    fclose(file);
    printf("Table created successfully.\n");
}

// Function to delete the CSV file
void drop_table(const char *filename)
{
    if (remove(filename) == 0)
    {
        printf("Table deleted successfully.\n");
    }
    else
    {
        perror("Error deleting file");
    }
}
