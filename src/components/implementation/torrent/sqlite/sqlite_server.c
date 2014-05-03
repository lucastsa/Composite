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
#include <stdio.h>
#include <string.h>
#include <sqlite3.h>

static long evt_sqlite = 0;
static td_t TD_SQLITE = 0;
static sqlite3 *SQLITE_DB;

static int MAX_SIZE = 1024*2;

struct sqlite_metadata{
    void *value;
    cbufp_t cb;
};


void
cos_init(void)
{
    int rc;

    //Tor init functions
    torlib_init();

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
    char *bufdata;
    struct sqlite_metadata *smetada;

    t = tor_lookup(td);
    if (!t) ERR_THROW(-EINVAL, done);

    //Recovering the data from the torrent
    smetada = t->data;
    ret = smetada->cb; //Cbuf
    stmt = smetada->value; //Data

    //Recovering the data from cbuf
    bufdata = (char *)cbuf2buf(ret,MAX_SIZE);
    if (!bufdata) ERR_THROW(-EINVAL, done);
    *sz = MAX_SIZE;

    rc = sqlite3_step(stmt);

    // No results
    if (rc == SQLITE_DONE) {
        bufdata[0] = SQLITE_DONE;
        *sz = 1;
        goto done;
    }
    else if (rc == SQLITE_ROW) {
        int i, column_count;
        char * cur_column_name;
        int cur_column_type;
        void * cur_column_value;
        int idx=1;

        bufdata[0] = SQLITE_ROW;

        /**
        [RESULT_CODE:1] [COLUMN_COUNT:4+1] ( [COLUMN_TYPE:4+1][COLUMN_NAME_LENGTH:4+1][COLUMN_NAME:???+1] ...)
        */

        // column count in buffer
        column_count = sqlite3_column_count(stmt);
        snprintf(&bufdata[idx], sizeof(column_count) + 1, "%d", column_count);
        idx += sizeof(column_count) + 1;

        // for each column
        for (i=0; i < column_count; i++) {
            cur_column_type = sqlite3_column_type(stmt, i);

            // column type
            snprintf(&bufdata[idx], sizeof(cur_column_type) + 1, "%d", cur_column_type);
            idx += sizeof(cur_column_type) + 1;

            /*********************/
            cur_column_name = sqlite3_column_name(stmt, i);

            // column name length
            snprintf(&bufdata[idx], sizeof(int) + 1, "%d", (int)strlen(cur_column_name));
            idx += sizeof(int) + 1;

            // column name value
            strcpy(&bufdata[idx], cur_column_name);
            idx += strlen(cur_column_name) + 1;

            switch (cur_column_type) {
                case SQLITE_INTEGER:
                    snprintf(&bufdata[idx], sizeof(int) + 1, "%d", sqlite3_column_int(stmt, i));
                    idx += sizeof(int) + 1;
                    break;

                case SQLITE_FLOAT:
                    // TODO: CONVERT DOUBLE TO CHAR ARRAY (assume DOUBLE 64-bit)
                    break;

                case SQLITE_BLOB:
                    snprintf(&bufdata[idx], sizeof(int) + 1, "%d", sqlite3_column_bytes(stmt, i));
                    idx += sizeof(int) + 1;

                    memcpy(&bufdata[idx], sqlite3_column_blob(stmt, i), sqlite3_column_bytes(stmt, i));
                    idx += sqlite3_column_bytes(stmt, i);
                    break;

                case SQLITE_NULL:
                    break;

                case SQLITE_TEXT:
                    snprintf(&bufdata[idx], sizeof(int) + 1, "%d", sqlite3_column_bytes(stmt, i));
                    idx += sizeof(int) + 1;

                    strcpy(&bufdata[idx], sqlite3_column_text(stmt, i));
                    idx += sqlite3_column_bytes(stmt, i) + 1;
            }
        }

        *sz = idx;
        goto done;
    }
    else ERR_THROW(-EINVAL, done);
done:
    evt_trigger(cos_spd_id(),t->evtid);
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
    cbuf_free(cbuf2buf(smetada->cb,MAX_SIZE));

    free(smetada);

    sqlite3_finalize(smetada->value);

done:
    return;
};

int tmerge(spdid_t spdid, td_t td, td_t td_into, char *param, int len){return -ENOTSUP;}
int tread(spdid_t spdid, td_t td, int cbid, int sz){return -ENOTSUP;}
int twrite(spdid_t spdid, td_t td, int cbid, int sz){return -ENOTSUP;}
int twritep(spdid_t spdid, td_t td, int cbid, int sz) {return -ENOTSUP;}
