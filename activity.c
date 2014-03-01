#include <stdlib.h>
#include <string.h>

#include "activity.h"

#define alloc_nr(x) (((x)+16)*3/2)

/*
 * Realloc the buffer pointed at by variable 'x' so that it can hold
 * at least 'nr' entries; the number of entries currently allocated
 * is 'alloc', using the standard growing factor alloc_nr() macro.
 *
 * DO NOT USE any expression with side-effect for 'x', 'nr', or 'alloc'.
 */
#define ALLOC_GROW(x, nr, alloc) \
	do { \
		if ((nr) > alloc) { \
			if (alloc_nr(alloc) < (nr)) \
				alloc = (nr); \
			else \
				alloc = alloc_nr(alloc); \
			x = realloc((x), alloc * sizeof(*(x))); \
		} \
	} while (0)

Activity *activity_new(void) {
  Activity *a;
  if (!(a = malloc(sizeof(*a)))) {
    return NULL;
  }
  a->sport = UnknownSport;
  a->laps = NULL; /* TODO */
  a->data_points = NULL;
  a->num_points = 0;
  memset(a->has_data, false, sizeof(a->has_data));
  memset(a->errors, 0, sizeof(a->errors));

  return a;
}

void activity_destroy(Activity *a) {
  /* delete all data points */
  if (a->data_points) {
    free(a->data_points);
    a->data_points = NULL;
    a->num_points = 0;
  }

  /* delete all laps */
  if (a->laps) {
    free(a->laps);
    a->laps = NULL;
  }

  free(a);
  a = NULL;
}

/* TODO make sure we infer missing values and do corrections */
int activity_add_point(Activity *a, DataPoint *dp) {
  unsigned i;

  ALLOC_GROW(a->data_points, a->num_points + 1, a->points_alloc);
  if (!(a->data_points)) {
    return 1;
  }

  /* TODO fill in inferred missing values a la gpx/tcx */
  for (i = 0; i < DataFieldCount; i++) {
    a->data_points[a->num_points].data[i] = dp->data[i];;
  }
  a->num_points++;

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
