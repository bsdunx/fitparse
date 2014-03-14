OPTIMIZATION ?= -O2
DEBUG = -g -ggdb
WARN = -Wall -Wextra -pedantic -Wno-missing-field-initializers -Wno-unused-parameter
CFLAGS += $(WARN) $(DEBUG) -pthread -Ilib/mxml -Ilib/date

TARGET = libfitparse.a
MAINS = test.o client.o
OBJECTS = $(filter-out $(MAINS), $(patsubst %.c, %.o, $(wildcard *.c)))
LIB_HEADERS = lib/mxml/mxml.h lib/date/date.h
HEADERS = $(wildcard *.h) $(LIB_HEADERS)
LIBS = lib/mxml/libmxml.a lib/date/libdate.a

default: $(TARGET)

all: $(TARGET) fitparse test

fitparse: client.o $(TARGET)
	$(CC) $(CFLAGS) $^ -o $@ -lm

$(TARGET): $(OBJECTS) $(LIBS)
	-@rm -rf build
	@mkdir build && cp $(LIBS) build
	@cd build && ar x libmxml.a && ar x libdate.a
	ar rcs $@ build/*.o $(OBJECTS)
	-@rm -rf build

test: test.o $(OBJECTS) $(LIBS) $(HEADERS)
	$(CC) $(CFLAGS) $^ -o $@ -lm
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
	rm -rf *.a *.o util tests/out test gpx clang/* build
	cd lib/mxml >/dev/null && git clean -f -d -x >/dev/null && git checkout -- mxml.xml >/dev/null
	cd lib/date >/dev/null && git clean -f -d -x >/dev/null

.SILENT: lib/mxml/Makefile clean
.PHONY: default all clean format clang
