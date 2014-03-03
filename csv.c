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

#include <string.h>
#include <assert.h>
#include <stdio.h>

#include "csv.h"
#include "util.h"

static const char *DATA_FIELDS[DataFieldCount] = {
    "timestamp",  "latitude", "longitude",  "altitude",
    "distance",   "speed",    "power",      "grade",
    "heart_rate", "cadence",  "lr_balance", "temperature"};

static DataField name_to_field(char *name) {
  /* deal with optional leading whitespace */
  while(isspace(*name)) name++ ;
  /* make the name lowercase for comparisons */
  downcase(name);

  if (!strcmp(name, "timestamp") || !strcmp(name, "time")) {
    return Timestamp;
  } else if (!strcmp(name, "latitude") || !strcmp(name, "lat")) {
    return Latitude;
  } else if (!strcmp(name, "longitude") || !strcmp(name, "lon") ||
             !strcmp(name, "long")) {
    return Longitude;
  } else if (!strcmp(name, "altitude") || !strcmp(name, "elevation") ||
             !strcmp(name, "alt") || !strcmp(name, "ele")) {
    return Altitude;
  } else if (!strcmp(name, "distance") || !strcmp(name, "dist")) {
    return Distance;
  } else if (!strcmp(name, "speed") || !strcmp(name, "spd")) {
    return Speed;
  } else if (!strcmp(name, "power") || !strcmp(name, "pow") ||
             !strcmp(name, "watts")) {
    return Power;
  } else if (!strcmp(name, "slope") || !strcmp(name, "grade") ||
             !strcmp(name, "gradient")) {
    return Grade;
  } else if (!strcmp(name, "heart_rate") || !strcmp(name, "hr")) {
    return HeartRate;
  } else if (!strcmp(name, "cadence") || !strcmp(name, "cad")) {
    return Cadence;
  } else if (!strcmp(name, "balance") || !strcmp(name, "bal") ||
             !strcmp(name, "lr_balance")) {
    return LRBalance;
  } else if (!strcmp(name, "temperature") || !strcmp(name, "atemp") ||
             !strcmp(name, "temp")) {
    return Temperature;
  } else {/* not found */
    return DataFieldCount;
  }
}

/* Fields can have several names and be in different orders, but we assume
 * all are doubles in base SI unit which we then convert into our format */
Activity *csv_read(char *filename) {
  FILE *f = NULL;
  char buf[CSV_BUFSIZ], *comma, *last, field_str[CSV_FIELD_SIZE];
  DataField data_fields[CSV_MAX_FIELDS], field;
  Activity *a;
  DataPoint dp;
  unsigned i, j, count = 0;

  /* initialize */
  unset_data_point(&dp);

  if (!(f = fopen(filename, "r"))) {
    return NULL;
  }

  /* make sure we can read the header */
  if (!fgets(buf, sizeof(buf), f)) {
    return NULL;
  }

  /* read in the header */
  for (i = 0, last = buf, comma = strchr(buf, ',');
       count < DataFieldCount && i < CSV_MAX_FIELDS && comma;
       comma = strchr(last, ','), i++) {
    strncpy(field_str, last, comma - last);
    field_str[comma - last] = '\0';

    if ((field = name_to_field(field_str)) != DataFieldCount) {
      count++;
    }
    data_fields[i] = field;

    last = comma + 1;
    memset(field_str, '\0', CSV_FIELD_SIZE);
  }
  /* strip newline */
  if ((comma = strrchr(last, '\n')) != NULL) *comma = '\0';
  /* grab the last header field (no trailing comma) */
  if (count < DataFieldCount && i < CSV_MAX_FIELDS) {
    if ((field = name_to_field(last)) != DataFieldCount) {
      count++;
    }
    data_fields[i] = field;
  }
  memset(buf, '\0', CSV_BUFSIZ);

  /* TODO */
  fprintf(stderr, "count: %d\n", count);
  for (i = 0, j = 0; i < CSV_MAX_FIELDS && j < count; i++) {
    fprintf(stderr, "%d: %d\n", i, data_fields[i]);
    if (data_fields[i] != DataFieldCount) {
      j++;
    }
  }
  /* TODO */

  /* if we couldn't read any fields from the header, this is not a csv */
  if (!count) {
    return NULL;
  }

  a = activity_new();

  /* read in data points */
  while (!fgets(buf, sizeof(buf), f)) {

    for (i = 0, j = 0, last = buf, comma = strchr(buf, ',');
        j < i && i < CSV_MAX_FIELDS && comma;
        comma = strchr(last, ','), i++) {

      strncpy(field_str, last, comma - last);
      field_str[comma - last] = '\0';

      if ((field = name_to_field(field_str)) != DataFieldCount) {
        j++;
      }
      data_fields[i] = field;

      last = comma + 1;
      memset(field_str, '\0', CSV_FIELD_SIZE);
    }
    /* strip newline */
    if ((comma = strrchr(last, '\n')) != NULL) *comma = '\0';
    /* grab the last header field (no trailing comma) */
    if (count < DataFieldCount && i < CSV_MAX_FIELDS) {
      if ((field = name_to_field(last)) != DataFieldCount) {
        count++;
      }
      data_fields[i] = field;
    }


    activity_add_point(a, &dp);

    unset_data_point(&dp);
    memset(buf, '\0', CSV_BUFSIZ);
  }

  a->format = CSV;
  activity_destroy(a); /* TODO */

  fclose(f);
  return NULL;
}

static void write_field(FILE *f, const char *format, size_t i, DataField field,
                        Activity *a, CSVOptions o, bool *first) {
  double d = a->data_points[i].data[field];
  if (!o.remove_unset || a->has_data[field]) {
    if (!*first) {
      fprintf(f, ",");
    }
    if (d == UNSET_FIELD) {
      fprintf(f, "%s", o.unset_value);
    } else {
      fprintf(f, format, d);
    }
    *first = false;
  }
}

int csv_write_options(char *filename, Activity *a, CSVOptions o) {
  FILE *f = NULL;
  unsigned i;
  bool first = true;

  assert(a != NULL);

  if (!(f = fopen(filename, "w"))) {
    return 1;
  }

  /* print header */
  for (i = 0; i < DataFieldCount; i++) {
    if (!o.remove_unset || a->has_data[i]) {
      if (first) {
        fprintf(f, "%s", DATA_FIELDS[i]);
        first = false;
      } else {
        fprintf(f, ",%s", DATA_FIELDS[i]);
      }
    }
  }
  fprintf(f, "\n");

  /* print data points - must be at least one non empty */
  for (i = 0, first = true; i < a->num_points; i++, first = true) {
    write_field(f, "%.0f", i, Timestamp, a, o, &first);
    write_field(f, "%.7f", i, Latitude, a, o, &first);
    write_field(f, "%.7f", i, Longitude, a, o, &first);
    write_field(f, "%.3f", i, Altitude, a, o, &first);
    write_field(f, "%.2f", i, Distance, a, o, &first);
    write_field(f, "%.2f", i, Speed, a, o, &first);
    write_field(f, "%.0f", i, Power, a, o, &first);
    write_field(f, "%.2f", i, Grade, a, o, &first);
    write_field(f, "%.0f", i, HeartRate, a, o, &first);
    write_field(f, "%.0f", i, Cadence, a, o, &first);
    write_field(f, "%.0f", i, LRBalance, a, o, &first);
    write_field(f, "%.0f", i, Temperature, a, o, &first);
    fprintf(f, "\n");
  }

  fclose(f);
  return 0;
}
