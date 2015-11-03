# Makefile for temp-watch, based off of Hobbits netcat makefile.
# invoke with "make systype" -- if your systype is not defined try "generic"
# if you port to other systems please mail the diffs to mudge@l0pht.com

### PREDEFINES

# DEFAULTS, possibly overridden by <systype> recursive call:
# pick gcc if you'd rather , and/or do -g instead of -O if debugging
# debugging
# DFLAGS = -DTEST -DDEBUG
DFLAGS = -DREGEX
CFLAGS ?=-O2 -Wall -pedantic
XFLAGS =        # xtra cflags, set by systype targets
XLIBS =         # xtra libs if necessary?
# -Bstatic for sunos,  -static for gcc, etc.  You want this, trust me.
STATIC =
CC ?= gcc
#CC += $(CFLAGS)
# LD = $(CC) -s   # linker; defaults to stripped executables
LD = $(CC)
o = o           # object extension
CKTEMP_OBJS = check_tmp.o daemon.o list_utils.o usage.o
ALL = temp-watch

### BOGON-CATCHERS

bogus:
	@echo
	@echo "Usage:  make  <systype>  [options]"
	@echo "   <systype> can be: solaris25, solaris26, bsd, linux"
	@echo "                     bsd-static, linux-static, generic"
	@echo

### HARD TARGETS

temp-watch:	$(CKTEMP_OBJS) check_tmp.h
	$(LD) $(DFLAGS) $(XFLAGS) $(STATIC) -o temp-watch \
	$(CKTEMP_OBJS) $(XLIBS)

check_tmp.o :	check_tmp.c
	$(CC) $(DFLAGS) $(XFLAGS) $(STATIC) -c check_tmp.c

daemon.o :	daemon.c
	$(CC) $(DFLAGS) $(XFLAGS) $(STATIC) -c daemon.c

list_utils.o :	list_utils.c
	$(CC) $(DFLAGS) $(XFLAGS) $(STATIC) -c list_utils.c

usage.o :	usage.c
	$(CC) $(DFLAGS) $(XFLAGS) $(STATIC) -c usage.c 


### SYSTYPES
solaris26:
	make -e $(ALL) $(MFLAGS) \
	XFLAGS='-DSOLARIS -DSOLARIS26 -DREGEX -DREGEXPR_3G' \
	CC="gcc $(CFLAGS)" XLIBS=-lgen

solaris25:
	make -e $(ALL) $(MFLAGS) \
	XFLAGS='-DSOLARIS -DSOLARIS25 -DREGEXPR_3G' \
	CC="gcc $(CFLAGS)" XLIBS=-lgen

bsd:
	make -e $(ALL) $(MFLAGS) \
	XFLAGS='-DBSD -DREGEX -DREGCOMP_3C' CC="$(CFLAGS)"

bsd-static:
	make -e $(ALL) $(MFLAGS) \
	XFLAGS='-DBSD -DREGEX -DREGCOMP_3C' CC="$(CFLAGS)"
	STATIC="-static"

BSD: bsd

FreeBSD: OpenBSD

OpenBSD: FreeBSD

generic:
	make -e $(ALL) $(MFLAGS) \
	XFLAGS='-DREGEX -DREGCOMP_3C' CC="$(CC) $(CFLAGS)" \

Linux: generic

linux: Linux

linux-static:
	make -e $(ALL) $(MFLAGS) \
	XFLAGS='-DREGEX -DREGCOMP_3C' CC="$(CC) $(CFLAGS) " \
	STATIC="-static"

lin: linux

linux-static-strip: linux-static strip

bsd-static-strip: bsd-static strip

strip: $(CKTEMP_OBJS) $(ALL)
	strip $(CKTEMP_OBJS) $(ALL)

#LIBS=-lgen

### RANDOM

clean:
	rm -f *.o core

clobber: clean
	rm -f $(ALL)
