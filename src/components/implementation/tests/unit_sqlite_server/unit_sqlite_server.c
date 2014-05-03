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

char buffer[1024];

void cos_init(void)
{
    td_t t1, t2;
    long evt1, evt2;
    char *query1 = "CREATE TABLE students(id INTEGER PRIMARY KEY, name TEXT);";
    char *query2 = "INSERT INTO students VALUES (NULL,'ney mello');";
    cbufp_t cb1,cb2;
    int off,sz;
    void *result;

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
    printc("#SQLITE: Result is %s\n",result);

    printc("#SQLITE: Releasing the torrent\n");
    trelease(cos_spd_id(), t1);


    /*****
        TEST 2
    ****/
    evt2 = evt_split(cos_spd_id(), 0, 0);
    assert(evt2 > 0);

    printc("\n\tSQLITE: Debugging 2... Calling tsplit\n");
    t2 = tsplit(cos_spd_id(), td_root, query2, strlen(query2), TOR_ALL, evt2
    );
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
    printc("#SQLITE: Result is %s\n",result);

    printc("#SQLITE: Releasing the torrent\n");
    trelease(cos_spd_id(), t2);

    return;
}
