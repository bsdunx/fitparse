OPTIMIZATION ?= -O2
DEBUG = -g -ggdb
WARN = -Wall -pedantic
CFLAGS += $(WARN) $(DEBUG)

#TARGET = fitparse
#OBJECTS = activity.o fit.o tcx.o gpx.o

#all: $(TARGET)
all: mxml

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
.PHONY: all mxml clean format
