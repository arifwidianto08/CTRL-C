// db/migrations/migration_template.c

#include "migrator.h"
#include <stdio.h>
#include <string.h>

void up()
{
    const char *users_csv = "db/data/users.csv";
    const char *fishes_csv = "db/data/fishes.csv";

    // Migration logic: Add your changes here
    printf("Migration: Up\n");
    drop_column(users_csv, "Email");
    rename_column(users_csv, "Phone", "Address");

    create_table(fishes_csv);
    add_column(fishes_csv, "Name", "default");
    add_column(fishes_csv, "Price", "900000");
}

void down()
{
    const char *users_csv = "db/data/users.csv";
    const char *fishes_csv = "db/data/fishes.csv";

    // Rollback logic: Add your changes here
    printf("Rollback: Down\n");
    add_column(users_csv, "Email", "default");
    rename_column(users_csv, "Address", "Phone");

    drop_table(fishes_csv);
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
