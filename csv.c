#include "csv.h"

static DataField name_to_field(char *name) {
  // TODO convert name to lower case....
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
  DataField data_fields[DataFieldCount];  // TODO worry all set to zero which is
                                          // Timestamp
  DataPoint point;
  char buf[CSV_BUFSIZ];

  unset_data_point(&point);

  if (!(f = fopen(filename, "r"))) {
    return 1;
  }

  /* make sure we can read the header */
  if (!fget(buf, sizeof(buf), f)) {
    return 1;
  }

  // builder header array

  for (;;) {

    // read in array of doubles - IN ORDER

    activity_add_point(a, &point);
    unset_data_point(&point);
  }

  fclose(f);
  return 0;
}

static int write_csv(FILE *f, const char *format, size_t i, DataField f,
                     Activity *a, CSVOptions o, bool *first) {
  double d = a->data_points[i].data[field];
  if (!o.remove_unset || a->has_data[field]) {
    if (d == UNSET_FIELD) {
      fprintf(f, "%s", o.unset_value);
    } else {
      fprintf(f, format, d);
    }
    if (!*first) {
      fprintf(f, ",");
    }
    *first = false;
  }
}

int csv_write(char *filename, Activity *a, CSVOptions o) {
  FILE *f = NULL;
  unsigned i;
  bool first = true;
  DataPoint *d;

  if (!(f = fopen(filename, "w"))) {
    return 1;
  }

  /* print header */
  for (i = 0; i < DataFieldCount; i++) {
    if (!o.remove_unset || a->has_data[i]) {
      if (first) {
        fprintf(f, "%s" DATA_FIELDS[i]);
        first = false;
      } else {
        fprintf(f, ",%s" DATA_FIELDS[i]);
      }
    }
  }
  fprintf(f, "\n");

  for (i = 0; i < a->num_points; i++, first = true) {
    CSV(f, "%.0f", i, Timestamp, a, &o, &first);
    CSV(f, "%.15f", i, Latitude, a, &o, &first);
    CSV(f, "%.15f", i, Longitude, a, &o, &first);
    CSV(f, "%.3f", i, Altitude, a, &o, &first);
    CSV(f, "%.2f", i, Distance, a, &o, &first);
    CSV(f, "%.2f", i, Speed, a, &o, &first);
    CSV(f, "%.0f", i, Power, a, &o, &first);
    CSV(f, "%.2f", i, Grade, a, &o, &first);
    CSV(f, "%.0f", i, HeartRate, a, &o, &first);
    CSV(f, "%.0f", i, Cadence, a, &o, &first);
    CSV(f, "%.0f", i, LRBalance, a, &o, &first);
    CSV(f, "%.0f", i, Temperature, a, &o, &first);
    fprintf(f, "\n");
  }

  fclose(f);
  return 0;
}
