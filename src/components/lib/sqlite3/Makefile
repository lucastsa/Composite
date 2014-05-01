CC := ../dietlibc-0.29/bin-i386/diet gcc
AR := ar
CFLAGS := -Wall -fno-stack-protector
SQLITEFLAGS := -DSQLITE_THREADSAFE=0 -DSQLITE_OMIT_LOCALTIME -DSQLITE_TEMP_STORE=3 -DSQLITE_OMIT_LOAD_EXTENSION -DHAVE_UTIME -DHAVE_MREMAP=0 -DSQLITE_MEMDEBUG

.PHONY: all clean

all: libsqlite3.a

clean:
	rm -f *.o libsqlite3.a

%.o: %.c sqlite3.h
	$(CC) $< -o $@ -c $(CFLAGS) $(SQLITEFLAGS)

libsqlite3.a: sqlite3.o test_vfstrace.o
	$(AR) rcs $@ $^
