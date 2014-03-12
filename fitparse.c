/*
 *  Copyright (c) 2014 Kirk Scheibelhut <kjs@scheibo.com>
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

#include <string.h>
#include <ctype.h>

#include "fitparse.h"
#include "activity.h"
#include "util.h"
#include "csv.h"
#include "fit.h"
#include "gpx.h"
#include "tcx.h"

#define DEFAULT_WRITE_FORMAT CSV

/* indexed by FileFormat */
static const ReadFn readers[] = {csv_read, gpx_read, tcx_read, fit_read};

/* indexed by FileFormat */
static const WriteFn writers[] = {csv_write, gpx_write, tcx_write, fit_write};

static FileFormat file_format_from_name(char *filename) {
  char ext[4];
  strcpy(ext, extension(filename));
  downcase(ext);
  return file_format(ext);
}

Activity *fitparse_read(char *filename) {
  FILE *f;
  Activity *a;
  FileFormat format = file_format_from_name(filename);

  if (format != UnknownFileFormat) {
    return (a = fitparse_read_format(filename, format)) ? a : NULL;
  }

  if (!(f = fopen(filename, "r"))) return NULL;

  a = fitparse_read_file(f);
  fclose(f);
  return a;
}

Activity *fitparse_read_file(FILE *f) {
  Activity *a;
  size_t i;
  for (i = 0; i < ARRAY_SIZE(readers); i++) {
    if ((a = readers[i](f))) {
      return a;
    }
  }

  return NULL;
}

Activity *fitparse_read_format(char *filename, FileFormat format) {
  FILE *f;
  Activity *a;
  if (!(f = fopen(filename, "r"))) return NULL;
  a = fitparse_read_format_file(f, format);
  fclose(f);
  return a;
}

Activity *fitparse_read_format_file(FILE *f, FileFormat format) {
  return readers[format](f);
}

int fitparse_write(char *filename, Activity *a) {
  FileFormat format = file_format_from_name(filename);
  return fitparse_write_format(filename, format, a);
}

int fitparse_write_format(char *filename, FileFormat format, Activity *a) {
  FILE *f;
  if (!(f = fopen(filename, "w"))) return 1;
  fitparse_write_format_file(f, format, a);
  fclose(f);
  return 0;
}

int fitparse_write_format_file(FILE *f, FileFormat format, Activity *a) {
  if (format == UnknownFileFormat) format = DEFAULT_WRITE_FORMAT;
  return writers[format](f, a);
}
