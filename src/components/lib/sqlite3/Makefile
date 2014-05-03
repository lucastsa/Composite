CC := ../dietlibc-0.29/bin-i386/diet gcc
AR := ar
INCLUDES := -I../../include -I../../../kernel/include -I../../../kernel/include/shared
CFLAGS := -Wall -fno-stack-protector $(INCLUDES)
SQLITEFLAGS := -DSQLITE_OS_OTHER=1 -DSQLITE_THREADSAFE=0 -DSQLITE_OMIT_LOCALTIME -DSQLITE_TEMP_STORE=3 -DSQLITE_OMIT_LOAD_EXTENSION -DHAVE_UTIME -DHAVE_MREMAP=0
INCLUDEDIR := ../../include

.PHONY: all clean

all: libsqlite3.a

clean:
	rm -f *.o libsqlite3.a

%.o: %.c sqlite3.h
	$(CC) $< -o $@ -c $(CFLAGS) $(SQLITEFLAGS)

libsqlite3.a: sqlite3.o os_composite.o test_vfstrace.o
	$(AR) rcs $@ $^
	cp sqlite3.h $(INCLUDEDIR)
