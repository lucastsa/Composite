/**
 * Copyright 2014 by
 *      Lucas Sa,   lucatsa@gwmail.gwu.edu
 *      Ney Mello,  neymello@gwmail.gwu.edu
 *
 * Redistribution of this file is permitted under the GNU General
 * Public License v2.
 */

#include <cos_component.h>
#include <torrent.h>
#include <torlib.h>

#include <print.h>
#include <sched.h>
#include <cbuf.h>
#include <evt.h>

#include "sqlite3.h"

static long evt_sqlite = 0;
static td_t TD_SQLITE = 0;
static sqlite3 *SQLITE_DB;


void
cos_init(void)
{
    int rc;

	printc("\n\n\n#SQLITE: initialization!!!\n\n\n");

    printc("#SQLITE: Creating the database\n");
    rc = sqlite3_open_v2(":memory:", &SQLITE_DB, SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE | SQLITE_OPEN_PRIVATECACHE, NULL);
    if (rc) {
        printc("#SQLITE: SQL error: Can't open database (%d): %s\n", rc, sqlite3_errmsg(SQLITE_DB));
        sqlite3_close(SQLITE_DB);

        return;
    }

	return;
}

td_t
tsplit(spdid_t spdid, td_t tid, char *param, int len, tor_flags_t tflags, long evtid)
{
    printc("#SQLITE: Tsplitting ...\n");

    int rc, ret;
    sqlite3_stmt *stmt;
    struct torrent *nt, *t;

    t = tor_lookup(tid);
    if (!t) ERR_THROW(-EINVAL, done);

    // preparing sql
    printc("#SQLITE: Preparing the query statement\n");
    rc = sqlite3_prepare_v2(SQLITE_DB, param, -1, &stmt, NULL);
    if (rc) {
        printc("#SQLITE: SQL error: %d : %s\n", rc, sqlite3_errmsg(SQLITE_DB));
        ERR_THROW(-EAGAIN, done);
    }

    nt = tor_alloc(&stmt, tflags);
    if (!nt) ERR_THROW(-ENOMEM, done);

    nt->evtid = evtid;
    ret = nt->td;
done:
    return ret;
}

int treadp(spdid_t spdid, td_t td, int *off, int *sz){
    printc("#SQLITE: Treading ...\n");

    cbufp_t ret;
    struct torrent *t;
    int rc;

    t = tor_lookup(td);
    if (!t) ERR_THROW(-EINVAL, done);

    printc("#SQLITE: Executing the query\n");
    while ((rc = sqlite3_step(t->data)) != SQLITE_DONE) {
        switch(rc) {
            case SQLITE_ROW:
                ret = rc;
            default:
                printc("#SQLITE: SQL error: %d : %s\n", rc, sqlite3_errmsg(SQLITE_DB));
                ERR_THROW(-EAGAIN, done);
        }

        evt_trigger(cos_spd_id(),t->evtid);
    }
done:
    return ret;
}

void
trelease(spdid_t spdid, td_t tid)
{
    printc("#SQLITE: Treleasing ...\n");
    struct torrent *t;
    int ret;

    t = tor_lookup(tid);
    if (!t) ERR_THROW(-EINVAL, done);

    printc("#SQLITE: Finialize the statement obejct.\n");
    sqlite3_finalize(t->data);

done:
    return;
};

int tmerge(spdid_t spdid, td_t td, td_t td_into, char *param, int len){return -ENOTSUP;}
int tread(spdid_t spdid, td_t td, int cbid, int sz){return -ENOTSUP;}
int twrite(spdid_t spdid, td_t td, int cbid, int sz){return -ENOTSUP;}
int twritep(spdid_t spdid, td_t td, int cbid, int sz) {return -ENOTSUP;}
int trmeta(spdid_t spdid, td_t td, const char *key, unsigned int klen, char *retval, unsigned int max_rval_len){return -ENOTSUP;}
int twmeta(spdid_t spdid, td_t td, const char *key, unsigned int klen, const char *val, unsigned int vlen){return -ENOTSUP;}