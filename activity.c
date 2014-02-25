#include "activity.h"

void activity_destroy(Activity *a) {
  DataPoint *d, *next;
  for (d = a->data_points; a; a = next) {
    next = d->next;
    free(d);
  }
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
