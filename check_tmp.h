#ifndef __CHECK_TMP_H
#define __CHECK_TMP_H

#include <sys/types.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/param.h>
#include<asm-generic/ioctls.h> 
#include<asm-generic/termios.h>
#include<string.h>
#include<time.h>
/* #include <sys/ttycom.h> */
#include <dirent.h>
#include <fcntl.h>
#include <grp.h>
#include <pwd.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#if (__FreeBSD_version >= 500001) || (__FreeBSD_version >= 400019 && __FreeBSD_version < 500000)
#include <sys/event.h>
#include <sys/time.h>
#include <err.h>
#endif

#if defined (REGEXPR_3G)
#include <regexpr.h>
#else
#if defined (REGCOMP_3C)
#include <regex.h>
#endif
#endif

#include <syslog.h>

#define ADDITION 1
#define SUBTRACTION 0

#define MODESIZE 11

struct listStruct {
  char filename[MAXNAMLEN + 1];
  struct stat statbuf;
  struct listStruct *prev;
  struct listStruct *next;
};  

int syslogflag; /* grody after thought */

#endif
