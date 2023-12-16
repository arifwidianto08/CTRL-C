#ifndef CSV_OPERATIONS_H
#define CSV_OPERATIONS_H

void check_column_exists(const char *filename, char *new_column_name);

int add_column(const char *filename, char *new_column_name, char *default_value);

void rename_column(const char *filename, const char *old_column_name, const char *new_column_name);

void drop_column(const char *filename, const char *columnName);

void create_table(const char *filename);

void drop_table(const char *filename);

#endif // CSV_OPERATIONS_H
