OPTIMIZATION ?= -O2
DEBUG = -g -ggdb
WARN = -Wall -pedantic
CFLAGS += $(WARN) $(DEBUG) -Ilib

#TARGET = fitparse
#OBJECTS = activity.o fit.o tcx.o gpx.o

default: util

#all: $(TARGET)
all: mxml

util: util.o lib/date.o
	$(CC) $(CFLAGS) $^ -o $@

util.o: util.c lib/date.h
	$(CC) $(CFLAGS) -c $< -o $@

lib/date.o: lib/date.c lib/date.h
	$(CC) $(CFLAGS) -c $< -o $@

#$(TARGET): $(OBJECTS) mxml
		#$(CC) $(OBJECTS) $(FLAGS) $(CFLAGS) -Llib/mxml -lmxml -o $(TARGET)

lib/mxml/Makefile:
	cd lib/mxml >/dev/null && ./configure >/dev/null

mxml: lib/mxml/Makefile
	$(MAKE) -C lib/mxml >/dev/null

format:
	-@clang-format -style=Google -i *.c *.h

clean:
	rm -rf *.o
	cd lib/mxml >/dev/null && git clean -f -d -x >/dev/null

.SILENT: lib/mxml/Makefile mxml clean
.PHONY: default all mxml clean format
