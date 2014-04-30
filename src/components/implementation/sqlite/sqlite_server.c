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
#include <sqlite3.h>

static long evt_sqlite = 0;
static td_t TD_SQLITE = td_null;
static sqlite3 *SQLITE_DB;


void
cos_init(void)
{
	printc("\n\n\n#SQLITE: initialization!!!\n\n\n");

	return;
}

td_t
tsplit(spdid_t spdid, td_t tid, char *param, int len, tor_flags_t tflags, long evtid)
{
    int rc;
    struct torrent *nt;

    printc("#SQLITE: tsplit ok\n");
    if (TD_SQLITE) {
        return TD_SQLITE;
    }

    printc("#SQLITE: Creating the database\n");
    rc = sqlite3_open_v2(":memory:", &SQLITE_DB, SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE | SQLITE_OPEN_PRIVATECACHE, NULL);
    if (rc) {
        printf("#SQLITE: Can't open database (%d): %s\n", rc, sqlite3_errmsg(SQLITE_DB));
        sqlite3_close(SQLITE_DB);
        return -EAGAIN;
    }

    nt = tor_alloc(NULL, tflags);
    if (!nt) ERR_THROW(-ENOMEM, done);
    nt->evtid = evtid;

    TD_SQLITE = nt->td;

    return TD_SQLITE;
}

int treadp(spdid_t spdid, td_t td, int *off, int *sz){
    printc("#SQLITE: treadp ok\n");

    return 0;
}

static int
sqlite3_server_exec(char * zSql)
{
    int rc, ret;
    sqlite3_stmt *stmt;

    // preparing sql
    rc = sqlite3_prepare_v2(SQLITE_DB, zSql, -1, &stmt, NULL);
    if (rc) {
        printc("#SQLITE: SQL error: %d : %s\n", r, sqlite3_errmsg(db));
        ERR_THROW(-EAGAIN, done);
    }

    while ((rc = sqlite3_step(stmt)) != SQLITE_DONE) {
        switch(rc) {
            case SQLITE_ROW:
                // TODO: write data to torrent;
            default:
                printc("#SQLITE: SQL error: %d : %s\n", r, sqlite3_errmsg(db));
                ERR_THROW(-EAGAIN, done);
        }
    }

    // TODO: write delimiter to torrent??

    // finalize the statement to release resources
    sqlite3_finalize(stmt);

    ret = SQLITE_OK;

done:
    return ret;
}

int
twritep(spdid_t spdid, td_t td, int cbid, int sz)
{
    int ret = -1;
    struct torrent *t;
    struct as_conn *ac;
    cbufp_t cb = cbid;

    if (tor_isnull(td)) return -EINVAL;
    LOCK();
    t = tor_lookup(td);
    if (!t) ERR_THROW(-EINVAL, done);
    assert(t->data);
    if (!(t->flags & TOR_WRITE)) ERR_THROW(-EACCES, done);

    ret = sqlite3_server_exec((char*)t->data);
done:
    UNLOCK();
    return ret;
}


void
trelease(spdid_t spdid, td_t tid)
{

    return;
};

int tmerge(spdid_t spdid, td_t td, td_t td_into, char *param, int len){return -ENOTSUP;}
int tread(spdid_t spdid, td_t td, int cbid, int sz){return -ENOTSUP;}
int twrite(spdid_t spdid, td_t td, int cbid, int sz){return -ENOTSUP;}
int trmeta(spdid_t spdid, td_t td, const char *key, unsigned int klen, char *retval, unsigned int max_rval_len){return -ENOTSUP;}
int twmeta(spdid_t spdid, td_t td, const char *key, unsigned int klen, const char *val, unsigned int vlen){return -ENOTSUP;}