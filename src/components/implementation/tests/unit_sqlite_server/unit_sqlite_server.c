/**
 * Copyright 2014 by
 *      Lucas Sa,   lucatsa@gwmail.gwu.edu
 *      Ney Mello,  neymello@gwmail.gwu.edu
 *
 * Redistribution of this file is permitted under the GNU General
 * Public License v2.
 */

#include <cos_component.h>
#include <print.h>
#include <sched.h>
#include <cbuf.h>
#include <evt.h>
#include <torrent.h>
#include <evt.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

/* helper functions to select statements (deserializes buffer) */
#include "../../torrent/sqlite/sqlite_server.h"

void cos_init(void)
{
    td_t t1, t2, t3;
    long evt1, evt2, evt3;
    char *query1 = "CREATE TABLE students(id INTEGER PRIMARY KEY, name TEXT);";
    char *query2 = "INSERT INTO students SELECT NULL as 'id', 'ney mello' as 'name' UNION SELECT NULL, 'lucas sa';";
    char *query3 = "SELECT * FROM students;";
    cbufp_t cb1,cb2,cb3;
    int off,sz;
    char *result, *bufdata;

    printc("#SQLITE: UNIT TEST Unit tests for torrents...\n");

    evt1 = evt_split(cos_spd_id(), 0, 0);
    assert(evt1 > 0);

    printc("\n\tSQLITE: Debugging... Calling tsplit\n");
    t1 = tsplit(cos_spd_id(), td_root, query1, strlen(query1), TOR_ALL, evt1);
    if (t1 < 1) {
        printc("#SQLITE: UNIT TEST FAILED: split failed %d\n", t1);
        return;
    }
    printc("\n\tSQLITE: Debugging... Returning from tsplit %d\n",t1);

    printc("\n\tSQLITE: Debugging... Calling treadp\n");
    cb1 = treadp(cos_spd_id(), t1, &off, &sz);
    printc("\n\tSQLITE: Debugging... Returning from tread\n");

    printc("#SQLITE: Reading the result -- Size: %d\n",sz);
    result = cbuf2buf(cb1,sz);
    printc("#SQLITE: Result is %hhd\n", result[0]);

    printc("#SQLITE: Releasing the torrent\n");
    trelease(cos_spd_id(), t1);


    /*****
        TEST 2
    ****/
    evt2 = evt_split(cos_spd_id(), 0, 0);
    assert(evt2 > 0);

    printc("\n\tSQLITE: Debugging 2... Calling tsplit\n");
    t2 = tsplit(cos_spd_id(), td_root, query2, strlen(query2), TOR_ALL, evt2);
    if (t2 < 1) {
        printc("#SQLITE: UNIT TEST FAILED: split failed %d\n", t2);
        return;
    }
    printc("\n\tSQLITE: Debugging 2... Returning from tsplit %d\n",t2);

    printc("\n\tSQLITE: Debugging 2... Calling treadp\n");
    cb2 = treadp(cos_spd_id(), t2, &off, &sz);
    printc("\n\tSQLITE: Debugging 2... Returning from tread\n");

    printc("#SQLITE: Reading the result -- Size: %d\n",sz);
    result = cbuf2buf(cb2,sz);
    printc("#SQLITE: Result is %hhd\n", result[0]);

    printc("#SQLITE: Releasing the torrent\n");
    trelease(cos_spd_id(), t2);

    /*****
        TEST 3
    ****/
    evt3 = evt_split(cos_spd_id(), 0, 0);
    assert(evt3 > 0);

    printc("\n\tSQLITE: Debugging 3... Calling tsplit\n");
    t3 = tsplit(cos_spd_id(), td_root, query3, strlen(query3), TOR_ALL, evt3);
    if (t3 < 1) {
        printc("#SQLITE: UNIT TEST FAILED: split failed %d\n", t3);
        return;
    }
    printc("\n\tSQLITE: Debugging 3... Returning from tsplit %d\n",t3);

    while (1) {
        printc("\n\tSQLITE: Debugging 3... Calling treadp\n");
        cb3 = treadp(cos_spd_id(), t3, &off, &sz);
        printc("\n\tSQLITE: Debugging 3... Returning from tread\n");

        printc("#SQLITE: Reading the result -- Size: %d\n",sz);
        bufdata = cbuf2buf(cb3,sz);

        if (bufdata[0] != SQLITE_ROW)
            break;

        cos_sqlite3_result * res;
        res = cos_sqlite3_build_result(bufdata);

        printc("#SQLITE: Result code: %d\n", res->result_code);
        printc("#SQLITE: %s: %d\n", res->column_names[0], *((int *)res->column_values[0]));
        printc("#SQLITE: %s: %s\n", res->column_names[1], (char *)res->column_values[1]);
    }
    /*********************************************************************/

    printc("#SQLITE: Releasing the torrent\n");
    trelease(cos_spd_id(), t3);

    return;
}
