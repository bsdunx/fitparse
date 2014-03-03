/*
 * Copyright (c) 2014 Kirk Scheibelhut <kjs@scheibo.com>
 *
 *  This file is free software: you may copy, redistribute and/or modify it
 *  under the terms of the GNU General Public License as published by the
 *  Free Software Foundation, either version 2 of the License, or (at your
 *  option) any later version.
 *
 *  This file is distributed in the hope that it will be useful, but
 *  WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <stdio.h>
#include <stdarg.h>
#include <string.h>

#include "fitparse.h"
#include "util.h"

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
  if (!(a = fitparse_read((char *)filename))) {
    print("failed to read in '%s'\n", filename);
    return false;
  }
  print("successfully read '%s' in as format %d\n", filename, a->format);

  /* create a new filename by prefixing the old filename and the test dir */
  strcpy(namebuf, dir);
  if (dir[len - 1] != '/') {
    namebuf[len] = '/';
    len++;
  }

  strncpy(namebuf + len, PREFIX, sizeof(PREFIX) - 1);
  strcpy(namebuf + len + (sizeof(PREFIX) - 1), strrchr(filename, '/') + 1);

  change_extension(namebuf, "csv");
  print("writing to file %s\n", namebuf);
  /*fitparse_write_format(namebuf, GPX, a);*/
  fitparse_write(namebuf, a);
  activity_destroy(a);
  return 0;
#if 0
  fitparse_write(namebuf, a);

  /* read in the file in its original format */
  if (!(b = fitparse_read(namebuf))) {
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
