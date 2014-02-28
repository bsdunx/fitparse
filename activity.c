#include <stdlib.h>
#include <string.h>

#include "activity.h"

Activity *activity_new(void) {
  Activity *a;
  if (!(a = malloc(sizeof(*a)))) {
    return NULL;
  }
  a->sport = UnknownSport;
  a->laps = NULL; /* TODO */
  a->data_points = a->last_point = NULL;
  a->has_data = calloc(1, sizeof(*(a->has_data)));

  return a;
}

void activity_destroy(Activity *a) {
  DataPoint *d, *next;

  /* delete all data points */
  for (d = a->data_points; d; d = next) {
    next = d->next;
    free(d);
  }
  a->data_points = NULL;

  /* delete all laps */
  if (a->laps) {
    free(a->laps);
    a->laps = NULL;
  }

  free(a->has_data);
  a->has_data = NULL;

  free(a);
  a = NULL;
}

/* TODO make sure we infer missing values and do corrections */
int activity_add_point(Activity *a, DataPoint *point) {

  /* TODO (don't malloc one point at a time...), also check return */
  DataPoint *d;
  if (!(d = malloc(sizeof(*d)))) {
    return 1;
  }
  /* TODO fill in inferred missing values a la gpx/tcx */
  d->timestamp = point->timestamp;
  d->latitude = point->latitude;
  d->longitude = point->longitude;
  d->altitude = point->altitude;
  d->distance = point->distance;
  d->speed = point->speed;
  d->power = point->power;
  d->grade = point->grade;
  d->heart_rate = point->heart_rate;
  d->cadence = point->cadence;
  d->lr_balance = point->lr_balance;
  d->temperature = point->temperature;
  d->next = NULL;

  if (!a->data_points) {
    a->last_point = a->data_points = d;
  } else {
    a->last_point->next = a->last_point = d;
  }

  return 0;
}

/* TODO */
int activity_add_lap(Activity *a, uint32_t lap) {
  if (a->laps) {
    /* see if theres enough space else realloc */
  } else {
    /* alloc a certain amount of space */
  }
  /* add activity->laps[next] = lap; */
  return 0;
}
