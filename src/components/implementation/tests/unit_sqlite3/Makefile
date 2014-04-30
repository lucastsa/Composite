C_OBJS=sqlite3.o test_sqlite3.o test_vfstrace.o
ASM_OBJS=
COMPONENT=unit_sqlite3.o
INTERFACES=
DEPENDENCIES=printc torrent sched cbufp cbuf_c evt mem_mgr_large valloc lock
IF_LIB=
SQLITEFLAGS=-DSQLITE_THREADSAFE=0 -DSQLITE_OMIT_LOCALTIME -DSQLITE_TEMP_STORE=3 -DSQLITE_OMIT_LOAD_EXTENSION -DHAVE_UTIME -DHAVE_MREMAP=0

include ../../Makefile.subsubdir

CFLAGS+= $(SQLITEFLAGS)