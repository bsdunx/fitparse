OPTIMIZATION ?= -O2
DEBUG = -g -ggdb
WARN = -Wall -pedantic
CFLAGS += $(WARN) $(DEBUG) -pthread

#TARGET = fitparse
#OBJECTS = activity.o fit.o tcx.o gpx.o

default: util

all: mxml date

gpx: gpx.o mxml
	$(CC) $< $(CFLAGS) -static -Llib/mxml -lmxml -o $@

gpx.o: gpx.c gpx.h lib/mxml/mxml.h
	$(CC) $(CFLAGS) -Ilib/mxml -c $< -o $@

util: util.o date
	$(CC) $< $(CFLAGS) -Llib/date -ldate -o $@

util.o: util.c lib/date/date.h
	$(CC) $(CFLAGS) -Ilib/date -c $< -o $@

mxml: lib/mxml/libmxml.a
date: lib/date/libdate.a

lib/mxml/Makefile:
	cd lib/mxml >/dev/null && ./configure >/dev/null

lib/mxml/libmxml.a: lib/mxml/Makefile
	$(MAKE) -C lib/mxml >/dev/null

lib/date/libdate.a:
	$(MAKE) -C lib/date >/dev/null

format:
	-@clang-format -style=Google -i *.c *.h

clean:
	rm -rf *.o util
	cd lib/mxml >/dev/null && git clean -f -d -x >/dev/null
	cd lib/date >/dev/null && git clean -f -d -x >/dev/null

.SILENT: lib/mxml/Makefile mxml clean
.PHONY: default all mxml date clean format
