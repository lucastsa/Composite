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

#include <cbuf.h>
#include <print.h>
#include <cos_synchronization.h>
#include <evt.h>
#include <cos_alloc.h>
#include <cos_map.h>
#include <fs.h>
#include <sqlite3.h>

static long evt_sqlite = 0;
static td_t TD_SQLITE = 0;
static sqlite3 *SQLITE_DB;

static int MAX_SIZE = 1024;

struct sqlite_metadata{
    void *value;
    cbufp_t cb;
};

struct fsobj root;

void
cos_init(void)
{
    int rc;

    //Tor init functions
    torlib_init();
    fs_init_root(&root);
    root_torrent.data = &root;
    root.flags = TOR_READ | TOR_SPLIT;

    //Creating the database
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
    int rc, ret;
    sqlite3_stmt *stmt;
    struct torrent *nt, *t;
    struct sqlite_metadata *smetada = malloc(sizeof(struct sqlite_metadata));
    cbufp_t ncb;
    void *b;

    t = tor_lookup(tid);
    if (!t) ERR_THROW(-EINVAL, done);

    b = cbuf_alloc(MAX_SIZE, &ncb);
    if (!b) ERR_THROW(-EINVAL, done);

    // preparing sql
    rc = sqlite3_prepare_v2(SQLITE_DB, param, -1, &stmt, NULL);
    if (rc) ERR_THROW(-EAGAIN, done);

    //Set the metadata value
    smetada->value = stmt; //Data
    smetada->cb = ncb; //Cbuf

    //Allocate the torrent
    nt = tor_alloc(smetada, tflags);
    if (!nt) ERR_THROW(-ENOMEM, done);

    nt->evtid = evtid;

    ret = nt->td;
done:
    return ret;
}

int treadp(spdid_t spdid, td_t td, int *off, int *sz){
    cbufp_t ret;
    struct torrent *t;
    int rc;
    sqlite3_stmt *stmt;
    void *bufdata;
    struct sqlite_metadata *smetada;

    t = tor_lookup(td);
    if (!t) ERR_THROW(-EINVAL, done);

    //Recovering the data from the torrent
    smetada = t->data;
    ret = smetada->cb; //Cbuf
    stmt = smetada->value; //Data

    //Recovering the data from cbuf
    bufdata = cbuf2buf(ret,MAX_SIZE);
    if (!bufdata) ERR_THROW(-EINVAL, done);
    *sz = MAX_SIZE;

    //Looping in the results
    while ((rc = sqlite3_step(stmt)) != SQLITE_DONE) {
        switch(rc) {
            case SQLITE_ROW:
                //TODO: decide the structure that will be returned
                memcpy(bufdata,test,11) ;
            default:
                ERR_THROW(-EAGAIN, done);
        }

        //Triggering the event
        evt_trigger(cos_spd_id(),t->evtid);
    }
done:
    return ret;
}

void
trelease(spdid_t spdid, td_t tid)
{
    struct torrent *t;
    int ret;
    struct sqlite_metadata *smetada;

    t = tor_lookup(tid);
    if (!t) ERR_THROW(-EINVAL, done);

    //Recovering the data from the torrent
    smetada = t->data;

    //Free the cbuf related to this torrent
    cbuf_free(smetada->cb);

    free(smetada);

    sqlite3_finalize(smetada->value);

done:
    return;
};

int tmerge(spdid_t spdid, td_t td, td_t td_into, char *param, int len){return -ENOTSUP;}
int tread(spdid_t spdid, td_t td, int cbid, int sz){return -ENOTSUP;}
int twrite(spdid_t spdid, td_t td, int cbid, int sz){return -ENOTSUP;}
int twritep(spdid_t spdid, td_t td, int cbid, int sz) {return -ENOTSUP;}
// int trmeta(spdid_t spdid, td_t td, const char *key, unsigned int klen, char *retval, unsigned int max_rval_len){return -ENOTSUP;}
// int twmeta(spdid_t spdid, td_t td, const char *key, unsigned int klen, const char *val, unsigned int vlen){return -ENOTSUP;}