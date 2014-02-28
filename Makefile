OPTIMIZATION ?= -O2
DEBUG = -g -ggdb
WARN = -Wall -pedantic
CFLAGS += $(WARN) $(DEBUG) -pthread

#TARGET = fitparse
#OBJECTS = activity.o fit.o tcx.o gpx.o
HEADERS = $(wildcard *.h)

default: gpx

all: gpx

gpx: gpx.o activity.o util.o lib/mxml/libmxml.a lib/date/libdate.a
	$(CC) $^ $(CFLAGS) -o $@

gpx.o: gpx.c gpx.h lib/mxml/mxml.h $(HEADERS)
	$(CC) $(CFLAGS) -Ilib/mxml -c $< -o $@

util.o: util.c lib/date/date.h $(HEADERS)
	$(CC) $(CFLAGS) -Ilib/date -c $< -o $@

activity.o: activity.c $(HEADERS)
	$(CC) $(CFLAGS) -c $< -o $@

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
	cd lib/mxml >/dev/null && git clean -f -d -x >/dev/null && git checkout -- mxml.xml >/dev/null
	cd lib/date >/dev/null && git clean -f -d -x >/dev/null

.SILENT: lib/mxml/Makefile mxml clean
.PHONY: default all mxml date clean format
