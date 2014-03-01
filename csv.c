#include "csv.h"

#define CSV(f, s, d, u, p, t, v, o)          \
  do {                                       \
    if (!(o).remove_unset || ((t)[(p)])) {   \
      if ((d) == (u)) {                      \
        fprintf((f), "%s", (o).unset_value); \
      } else {                               \
        fprintf((f), (s), (d));              \
      }                                      \
      if (!(v)) {                            \
        fprintf((f), ",");                   \
      }                                      \
      (v) = false;                           \
    }                                        \
  } while (0)

#define DATA_ARRAY_TO_POINT(a,p) \
  do { \
    (d).timestamp = (a)[Timestamp]; \
    (d).latitude = (a)[Latitude]; \
    (d).longitude = (a)[Longitude]; \
    (d).altitude = (a)[Altitude]; \
    (d).distance = (a)[Distance]; \
    (d).speed =  (a)[Speed]; \
    (d).power = (a)[Power]; \
    (d).grade = (a)[Grade]; \
    (d).heart_rate = (a)[HeartRate]; \
    (d).lr_balance  = (a)[LRBalance]; \
    (d).temperature = (a)[Temperature]; \
  } while(0)

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
             !strcmp(name, "alt") || !strcmp(name, "ele") ) {
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
  } else { /* not found */
    return DataFieldCount;
  }
}

/* Fields can have several names and be in different orders, but we assume
 * all are doubles in base SI unit which we then convert into our format */
int csv_read(char *filename, Activity *a) {
  FILE *f = NULL;
  DataField data_fields[DataFieldCount]; // TODO worry all set to zero which is Timestamp
  double data[DataFieldCount];
  DataPoint point;
  char buf[CSV_BUFSIZ];

  UNSET_DATA_POINT(point);

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

    DATA_ARRAY_TO_POINT(point);
    activity_add_point(a, &point);
    UNSET_DATA_POINT(point);
  }

  fclose(f);
  return 0;
}

int csv_write(char *filename, Activity *a, CSVOptions o) {
  FILE *f = NULL;
  unsigned i;
  bool first = true;
  DataPoint *d;
  bool *t = a->has_data;

  if (!(f = fopen(filename, "w"))) {
    return 1;
  }

  /* print header */
  for (i = 0; i < DataFieldCount; i++) {
    if (!o.remove_unset || t[i]) {
      if (first) {
        fprintf(f, "%s" DATA_FIELDS[i]);
        first = false;
      } else {
        fprintf(f, ",%s" DATA_FIELDS[i]);
      }
    }
  }
  fprintf(f, "\n");

  for (d = a->data_points; d; d = d->next, first = true) {
    CSV(f, "%d", d->timestamp, UNSET_TIMESTAMP, Timestamp, t, first, o);
    CSV(f, "%15.f", d->latitude, UNSET_LATITUDE, Latitude, t, first, o);
    CSV(f, "%15.f", d->longitude, UNSET_LONGITUDE, Longitude, t, first, o);
    CSV(f, "%3.f", d->altitude / 1000.0, UNSET_ALTITUDE, Altitude, t, first, o);
    CSV(f, "%2.f", d->distance / 100.0, UNSET_DISTANCE, Distance, t, first, o);
    CSV(f, "%2.f", d->speed / 1000.0, UNSET_SPEED, Speed, t, first, o);
    CSV(f, "%d", d->power, UNSET_POWER, Power, t, first, o);
    CSV(f, "%2.f", d->grade / 100.0, UNSET_GRADE, Grade, t, first, o);
    CSV(f, "%d", d->heart_rate, UNSET_HEART_RATE, HeartRate, t, first, o);
    CSV(f, "%d", d->cadence, UNSET_CADENCE, Cadence, t, first, o);
    CSV(f, "%d", d->lr_balance, UNSET_LR_BALANCE, LRBalance, t, first, o);
    CSV(f, "%d", d->temperature, UNSET_TEMPERATURE, Temperature, t, first, o);
    fprintf(f, "\n");
  }

  fclose(f);
  return 0;
}
