#ifndef CSV_UTILS_H
#define CSV_UTILS_H

#include <stdio.h>
#include <stdlib.h>

// Function to modify the header according to RFC4180
void modify_header(char *column_before);

// Function to get a CSV field from a file
char *get_csv_field(FILE *fp, char separator, int *state);

// Function to read CSV data from a file
char ***read_csv(const char *filename, size_t *rows, size_t *cols);

// Function to write CSV data to a file
void write_csv(const char *filename, char ***mat, size_t rows, size_t cols);

// Function to trim whitespace from a string
char *trim(char *s);

#endif // CSV_UTILS_H
