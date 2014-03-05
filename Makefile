OPTIMIZATION ?= -O2
DEBUG = -g -ggdb
WARN = -Wall -Wextra -pedantic
CFLAGS += $(WARN) $(DEBUG) -pthread -Ilib/mxml -Ilib/date

#TARGET = fitparse
#OBJECTS = activity.o fit.o tcx.o gpx.o
OBJECTS = $(patsubst %.c, %.o, $(wildcard *.c))
LIB_HEADERS = lib/mxml/mxml.h lib/date/date.h
HEADERS = $(wildcard *.h) $(LIB_HEADERS)
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

# TODO could make this more exact to minimize recompiles
%.o: %.c $(HEADERS)
	$(CC) $(CFLAGS) -c $< -o $@

format:
	-@clang-format -style=Google -i *.c *.h

clang:
	-@rm -rf clang/*
	-@scan-build -V -k -o `pwd`/clang $(MAKE) clean all

clean:
	rm -rf *.o util tests/out test gpx clang/*
	cd lib/mxml >/dev/null && git clean -f -d -x >/dev/null && git checkout -- mxml.xml >/dev/null
	cd lib/date >/dev/null && git clean -f -d -x >/dev/null

.SILENT: lib/mxml/Makefile clean
.PHONY: default all clean format clang
