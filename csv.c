#include <string.h>
#include <assert.h>
#include <stdio.h>

#include "csv.h"

/* test empty option and remove = true... */

static DataField name_to_field(char *name) {
  /* TODO convert name to lower case.... */
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
    return Cadence;
  } else {/* not found */
    return DataFieldCount;
  }
}

/* Fields can have several names and be in different orders, but we assume
 * all are doubles in base SI unit which we then convert into our format */
int csv_read(char *filename, Activity *a) {
  FILE *f = NULL;
  char buf[CSV_BUFSIZ], *comma, *last, field_str[CSV_FIELD_SIZE];
  DataField data_fields[DataFieldCount], field;
  DataPoint point;
  unsigned i, count = 0;

  /* initialize */
  for (i = 0; i < DataFieldCount; i++) {
    data_fields[i] = DataFieldCount;
  }
  unset_data_point(&point);

  if (!(f = fopen(filename, "r"))) {
    return 1;
  }

  /* make sure we can read the header */
  if (!fgets(buf, sizeof(buf), f)) {
    return 1;
  }

  for (last = buf, comma = strchr(buf, ','); count < DataFieldCount && comma; comma = strchr(last, ',')) {
    strncpy(field_str, last, comma - last);
    field_str[comma - last] = '\0';

    if ((field = name_to_field(field_str))) {
      data_fields[count] = field;
      count++;
    }

    last = comma + 1;
  }
  if (count < DataFieldCount) {
    name_to_field(last);
  }

  /*
  // builder header array
  for (;;) {

    // read in array of doubles - IN ORDER

    activity_add_point(a, &point);
    unset_data_point(&point);
  }
  */

  fclose(f);
  return 1;
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
    write_field(f, "%.15f", i, Latitude, a, o, &first);
    write_field(f, "%.15f", i, Longitude, a, o, &first);
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
