/**
 * Copyright 2014 by Lucas Sa and Ney Mello
 *
 * Redistribution of this file is permitted under the GNU General
 * Public License v2.
 */

 /**
 * This is the Composite' SQLite Virtual File System (VFS) skeleton.
 * Most functions is this file are no-ops, but return appropriate error
 * codes to indicate that this VFS is not capable of performing I/O.
 * A demo VFS by SQLite developers was used as the code base for this VFS.
 *
 * Whe using this VFS, SQLite will be able to initiate an in-memory database
 * only, i.e., using:
 *
 * sqlite3_open_v2(":memory:", ..., ..., "cos");
 *
 * By default, when SQLite is compiled with this VFS, this file will register
 * itself as the default VFS. If you change the default VFS, you can use the
 * VFS name "cos" to initiate a database using this VFS.
 */

#define SQLITE_OS_COMPOSITE 0

#ifdef SQLITE_OS_COMPOSITE

#include "sqlite3.h"

#include <assert.h>
#include <string.h>
#include <time.h>       /* used for time */
#include <unistd.h>     /* used for sleep */
// #include <sys/types.h>
// #include <sys/stat.h>
// #include <sys/file.h>
// #include <sys/param.h>
#include <errno.h>

/** Macros to supress explicitly not used parameters / variables */
#define UNUSED_PARAMETER(x) (void)(x)
#define UNUSED_VARIABLE(x) (void)(x)

/*
** Size of the write buffer used by journal files in bytes.
*/
#ifndef SQLITE_COSVFS_BUFFERSZ
# define SQLITE_COSVFS_BUFFERSZ 8192
#endif

/*
** The maximum pathname length supported by this VFS.
*/
#define MAXPATHNAME 512

/*
** When using this VFS, the sqlite3_file* handles that SQLite uses are
** actually pointers to instances of type cos_sqlite3_File.
*/
typedef struct cos_sqlite3_File cos_sqlite3_File;
struct cos_sqlite3_File {
  sqlite3_file base;              /* Base class. Must be first. */
  int fd;                         /* File descriptor */

  char *aBuffer;                  /* Pointer to malloc'd buffer */
  int nBuffer;                    /* Valid bytes of data in zBuffer */
  sqlite3_int64 iBufferOfst;      /* Offset in file of zBuffer[0] */
};

/*
** Close a file. This function is a no-op.
*/
static int cos_sqlite3_Close(sqlite3_file *pFile){
  return SQLITE_IOERR_CLOSE; // Not implemented
}

/*
** Read data from a file. This function is a no-op.
*/
static int cos_sqlite3_Read(
  sqlite3_file *pFile,
  void *zBuf,
  int iAmt,
  sqlite_int64 iOfst
){
  return SQLITE_IOERR_READ; // Not implemented
}

/*
** Write data to a crash-file. This function is a no-op.
*/
static int cos_sqlite3_Write(
  sqlite3_file *pFile,
  const void *zBuf,
  int iAmt,
  sqlite_int64 iOfst
){
  return SQLITE_IOERR_WRITE; // Not implemented
}

/*
** Truncate a file. This is a no-op for this VFS.
*/
static int cos_sqlite3_Truncate(sqlite3_file *pFile, sqlite_int64 size){
  return SQLITE_IOERR_TRUNCATE; // Not implemented
}

/*
** Sync the contents of the file to the persistent media. This function is no-op.
*/
static int cos_sqlite3_Sync(sqlite3_file *pFile, int flags){
  return SQLITE_IOERR_FSYNC; // Not implemented
}

/*
** Write the size of the file in bytes to *pSize. This is no-op too.
*/
static int cos_sqlite3_FileSize(sqlite3_file *pFile, sqlite_int64 *pSize){
  return SQLITE_IOERR; // Not implemented
}

/*
** Locking functions. The xLock() and xUnlock() methods are both no-ops.
** The xCheckReservedLock() always indicates that no other process holds
** a reserved lock on the database file. This ensures that if a hot-journal
** file is found in the file-system it is rolled back.
*/
static int cos_sqlite3_Lock(sqlite3_file *pFile, int eLock){
  return SQLITE_IOERR_UNLOCK; // Not implemented
}
static int cos_sqlite3_Unlock(sqlite3_file *pFile, int eLock){
  return SQLITE_IOERR_LOCK; // Not implemented
}
static int cos_sqlite3_CheckReservedLock(sqlite3_file *pFile, int *pResOut){
  *pResOut = 0;
  return SQLITE_IOERR_CHECKRESERVEDLOCK; // Not implemented
}

/*
** No xFileControl() verbs are implemented by this VFS.
*/
static int cos_sqlite3_FileControl(sqlite3_file *pFile, int op, void *pArg){
  return SQLITE_IOERR; // Not implemented
}

/*
** The xSectorSize() and xDeviceCharacteristics() methods. These two
** may return special values allowing SQLite to optimize file-system
** access to some extent. But it is also safe to simply return 0.
*/
static int cos_sqlite3_SectorSize(sqlite3_file *pFile){
  return 0;
}
static int cos_sqlite3_DeviceCharacteristics(sqlite3_file *pFile){
  return 0;
}

/*
** Open a file handle.
*/
static int cos_sqlite3_Open(
  sqlite3_vfs *pVfs,              /* VFS */
  const char *zName,              /* File to open, or 0 for a temp file */
  sqlite3_file *pFile,            /* Pointer to cos_sqlite3_File struct to populate */
  int flags,                      /* Input SQLITE_OPEN_XXX flags */
  int *pOutFlags                  /* Output SQLITE_OPEN_XXX flags (or NULL) */
){
  static const sqlite3_io_methods cos_sqlite3_io = {
    1,                                    /* iVersion */
    cos_sqlite3_Close,                    /* xClose */
    cos_sqlite3_Read,                     /* xRead */
    cos_sqlite3_Write,                    /* xWrite */
    cos_sqlite3_Truncate,                 /* xTruncate */
    cos_sqlite3_Sync,                     /* xSync */
    cos_sqlite3_FileSize,                 /* xFileSize */
    cos_sqlite3_Lock,                     /* xLock */
    cos_sqlite3_Unlock,                   /* xUnlock */
    cos_sqlite3_CheckReservedLock,        /* xCheckReservedLock */
    cos_sqlite3_FileControl,              /* xFileControl */
    cos_sqlite3_SectorSize,               /* xSectorSize */
    cos_sqlite3_DeviceCharacteristics     /* xDeviceCharacteristics */
  };

  UNUSED_VARIABLE(cos_sqlite3_io);

  return SQLITE_CANTOPEN; // Not implemented
}

/*
** Delete the file identified by argument zPath. If the dirSync parameter
** is non-zero, then ensure the file-system modification to delete the
** file has been synced to disk before returning.
*/
static int cos_sqlite3_Delete(sqlite3_vfs *pVfs, const char *zPath, int dirSync){
  return SQLITE_IOERR_DELETE;
}

/*
** Query the file-system to see if the named file exists, is readable or
** is both readable and writable.
*/
static int cos_sqlite3_Access(
  sqlite3_vfs *pVfs,
  const char *zPath,
  int flags,
  int *pResOut
){
  return SQLITE_IOERR_ACCESS; // Not implemented
}

/*
** Argument zPath points to a nul-terminated string containing a file path.
** If zPath is an absolute path, then it is copied as is into the output
** buffer. Otherwise, if it is a relative path, then the equivalent full
** path is written to the output buffer.
**
** This function assumes that paths are UNIX style. Specifically, that:
**
**   1. Path components are separated by a '/'. and
**   2. Full paths begin with a '/' character.
*/
static int cos_sqlite3_FullPathname(
  sqlite3_vfs *pVfs,              /* VFS */
  const char *zPath,              /* Input path (possibly a relative path) */
  int nPathOut,                   /* Size of output buffer in bytes */
  char *zPathOut                  /* Pointer to output buffer */
){
  return SQLITE_IOERR; // Not implemented
}

/*
** The following four VFS methods:
**
**   xDlOpen
**   xDlError
**   xDlSym
**   xDlClose
**
** are supposed to implement the functionality needed by SQLite to load
** extensions compiled as shared objects. This simple VFS does not support
** this functionality, so the following functions are no-ops.
*/
static void *cos_sqlite3_DlOpen(sqlite3_vfs *pVfs, const char *zPath){
  return 0;
}
static void cos_sqlite3_DlError(sqlite3_vfs *pVfs, int nByte, char *zErrMsg){
  sqlite3_snprintf(nByte, zErrMsg, "Loadable extensions are not supported");
  zErrMsg[nByte-1] = '\0';
}
static void (*cos_sqlite3_DlSym(sqlite3_vfs *pVfs, void *pH, const char *z))(void){
  return 0;
}
static void cos_sqlite3_DlClose(sqlite3_vfs *pVfs, void *pHandle){
  return;
}

/*
** Parameter zByte points to a buffer nByte bytes in size. Populate this
** buffer with pseudo-random data.
*/
#include <print.h>
static int cos_sqlite3_Randomness(sqlite3_vfs *pVfs, int nByte, char *zByte){
  printc("HELLO RANDMONESS\n");
  return SQLITE_OK;
}

/*
** Sleep for at least nMicro microseconds. Return the (approximate) number
** of microseconds slept for.
*/
static int cos_sqlite3_Sleep(sqlite3_vfs *pVfs, int nMicro){
  sleep(nMicro / 1000000);
  usleep(nMicro % 1000000);
  return nMicro;
}

/*
** Set *pTime to the current UTC time expressed as a Julian day. Return
** SQLITE_OK if successful, or an error code otherwise.
**
**   http://en.wikipedia.org/wiki/Julian_day
**
** This implementation is not very good. The current time is rounded to
** an integer number of seconds. Also, assuming time_t is a signed 32-bit
** value, it will stop working some time in the year 2038 AD (the so-called
** "year 2038" problem that afflicts systems that store time this way).
*/
static int cos_sqlite3_CurrentTime(sqlite3_vfs *pVfs, double *pTime){
  time_t t = time(0);
  *pTime = t/86400.0 + 2440587.5;
  return SQLITE_OK;
}

/*
** This function returns a pointer to the VFS implemented in this file.
** To make the VFS available to SQLite:
**
**   sqlite3_vfs_register(sqlite3_cos_sqlite3_vfs(), 0);
*/
sqlite3_vfs *sqlite3_cos_sqlite3_vfs(void){
  static sqlite3_vfs cos_sqlite3_vfs = {
    1,                                    /* iVersion */
    sizeof(cos_sqlite3_File),             /* szOsFile */
    MAXPATHNAME,                          /* mxPathname */
    0,                                    /* pNext */
    "cos",                                /* zName */
    0,                                    /* pAppData */
    cos_sqlite3_Open,                     /* xOpen */
    cos_sqlite3_Delete,                   /* xDelete */
    cos_sqlite3_Access,                   /* xAccess */
    cos_sqlite3_FullPathname,             /* xFullPathname */
    cos_sqlite3_DlOpen,                   /* xDlOpen */
    cos_sqlite3_DlError,                  /* xDlError */
    cos_sqlite3_DlSym,                    /* xDlSym */
    cos_sqlite3_DlClose,                  /* xDlClose */
    cos_sqlite3_Randomness,               /* xRandomness */
    cos_sqlite3_Sleep,                    /* xSleep */
    cos_sqlite3_CurrentTime,              /* xCurrentTime */
  };
  return &cos_sqlite3_vfs;
}

#ifdef SQLITE_OS_COMPOSITE_TEST

#include <print.h>

extern int vfstrace_register(
   const char *zTraceName,
   const char *zOldVfsName,
   int (*xOut)(const char*,void*),
   void *pOutArg,
   int makeDefault
);

int traceOutput(const char *zMessage, void *pAppData) {
    printc("#SQLITE_CALL: ");
    printc(zMessage);
    printc("\n");

    return 0;
}

#endif /* SQLITE_OS_COMPOSITE_TEST */

/**
 * Initialize the operating system interface.
*/
int sqlite3_os_init(void) {
  // sqlite3_config(SQLITE_CONFIG_HEAP, pBuf, szBuf, mnReq);

  /* register Composite's VFS as the default VFS */
  sqlite3_vfs_register(sqlite3_cos_sqlite3_vfs(), 1);

#ifdef SQLITE_OS_COMPOSITE_TEST
  vfstrace_register("VFSTRACE", NULL, traceOutput, NULL, 1);

  if (!sqlite3_vfs_find("VFSTRACE")) return SQLITE_ERROR;
#endif /* SQLITE_OS_COMPOSITE_TEST */

  return SQLITE_OK;
}

/**
 * Shutdown the operating system interface.
 *
 * This function is a no-op.
*/
int sqlite3_os_end(void){
  return SQLITE_OK;
}

#endif /* SQLITE_OS_COMPOSITE */