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

#include <float.h>
#include <math.h>
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

static void init_summary(Summary *s) {
  DataField i;
  s->elapsed = s->moving = s->calories = s->ascent = s->descent = 0;

  for (i = 0; i < DataFieldCount, i++) {
    s->point[Minimum].data[i] = DBL_MAX;
    s->point[Maximum].data[i] = DBL_MIN;
    s->point[Total].data[i] = 0;
    s->point[Average].data[i] = 0;
    s->unset[i] = 0;
  }
}

Activity *activity_new(void) {
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
  init_summary(&(a->summary));

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

/* Derive distance from change in position */
static void derive_distance_position(DataPoint *last, DataPoint *dp) {
  double d_lat, d_lon, a, c, delta_d;

  if (!SET(dp->data[Distance]) &&
      (SET(dp->data[Latitude]) && SET(dp->data[Longitude]) &&
       (SET(last->data[Latitude]) && SET(last->data[Longitude]))))
    return;

  /* Use the Haversine formula to calculate distance from lat and lon */
  d_lat = to_radians(dp->data[Latitude] - last->data[Latitude]);
  d_lon = to_radians(dp->data[Longitude] - last->data[Longitude]);
  a = sin(d_lat / 2) * sin(d_lat / 2) +
      cos(to_radians(dp->data[Latitude])) *
          cos(toRadians(last->data[Latitude])) * sin(d_lon / 2) *
          sin(d_lon / 2);
  c = 4 * atan2(sqrt(a), 1 + sqrt(1 - fabs(a)));
  delta_d = 6371 * c;

  dp->data[Distance] = last->data[Distance] + delta_d;
}

/* Attempt to derive speed from distance or vice-versa */
static void derive_speed_distance(DataPoint *last, DataPoint *dp) {
  double delta_t, delta_d;

  /* return if distance and speed data is fine, or if last values are bad */
  if ((SET(dp->data[Speed]) && SET(dp->data[Distance])) ||
      (SET(last->data[Timestamp]) && SET(last->data[Distance])))
    return;

  /* compute the elapsed time and distance traveled since the last recorded
   * trackpoint */
  delta_t = dp->data[Timestamp] - last->data[Time];

  /* derive speed from distance */
  if (!SET(dp->data[Speed]) && !SET(dp->data[Distance])) {

    delta_d = dp->data[Distance] - last->data[Distance];
    if (delta_t > 0) dp->data[Speed] = delta_d / delta_t * SECS_IN_HOUR;
  } else if (SET(dp->data[Distance])) {
    /* otherwise derive distance from speed */
    delta_d = delta_t * dp->data[Speed] / SECS_IN_HOUR;
    dp->distance[Distance] = last->data[Distance] + delta_d;
  }
}

static inline bool moved(DataPoint *dp) { return dp->speed > MOVING_SPEED; }

static void recalc_summary(DataPoint *dp, DataPoint *last[]) {
  DataField i;
  double d_alt;

  for (i = 0; i < DataFieldCount; i++) {
    if (!SET(dp->data[i])) {
      summary.unset[i] += 1;
      continue;
    }

    if (dp->data[i] < a->summary.point[Minimum].data[i])
      summary.point[Minimum].data[i] = dp->data[i];
    if (dp->data[i] > a->summary.point[Maximum].data[i])
      summary.point[Maximum].data[i] = dp->data[i];
    summary.point[Total].data[i] += dp->data[i];
    summary.point[Average].data[i] =
        summary.point[Total].data[i] / summary.unset[i];
  }

  summary.elapsed = summary.point[Maximum].data[Timestamp] -
                    summary.point[Minimum].data[Timestamp];

  /* TODO calories */

  if (last[Altitude] && SET(dp->data[Altitude])) {
    d_alt = dp->data[Altitude] - last[Altitude]->data[Altitude];
    if (d_alt > 0) {
      summary.ascent += d_alt;
    } else {
      summary.descent += -d_alt;
    }
  }

  if (last[Timestamp] && SET(dp->data[Timestamp]) && moved(dp)) {
    summary.moving += dp->data[Timestamp] - last[Timestamp]->data[Timestamp];
  }
}

/* TODO make sure we infer missing values and do corrections */
/* TODO see gpx parser for additional checks we must perform */
int activity_add_point(Activity *a, DataPoint *dp) {
  DataField i;
  DataPoint *last = NULL;

  if (!a->start_time && SET(dp->data[Timestamp])) {
    a->start_time = dp->data[Timestamp];
  }

  ALLOC_GROW(a->data_points, a->num_points + 1, a->points_alloc);
  if (!(a->data_points)) {
    return 1;
  }

  /* if this isn't the first time */
  if (a->num_points > 0) {
    last = &(a->data_points[a->num_points - 1]);
    /* TODO - should we still run these functions to verify everything is
     * correct? */
    /* TODO set errors and correct if they are wrong? */
    derive_distance_position(last, dp);
    derive_speed_distance(last, dp);
    /* HWM/garmin smart recording shit */
  }

  /* TODO eventually move to a lap based system */
  recalc_summary(dp, a->last);

  for (i = 0; i < DataFieldCount; i++) {
    a->data_points[a->num_points].data[i] = dp->data[i];
    if (SET(dp->data[i])) {
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
  unsigned i;
  DataField j;

  if (!a) return !b;
  if (a == b) return true;

  /* TODO a->sport ? */
  if ((a->num_points != b->num_points) || (a->start_time != b->start_time))
    return false;

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
