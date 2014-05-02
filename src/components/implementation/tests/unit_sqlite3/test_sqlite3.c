/**
 * Copyright 2014 by Lucas Sa and Ney Mello
 *
 * Redistribution of this file is permitted under the GNU General
 * Public License v2.
 */

#include <cos_component.h>
#include <print.h>
#include <sched.h>
#include <string.h>
#include <sqlite3.h>

#define printf printc

#define N_NAMES 2

char CREATE_STMT[] = "CREATE TABLE students(id INTEGER PRIMARY KEY, name TEXT);";
char INSERT_STMT[] = "INSERT INTO students(id, name) values(NULL, ?);";
char SELECT_STMT[] = "SELECT id, name FROM students;";
char SELECT_RANDOM_STMT[] = "SELECT RANDOM();";
char SELECT_DATETIME_STMT[] = "SELECT DATETIME();";

char * names[N_NAMES] = {"Lucas Sa", "Ney Mello"};

int data_handler(void *param, int argc, char **argv, char **column) {
    int i;

    printf("#SQLITE: Statement:\n %s\n\n", (char *)param);

    for (i=0; i<argc; i++)
        printf("%s: %s \n", column[i], argv[i]);

    return SQLITE_OK;
}


void cos_init(void) {
    sqlite3 *db;
    sqlite3_stmt *stmt;
    char * errmsg;
    int r, i;

    printf("#SQLITE spd: %ld\n", cos_spd_id());

    r = sqlite3_open_v2(":memory:", &db, SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE | SQLITE_OPEN_PRIVATECACHE, "cos");

    if (r) {
        printf("#SQLITE: Can't open database (%d): %s\n", r, sqlite3_errmsg(db));
        sqlite3_close(db);
        return;
    }

    // creates table
    printf("#SQLITE: executing prepare for CREATE\n");
    // r = sqlite3_exec(db, CREATE_STMT, data_handler, (void*)CREATE_STMT, &errmsg);
    r = sqlite3_prepare_v2(db, CREATE_STMT, -1, &stmt, NULL);
    if (r) {
        printf("#SQLITE: SQL error: %d : %s\n", r, sqlite3_errmsg(db));
        return;
    }

    printf("#SQLITE: executing step for CREATE\n");
    r = sqlite3_step(stmt);
    if (r != SQLITE_DONE) {
        printf("#SQLITE: SQL error: %d : %s\n", r, sqlite3_errmsg(db));
        return;
    }

    // preparing only once for the loop
    r = sqlite3_prepare_v2(db, INSERT_STMT, -1, &stmt, NULL);
    if (r) {
        printf("#SQLITE: SQL error: %d : %s\n", r, sqlite3_errmsg(db));
        return;
    }

    // inserting names
    for (i=0; i < N_NAMES; i++) {
        r = sqlite3_bind_text(stmt, 1, names[i], strlen(names[i]), SQLITE_STATIC);
        if (r)  {
            printf("#SQLITE: Bind error: %d : %s\n", r, sqlite3_errmsg(db));
            return;
        }

        r = sqlite3_step(stmt);
        if (r != SQLITE_DONE) {
            printf("#SQLITE: SQL error: %d : %s\n", r, sqlite3_errmsg(db));
            return;
        }

        // resetting the statement for next step
        r = sqlite3_reset(stmt);
        if (r)  {
            printf("#SQLITE: Reset error: %d : %s\n", r, sqlite3_errmsg(db));
            return;
        }
    }

    // finalize the statement to release resources
    sqlite3_finalize(stmt);

    // select values
    r = sqlite3_exec(db, SELECT_STMT, data_handler, (void*)SELECT_STMT, &errmsg);
    if (r) {
        printf("#SQLITE: SQL error: %s\n", errmsg);
        sqlite3_free(errmsg);
        return;
    }

    // select datetime() to test time stuff
    r = sqlite3_exec(db, SELECT_DATETIME_STMT, data_handler, (void*)SELECT_DATETIME_STMT, &errmsg);
    if (r) {
        printf("#SQLITE: SQL error: %s\n", errmsg);
        sqlite3_free(errmsg);
        return;
    }

    // select random() to rest randomness
    r = sqlite3_exec(db, SELECT_RANDOM_STMT, data_handler, (void*)SELECT_RANDOM_STMT, &errmsg);
    if (r) {
        printf("#SQLITE: SQL error: %s\n", errmsg);
        sqlite3_free(errmsg);
        return;
    }

    sqlite3_close(db);

    return;
}
