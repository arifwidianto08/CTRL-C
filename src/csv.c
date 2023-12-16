#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>

// https://tools.ietf.org/html/rfc4180
void modify_header(char *column_before)
{
    // Determine the length of the original string
    size_t column_before_length = strlen(column_before);

    // Create a character array to store the modified string
    char modified_string[column_before_length + 1]; // +1 for null terminator

    // Use strcpy to copy the original string to the modified string
    strcpy(modified_string, column_before);

    // Remove newline character by overwriting it with the null terminator
    for (size_t i = 0; i < column_before_length; i++)
    {
        if (modified_string[i] == '\n')
        {
            modified_string[i] = '\0';
            break; // Stop once the first newline character is encountered
        }
    }

    // Update the header with the modified string
    strcpy(column_before, modified_string);
}

char *get_csv_field(FILE *fp, char separator, int *state)
{
    int ch = fgetc(fp);

    if (ch == EOF)
        return NULL;

    size_t size = 1, index = 0;
    char *field = malloc(size); // Allocate memory for field
    if (!field)
        return NULL; // Handle malloc failure

    bool quoted_in = false;

    for (; ch != EOF; ch = fgetc(fp))
    {
        if (ch == '"')
        {
            if (quoted_in)
            {
                int prefetch = fgetc(fp);
                if (prefetch == '"')
                {
                    ch = prefetch; // Treat double quotes as one
                }
                else
                {
                    quoted_in = false;    // End quote
                    ungetc(prefetch, fp); // Put back next character
                    continue;
                }
            }
            else
            {
                quoted_in = true; // Begin quote
                continue;
            }
        }
        else if (!quoted_in && (ch == separator || ch == '\n'))
        {
            break; // End of field
        }
        field[index++] = ch;
        char *temp = realloc(field, ++size); // Reallocate memory if needed
        if (!temp)
        {
            free(field); // Free allocated memory on error
            return NULL;
        }
        field = temp;
    }
    field[index] = '\0'; // Add null terminator
    *state = ch;         // Store next character state

    if (quoted_in)
    {
        fprintf(stderr, "The quotes is not closed.\n");
        free(field); // Free allocated memory on error
        return NULL;
    }

    return field; // Return the allocated field string
}

char ***read_csv(const char *filename, size_t *rows, size_t *cols)
{
    FILE *fp = fopen(filename, "r");
    if (!fp)
    {
        fprintf(stderr, "%s can't open in %s\n", filename, __func__);
        perror("fopen");
        return NULL;
    }

    char *field;
    int state;
    size_t r = 0, c = 0;
    char ***mat = NULL;
    void *temp;

    while (field = get_csv_field(fp, ',', &state))
    {
        if (c == 0)
        {
            mat = realloc(mat, (r + 1) * sizeof(*mat));
            if (!mat)
            {
                fprintf(stderr, "realloc failed in %s\n", __func__);
                exit(EXIT_FAILURE);
            }
            mat[r] = NULL;
        }
        mat[r] = realloc(mat[r], (c + 1) * sizeof(**mat));
        if (!mat[r])
        {
            fprintf(stderr, "realloc failed in %s\n", __func__);
            exit(EXIT_FAILURE);
        }
        mat[r][c++] = field;
        if (state == '\n' || state == EOF)
        {
            if (*cols == 0)
            {
                *cols = c;
            }
            else if (c != *cols)
            {
                fprintf(stderr, "line %zu doesn't match number of columns in %s\n", r, filename);
                exit(EXIT_FAILURE);
            }
            c = 0;
            *rows = ++r;
        }
    }
    fclose(fp);

    return mat;
}

char *trim(char *s)
{
    if (!s || !*s)
        return s;

    size_t len = strlen(s);
    char *trimmed = malloc(len + 1); // Allocate memory for trimmed string
    if (!trimmed)
        return NULL; // Handle malloc failure

    size_t i = 0, j = 0;
    while (i < len && isspace((unsigned char)s[i]))
        i++; // Skip leading whitespace
    for (; i < len && !isspace((unsigned char)s[i]); i++, j++)
        trimmed[j] = s[i]; // Copy non-whitespace characters
    trimmed[j] = '\0';     // Add null terminator

    while (j > 0 && isspace((unsigned char)trimmed[j - 1]))
        j--; // Trim trailing whitespace

    return trimmed; // Return the newly allocated trimmed string
}

void write_csv(const char *filename, char ***mat, size_t rows, size_t cols)
{
    FILE *fp = fopen(filename, "wb");
    if (!fp)
    {
        fprintf(stderr, "%s can't open in %s\n", filename, __func__);
        perror("fopen");
        exit(EXIT_FAILURE);
    }

    if (cols == 1)
    {
        if (mat[0] && mat[0][0])
        {
            fprintf(fp, "%s", mat[0][0]);
        }
        fclose(fp);
        return;
    }

    // Write the modified matrix to the file
    for (size_t r = 0; r < rows; ++r)
    {
        for (size_t c = 0; c < cols; ++c)
        {
            if (c)
            {
                fprintf(fp, ",");
            }

            if (mat[r] && mat[r][c])
            {
                fprintf(fp, "%s", mat[r][c]);
            }
        }
        fprintf(fp, "\n");
    }

    fclose(fp);
}
