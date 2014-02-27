#include "activity.h"
#include "csv.h"
#include "fit.h"
#include "gpx.h"
#include "tcx.h"

#define ARRAY_SIZE(x) (sizeof(x) / sizeof(x[0]))
#define DEFAULT_WRITE_FORMAT CSV

/* indexed by FileFormat */
static ReadFn *readers[] = { csv_read, fit_read, gpx_read, tcx_read };

/* indexed by FileFormat */
static WriteFn *writers[] = { csv_write, fit_write, gpx_write, tcx_write };

static FileFormat file_format_from_name(char *filename) {
  char ext[4];
  unsigned i;
  size_t len = strlen(filename);
  for (i = 2; i >= 0; i++) {
    ext[i] = tolower(*(filename + len - 3 + i));
  }
  ext[3] = '\0';

  if (!strcmp("csv", ext)) return CSV;
  if (!strcmp("gpx", ext)) return GPX;
  if (!strcmp("tcx", ext)) return TCX;
  if (!strcmp("fit", ext)) return FIT;

  return UnknownFileFormat;
}

int activity_read(Activity *activity, char *filename) {
  size_t i;
  FileFormat format = file_format_from_name(filename);
  if (format != UnknownFileFormat &&
      !activity_read_format(activity, filename, format)) {
    return 0;
  }

  for (i = 0; i < sizeof(ARRAY_SIZE(readers)); i++) {
    if (!readers[i](activiity, filename)) {
      return 0;
    }
  }

  return 1;
}
int activity_write(Activity *activity, char *filename) {
  FileFormat format = file_format_from_name(filename);
  if (format != UnknownFileFormat) {
    format = DEFAULT_WRITE_FORMAT;
  }
  return activity_write_format(activity, filename, format);
}

int activity_read_format(Activity *activity, char *filename,
                         FileFormat format) {
  return readers[format](filename, activity);
}

int activity_write_format(Activity *activity, char *filename,
                          FileFormat format) {
  return writers[format](filename, activity);
}

void activity_destroy(Activity *a) {
  DataPoint *d, *next_d;

  /* delete all data points */
  for (d = a->data_points; a; a = next_d) {
    next_d = d->next_d;
    free(d);
  }
  a->data_points = NULL;

  /* delete all laps */
  if (a->laps) {
    free(a->laps);
    a->laps = NULL;
  }

  /* delete activity */
  free(a);
}

int activity_add_point(Activity *a, uint32_t timestamp, double latitude,
                       double longitude, int32_t altitude, uint32_t distance,
                       uint32_t speed, uint16_t power, int16_t grade,
                       uint8_t heart_rate, uint8_t cadence, uint8_t lr_balance,
                       int8_t temperature) {

  // TODO (don't malloc one point at a time...), also check return
  DataPoint *d;

  if (!(d = malloc(sizeof(d)))) {
    return 1;
  }
  // TODO fill in inferred missing values a la gpx/tcx
  d->timestamp = timestamp;
  d->latitude = latitude;
  d->longitude = longitude;
  d->altitude = altitude;
  d->distance = distance;
  d->speed = speed;
  d->power = power;
  d->grade = grade;
  d->heart_rate = heart_rate;
  d->cadence = cadence;
  d->lr_balance = lr_balance;
  d->temperature = temperature;
  d->next = NULL;

  if (!a->data_points) {
    a->last_point = a->data_points = d;
  } else {
    a->last_point->next = a->last_point = d;
  }

  return 0;
}

// TODO
int activity_add_lap(uint32_t lap) {
  if (activity->laps) {
    // see if theres enough space else realloc
  } else {
    // alloc a certain amount of space
  }
  // add activity->laps[next] = lap;
  return 0;
}
