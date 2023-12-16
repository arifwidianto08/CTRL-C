// db/migrations/migration_template.c

#include "migrator.h"
#include <stdio.h>
#include <string.h>

void up()
{
    const char *users_csv = "db/data/users.csv";
    // Migration logic: Add your changes here

    printf("Migration: Up\n");
    create_table(users_csv);
    add_column(users_csv, "Name", "default");
    add_column(users_csv, "Email", "default");
    add_column(users_csv, "Password", "default");
    add_column(users_csv, "Phone", "default");
}

void down()
{
    const char *users_csv = "db/data/users.csv";
    // Rollback logic: Add your changes here
    printf("Rollback: Down\n");
    drop_table(users_csv);
}

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        fprintf(stderr, "Usage: %s <up|rollback>\n", argv[0]);
        return 1;
    }

    // Decide whether to up or rollback the table based on the command line argument
    if (strcmp(argv[1], "up") == 0)
    {
        up();
    }
    else if (strcmp(argv[1], "rollback") == 0)
    {
        down();
    }
    else
    {
        fprintf(stderr, "Invalid argument: Use <up|rollback>\n");
        return 1;
    }

    return 0;
}
