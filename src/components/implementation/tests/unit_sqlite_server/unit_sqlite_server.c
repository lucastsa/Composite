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

//#define VERBOSE 1
#ifdef VERBOSE
#define printv(fmt,...) printc(fmt, ##__VA_ARGS__)
#else
#define printv(fmt,...)
#endif

char buffer[1024];

void cos_init(void)
{
    td_t t1, t2;
    long evt1, evt2;
    char *params1 = "bar";
    char *params2 = "foo/";
    char *params3 = "foo/bar";
    char *data1 = "1234567890", *data2 = "asdf;lkj", *data3 = "asdf;lkj1234567890";
    char *query1 = "CREATE TABLE students(id INTEGER PRIMARY KEY, name TEXT);";
    cbufp_t cb1;
    int off,sz;

    printc("#SQLITE: UNIT TEST Unit tests for torrents...\n");

    // int a = 0, b = 0, c = 0;
    // c = treadp(cos_spd_id(), 0, &a, &b);

    // printc("#SQLITE: UNIT TEST Unit tests for torrents...\n");

    // printc("%d %d %d\n", a, b, c);

    evt1 = evt_split(cos_spd_id(), 0, 0);
    evt2 = evt_split(cos_spd_id(), 0, 0);
    assert(evt1 > 0 && evt2 > 0);

    printc("\n\n\n\tSQLITE: Debugging... Calling tsplit\n\n\n");
    t1 = tsplit(cos_spd_id(), td_root, query1, strlen(query1), TOR_ALL, evt1);
    if (t1 < 1) {
        printc("#SQLITE: UNIT TEST FAILED: split failed %d\n", t1);
        return;
    }
    printc("\n\n\n\tSQLITE: Debugging... Returning from tsplit\n\n\n");

    // ret1 = tread_pack(cos_spd_id(), t1, buffer, 1023);

    printc("\n\n\n\tSQLITE: Debugging... Calling treadp\n\n\n");
    cb1 = treadp(cos_spd_id(), t1, &off, &sz);
    printc("\n\n\n\tSQLITE: Debugging... Returning from tread\n\n\n");

    printc("\n\n\n\tSQLITE: Debugging... sleeping ...\n\n\n");
    // sleep(1);
    printc("\n\n\n\tSQLITE: Debugging... waking ...\n\n\n");

    // evt_wait(cos_spd_id(), evt1);
    // evt_wait(cos_spd_id(), evt1);

    printc("#SQLITE: Releasing the torrent\n");
    trelease(cos_spd_id(), t1);

    // printc("UNIT TEST PASSED: split->release\n");

    // t1 = tsplit(cos_spd_id(), td_root, params2, strlen(params2), TOR_ALL, evt1);
    // if (t1 < 1) {
    //     printc("UNIT TEST FAILED: split2 failed %d\n", t1); return;
    // }
    // t2 = tsplit(cos_spd_id(), t1, params1, strlen(params1), TOR_ALL, evt2);
    // if (t2 < 1) {
    //     printc("UNIT TEST FAILED: split3 failed %d\n", t2); return;
    // }

    // ret1 = twrite_pack(cos_spd_id(), t1, data1, strlen(data1));
    // ret2 = twrite_pack(cos_spd_id(), t2, data2, strlen(data2));
    // printv("write %d & %d, ret %d & %d\n", strlen(data1), strlen(data2), ret1, ret2);

    // trelease(cos_spd_id(), t1);
    // trelease(cos_spd_id(), t2);
    // printc("UNIT TEST PASSED: 2 split -> 2 write -> 2 release\n");

    // t1 = tsplit(cos_spd_id(), td_root, params2, strlen(params2), TOR_ALL, evt1);
    // t2 = tsplit(cos_spd_id(), t1, params1, strlen(params1), TOR_ALL, evt2);
    // if (t1 < 1 || t2 < 1) {
    //     printc("UNIT TEST FAILED: later splits failed\n");
    //     return;
    // }

    // ret1 = tread_pack(cos_spd_id(), t1, buffer, 1023);
    // if (ret1 > 0) buffer[ret1] = '\0';
    // printv("read %d (%d): %s (%s)\n", ret1, strlen(data1), buffer, data1);
    // assert(!strcmp(buffer, data1));
    // assert(ret1 == strlen(data1));
    // buffer[0] = '\0';

    // ret1 = tread_pack(cos_spd_id(), t2, buffer, 1023);
    // if (ret1 > 0) buffer[ret1] = '\0';
    // assert(!strcmp(buffer, data2));
    // assert(ret1 == strlen(data2));
    // printv("read %d: %s\n", ret1, buffer);
    // buffer[0] = '\0';

    // trelease(cos_spd_id(), t1);
    // trelease(cos_spd_id(), t2);

    // printc("UNIT TEST PASSED: 2 split -> 2 read -> 2 release\n");

    // t1 = tsplit(cos_spd_id(), td_root, params3, strlen(params3), TOR_ALL, evt1);
    // ret1 = tread_pack(cos_spd_id(), t1, buffer, 1023);
    // if (ret1 > 0) buffer[ret1] = '\0';
    // printv("read %d: %s\n", ret1, buffer);
    // assert(!strcmp(buffer, data2));
    // assert(ret1 == strlen(data2));
    // printc("UNIT TEST PASSED: split with absolute addressing -> read\n");
    // buffer[0] = '\0';

    // ret1 = twrite_pack(cos_spd_id(), t1, data1, strlen(data1));
    // printv("write %d, ret %d\n", strlen(data1), ret1);

    // trelease(cos_spd_id(), t1);
    // t1 = tsplit(cos_spd_id(), td_root, params3, strlen(params3), TOR_ALL, evt1);
    // ret1 = tread_pack(cos_spd_id(), t1, buffer, 1023);
    // if (ret1 > 0 && ret1 < 1024) buffer[ret1] = '\0';
    // printv("read %d: %s (%s)\n", ret1, buffer, data3);
    // assert(ret1 == strlen(data2)+strlen(data1));
    // assert(!strcmp(buffer, data3));
    // buffer[0] = '\0';
    // printc("UNIT TEST PASSED: writing to an existing file\n");

    // printc("UNIT TEST ALL PASSED\n");

    return;
}
