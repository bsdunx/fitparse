/*
 *  Copyright (c) 2014 Kirk Scheibelhut <kjs@scheibo.com>
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
 *
 * This file incorporates work covered by the following copyright and
 * permission notice:
 *
 *     Copyright (c) 2010-2011 Mark Liversedge (liversedge@gmail.com)
 *     Copyright (c) 2009 Justin F. Knotzke (jknotzke@shampoo.ca)
 *     Copyright (c) 2006-2008 Sean C. Rhea (srhea@srhea.net)
 *
 *     This program is free software; you can redistribute it and/or modify it
 *     under the terms of the GNU General Public License as published by the
 *     Free Software Foundation; either version 2 of the License, or (at your
 *     option) any later version.
 *
 *     This program is distributed in the hope that it will be useful, but
 *     WITHOUT ANY WARRANTY; without even the implied warranty of
 *     MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *     General Public License for more details.
 *
 *     You should have received a copy of the GNU General Public License along
 *     with this program; if not, write to the Free Software Foundation, Inc.,
 *     51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */

#include <stdlib.h>
#include <string.h>

#include "activity.h"

#define alloc_nr(x) (((x) + 16) * 3 / 2)

/*
 * Realloc the buffer pointed at by variable 'x' so that it can hold
 * at least 'nr' entries; the number of entries currently allocated
 * is 'alloc', using the standard growing factor alloc_nr() macro.
 *
 * DO NOT USE any expression with side-effect for 'x', 'nr', or 'alloc'.
 */
#define ALLOC_GROW(x, nr, alloc)              \
  do {                                        \
    if ((nr) > alloc) {                       \
      if (alloc_nr(alloc) < (nr))             \
        alloc = (nr);                         \
      else                                    \
        alloc = alloc_nr(alloc);              \
      x = realloc((x), alloc * sizeof(*(x))); \
    }                                         \
  } while (0)

Activity *activity_new(void) {
  unsigned i;
  Activity *a;

  if (!(a = malloc(sizeof(*a)))) {
    return NULL;
  }

  a->sport = UnknownSport;
  a->format = UnknownFileFormat;
  a->start_time = 0;
  a->laps = NULL; /* TODO */
  a->data_points = NULL;
  a->num_points = 0;

  memset(a->errors, 0, sizeof(*(a->errors)));
  memset(a->last, NULL, sizeof(*(a->last)));

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

/* Attempt to derive speed from distance or vice-versa */
static void derive_speed_distance(DataPoint *last, DataPoint *dp) {
  double delta_t, delta_d;

  /* return if distance and speed data is fine, or if last values are bad */
  if ((dp->data[Speed] != UNSET_FIELD &&
       dp->data[Distance] != UNSET_FIELD) ||
      (last->data[Timestamp] == UNSET_FIELD &&
       last->data[Distance] == UNSET_FIELD))
    return;

  /* compute the elapsed time and distance traveled since the last recorded trackpoint */
  delta_t = dp->data[Timestamp] - last->data[Time];

  /* derive speed from distance */
  if (dp->data[Speed] == UNSET_FIELD && dp->data[Distance] != UNSET_FIELD) {

    delta_d = dp->data[Distance] - last->data[Distance];
    if (delta_t > 0) dp->data[Speed] = delta_d / delta_t * SECS_IN_HOUR;
  } else if (dp->data[Distance] != UNSET_FIELD) { /* otherwise derive distance from speed */
    delta_d = delta_t * dp->data[Speed] / SECS_IN_HOUR;
    dp->distance[Distance] = last->data[Distance] + delta_d;
  }
}

/* TODO make sure we infer missing values and do corrections */
/* TODO see gpx parser for additional checks we must perform */
int activity_add_point(Activity *a, DataPoint *dp) {
  unsigned i;

  if (!a->start_time && dp->data[Timestamp] != UNSET_FIELD) {
    a->start_time = dp->data[Timestamp];
  }

  ALLOC_GROW(a->data_points, a->num_points + 1, a->points_alloc);
  if (!(a->data_points)) {
    return 1;
  }

  /* if this isn't the first time */
  if (a->num_points > 0) {
    derive_speed_distance(&(a->data_points[a->num_points - 1]), DataPoint *dp);
  }

  for (i = 0; i < DataFieldCount; i++) {
    a->data_points[a->num_points].data[i] = dp->data[i];
    if (dp->data[i] != UNSET_FIELD) {
      a->last[i] = a->data_points[a->num_points];
    }
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

/* TODO */
/* TODO double equals ? */
bool activity_equal(Activity *a, Activity *b) {
  unsigned i, j;

  if (!a) return !b;
  if (a == b) return true;

  /* TODO a->sport ? */
  if ((a->num_points != b->num_points) ||
      (a->start_time != b->start_time)) return false;

  for (i = 0; i < DataFieldCount; i++) {
    if (a->last[i] && !b->last[i] || !a->last[i] && b->last[i]) return false;
  }

  for (i = 0; i < a->num_points; i++) {
    for (j = 0; j < DataFieldCount; j++) {
      if (a->data_points[i].data[j] != b->data_points[i].data[j]) return false;
    }
  }

  return true;
}
