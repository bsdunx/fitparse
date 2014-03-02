#include <string.h>
#include <ctype.h>

#include "fitparse.h"
#include "activity.h"
#include "csv.h"
#include "fit.h"
#include "gpx.h"
#include "tcx.h"

#define DEFAULT_WRITE_FORMAT CSV

/* Athlete:
 *
 * name
 * gender (TRIMp)
 * preferences (metric, imperial)
 * power zones
 * hr zones (hrr, hrmax)
 * critical power
 * FTP
 * weight (kg/lbs)
 */

/* indexed by FileFormat */
static const ReadFn readers[] = {csv_read, gpx_read, tcx_read, fit_read };

/* indexed by FileFormat */
static const WriteFn writers[] = {csv_write, gpx_write, tcx_write, fit_write };

static FileFormat file_format_from_name(char *filename) {
  char ext[4];
  int i;
  size_t len = strlen(filename);
  for (i = 2; i >= 0; i--) {
    ext[i] = tolower(*(filename + len - 3 + i));
  }
  ext[3] = '\0';

  if (!strcmp("csv", ext)) return CSV;
  if (!strcmp("gpx", ext)) return GPX;
  if (!strcmp("tcx", ext)) return TCX;
  if (!strcmp("fit", ext)) return FIT;

  return UnknownFileFormat;
}

Activity *fitparse_read(char *filename) {
  Activity *a;
  size_t i;
  FileFormat format = file_format_from_name(filename);

  if (format != UnknownFileFormat) {
    return (a = fitparse_read_format(filename, format)) ? a : NULL;
  }

  for (i = 0; i < ARRAY_SIZE(readers); i++) {
    if ((a = readers[i](filename))) {
      return a;
    }
  }

  return NULL;
}

int fitparse_write(char *filename, Activity *activity) {
  FileFormat format = file_format_from_name(filename);
  if (format != UnknownFileFormat) {
    format = DEFAULT_WRITE_FORMAT;
  }
  return fitparse_write_format(filename, format, activity);
}

Activity *fitparse_read_format(char *filename, FileFormat format) {
  return readers[format](filename);
}

int fitparse_write_format(char *filename, FileFormat format, Activity *activity) {
  return writers[format](filename, activity);
}
