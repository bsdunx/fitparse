#include <stdio.h>
#include <stdarg.h>
#include <string.h>

#include "fitparse.h"

#define PREFIX "out."
#define DEFAULT_DIR "tests/out"

static void print(const char *format, ...) {
#ifdef DEBUG
  va_list ap;

  va_start(ap, format);
  vfprintf(stderr, format, ap);
  va_end(ap);
#endif
  return;
}

bool test(const char *filename, const char *dir) {
  char namebuf[BUFSIZ], err;
  Activity *a, *b;
  size_t len = strlen(dir);

  /* read in the file in its original format */
  if (fitparse_read((char *)filename, a)) {
    print("failed to read in '%s'\n", filename);
    return false;
  }
  print("successfully read '%s' in as format %d\n", filename, a->format);

  /* create a new filename by prefixing the old filename and the test dir */
  strcpy(namebuf, dir);
  if (dir[len-1] != '/') {
    namebuf[len] = '/';
    len++;
  }

  strncpy(namebuf+len, PREFIX, sizeof(PREFIX)-1);
  strcpy(namebuf+len+(sizeof(PREFIX)-1), strrchr(filename, '/')+1);

  print("writing to file %s\n", namebuf);
  fitparse_write_format(namebuf, CSV, a);
  return 0;
#if 0
  fitparse_write(namebuf, a);

  /* read in the file in its original format */
  if (fitparse_read(namebuf, b)) {
    print("failed to read in '%s'\n", namebuf);
    activity_destroy(a);
    return false;
  }

  print("successfully read '%s' in as format %d\n", namebuf, b->format);

  err = !activity_equal(a, b);

  print("activity_equal returned %d\n", !err);

  activity_destroy(b);
  activity_destroy(a);
  return err;
#endif
}

int main(int argc, char *argv[]) {
  if (argc == 3) {
    return test(argv[1], argv[2]);
  } else {
    return test(argv[1], DEFAULT_DIR);
  }
}
