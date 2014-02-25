#include "activity.h"

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

int activity_add_lap(uint32_t lap) {
  if (activity->laps) {
      // see if theres enough space else realloc
  } else {
    // alloc a certain amount of space
  }
  // add activity->laps[next] = lap;
  return 0;
}
