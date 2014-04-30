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

void cos_init(void)
{
	printc("\n\n\n#SQLITE: initialization!!!\n\n\n");

	return;
}

td_t tsplit(spdid_t spdid, td_t tid, char *param, int len, tor_flags_t tflags, long evtid){
    printc("#SQLITE: tsplit ok\n");

    return -ENOTSUP;
}

int treadp(spdid_t spdid, td_t td, int *off, int *sz){
    printc("#SQLITE: treadp ok\n");

    return 0;
}

int twritep(spdid_t spdid, td_t td, int cbid, int sz){

    return 0;
}

void trelease(spdid_t spdid, td_t tid){

    return;
};


int tmerge(spdid_t spdid, td_t td, td_t td_into, char *param, int len){return -ENOTSUP;}
int tread(spdid_t spdid, td_t td, int cbid, int sz){return -ENOTSUP;}
int twrite(spdid_t spdid, td_t td, int cbid, int sz){return -ENOTSUP;}
int trmeta(spdid_t spdid, td_t td, const char *key, unsigned int klen, char *retval, unsigned int max_rval_len){return -ENOTSUP;}
int twmeta(spdid_t spdid, td_t td, const char *key, unsigned int klen, const char *val, unsigned int vlen){return -ENOTSUP;}