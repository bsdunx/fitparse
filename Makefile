OPTIMIZATION ?= -O2
DEBUG = -g -ggdb
WARN = -Wall -pedantic
CFLAGS += $(WARN) $(DEBUG) -pthread -Ilib/mxml -Ilib/date

#TARGET = fitparse
#OBJECTS = activity.o fit.o tcx.o gpx.o
OBJECTS = $(patsubst %.c, %.o, $(wildcard *.c))
HEADERS = $(wildcard *.h)
LIB_HEADERS = lib/mxml/mxml.h lib/date/libdate.h
LIBS = lib/mxml/libmxml.a lib/date/libdate.a

default: test

all: test

test: $(OBJECTS) $(LIBS) $(HEADERS)
	$(CC) $(CFLAGS) $(OBJECTS) $(LIBS) -Wall -o $@
	mkdir -p tests/out

test.o: test.c $(HEADERS)
	$(CC) $(CFLAGS) -DDEBUG -DDEFAULT_DIR=tests/out -c $< -o $@

lib/mxml/Makefile:
	cd lib/mxml >/dev/null && ./configure >/dev/null

lib/mxml/libmxml.a: lib/mxml/Makefile
	$(MAKE) -C lib/mxml >/dev/null

lib/date/libdate.a:
	$(MAKE) -C lib/date >/dev/null

%.o: %.c $(HEADERS)
	$(CC) $(CFLAGS) -c $< -o $@

format:
	-@clang-format -style=Google -i *.c *.h

clean:
	rm -rf *.o util tests/out test gpx
	cd lib/mxml >/dev/null && git clean -f -d -x >/dev/null && git checkout -- mxml.xml >/dev/null
	cd lib/date >/dev/null && git clean -f -d -x >/dev/null

.SILENT: lib/mxml/Makefile mxml clean
.PHONY: default all mxml date clean format
