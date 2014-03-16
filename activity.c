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

#include <assert.h>
#include <float.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>

#include "activity.h"
#include "util.h"

/**
 * init_summary
 *
 * Description:
 *  Initializes the `Summary` object of the `Activity`.
 *
 * Parameters:
 *  s - a pointer to the `Summary` data for the activity.
 */
static void init_summary(Summary *s) {
  DataField i;
  s->elapsed = s->moving = s->calories = s->ascent = s->descent = 0;

  for (i = 0; i < DataFieldCount; i++) {
    s->point[Minimum].data[i] = DBL_MAX;
    s->point[Maximum].data[i] = DBL_MIN;
    s->point[Total].data[i] = 0;
    s->point[Average].data[i] = 0;
    s->unset[i] = 0;
  }
}

/**
 * activity_new
 *
 * Description:
 *  Instantiates a new `Activity` object. If a valid pointer is returned it
 *  must be free'd with a call to `activity_destroy`.
 *
 * Return value:
 *  NULL - unable to create a new `Activity` object.
 *  valid pointer - the pointer to the `Activity`.
 */
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
  memset(a->last, 0, sizeof(*(a->last)));
  init_summary(&(a->summary));

  return a;
}

/**
 * activity_destroy
 *
 * Description:
 *  Destroys an `Activity` object that was created by `activity_new` and frees
 *  all the associated memory.
 *
 * Parameters:
 *  a - a non-NULL `Activity` pointer created by `activity_new`.
 */
void activity_destroy(Activity *a) {
  assert(a != NULL);

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

/**
 * derive_distance_position
 *
 * Description:
 *  Derive distance from change in position. Using the Haversine formula we are
 *  able to infer distance travelled between two (lat,lon) points.
 *
 * Parameters:
 *  prev - the previous `DataPoint` which has speed and distance paramters set.
 *  dp - the `DataPoint` to potentially dervice speed or distance for.
 */
static void derive_distance_position(DataPoint *prev, DataPoint *dp) {
  double d_lat, d_lon, a, c, delta_d;

  if (!SET(dp->data[Distance]) &&
      (SET(dp->data[Latitude]) && SET(dp->data[Longitude]) &&
       (SET(prev->data[Latitude]) && SET(prev->data[Longitude]))))
    return;

  /* Use the Haversine formula to calculate distance from lat and lon */
  d_lat = to_radians(dp->data[Latitude] - prev->data[Latitude]);
  d_lon = to_radians(dp->data[Longitude] - prev->data[Longitude]);
  a = sin(d_lat / 2) * sin(d_lat / 2) +
      cos(to_radians(dp->data[Latitude])) *
          cos(to_radians(prev->data[Latitude])) * sin(d_lon / 2) *
          sin(d_lon / 2);
  c = 4 * atan2(sqrt(a), 1 + sqrt(1 - fabs(a)));
  delta_d = 6371 * c;

  dp->data[Distance] = prev->data[Distance] + delta_d;
}

/**
 * derive_speed_distance
 *
 * Description:
 *  Attempt to derive speed from distance or vice-versa. If we know one of the
 *  values we can always computer the other.
 *
 * Parameters:
 *  prev - the previous `DataPoint` which has speed and distance paramters set.
 *  dp - the `DataPoint` to potentially dervice speed or distance for.
 */
static void derive_speed_distance(DataPoint *prev, DataPoint *dp) {
  double delta_t, delta_d;

  /* return if distance and speed data is fine, or if prev values are bad */
  if ((SET(dp->data[Speed]) && SET(dp->data[Distance])) ||
      (SET(prev->data[Timestamp]) && SET(prev->data[Distance])))
    return;

  /* compute the elapsed time and distance traveled since the prev recorded
   * trackpoint */
  delta_t = dp->data[Timestamp] - prev->data[Timestamp];

  /* derive speed from distance */
  if (!SET(dp->data[Speed]) && !SET(dp->data[Distance])) {

    delta_d = dp->data[Distance] - prev->data[Distance];
    if (delta_t > 0) dp->data[Speed] = delta_d / delta_t * SECS_IN_HOUR;
  } else if (SET(dp->data[Distance])) {
    /* otherwise derive distance from speed */
    delta_d = delta_t * dp->data[Speed] / SECS_IN_HOUR;
    dp->data[Distance] = prev->data[Distance] + delta_d;
  }
}

/**
 * recalc_summary
 *
 * Description:
 *  Recompute the summary data for the `Activity` to account for the new
 *  information in the `DataPoint`.
 *
 * Parameters:
 *  a - the `Activity` to update.
 *  dp - the newest `DataPoint` information to update the `Summary` with.
 */
static void recalc_summary(Activity *a, DataPoint *dp) {
  DataField i;
  DataPoint **last = a->last;
  double d_alt;

  for (i = 0; i < DataFieldCount; i++) {
    if (!SET(dp->data[i])) {
      a->summary.unset[i] += 1;
      continue;
    }

    if (dp->data[i] < a->summary.point[Minimum].data[i])
      a->summary.point[Minimum].data[i] = dp->data[i];
    if (dp->data[i] > a->summary.point[Maximum].data[i])
      a->summary.point[Maximum].data[i] = dp->data[i];

    a->summary.point[Total].data[i] += dp->data[i];
    a->summary.point[Average].data[i] =
        a->summary.point[Total].data[i] / a->summary.unset[i];
  }

  a->summary.elapsed = a->summary.point[Maximum].data[Timestamp] -
                       a->summary.point[Minimum].data[Timestamp];

  /* TODO calories */

  if (last[Altitude] && SET(dp->data[Altitude])) {
    d_alt = dp->data[Altitude] - last[Altitude]->data[Altitude];
    if (d_alt > 0) {
      a->summary.ascent += d_alt;
    } else {
      a->summary.descent += -d_alt;
    }
  }

  if (last[Timestamp] && SET(dp->data[Timestamp]) &&
      (dp->data[Speed] > MOVING_SPEED)) {
    a->summary.moving += dp->data[Timestamp] - last[Timestamp]->data[Timestamp];
  }
}

/**
 * activity_add_point
 *
 * Description:
 *  Add a new `DataPoint` to the `Activity`. We assume the `DataPoint` is the
 *  next point chronologically and correct any information that might be
 *  missing or wrong within the point.
 *
 * Parameters:
 *  a - the `Activity` to add the point to.
 *  dp - the `DataPoint` to add.
 *
 * Return value:
 *  0 - if the point was added successfully
 *  1 - if there was an issue adding the `DataPoint`.
 */
int activity_add_point(Activity *a, DataPoint *dp) {
  DataField i;
  DataPoint *prev = NULL;

  if (!a->start_time && SET(dp->data[Timestamp])) {
    a->start_time = dp->data[Timestamp];
  }

  ALLOC_GROW(a->data_points, a->num_points + 1, a->points_alloc);
  if (!(a->data_points)) {
    return 1;
  }

  /* if this isn't the first time */
  if (a->num_points > 0) {
    prev = &(a->data_points[a->num_points - 1]);
    /* TODO - should we still run these functions to verify everything is
     * correct? */
    /* TODO set errors and correct if they are wrong? */
    derive_distance_position(prev, dp);
    derive_speed_distance(prev, dp);
    /* HWM/garmin smart recording shit */
  }

  /* TODO eventually move to a lap based system */
  recalc_summary(a, dp);

  for (i = 0; i < DataFieldCount; i++) {
    a->data_points[a->num_points].data[i] = dp->data[i];
    if (SET(dp->data[i])) {
      a->last[i] = &(a->data_points[a->num_points]);
    }
  }

  a->num_points++;
  return 0;
}

/**
 * activity_add_lap
 *
 * Description:
 *  TODO
 *
 * Parameters:
 *
 * Return value:
 */
int activity_add_lap(Activity *a, uint32_t lap) {
  if (a->laps) {
    /* see if theres enough space else realloc */
  } else {
    /* alloc a certain amount of space */
  }
  /* add activity->laps[next] = lap; */
  return 0;
}

/**
 * activity_equal
 *
 * Description:
 *  Determines whether two `Activity` objects are equivalent.
 *
 * Parameters:
 *  a - the first `Activity`.
 *  b - the second `Activity`.
 *
 * Return value:
 *  true - if the `Activity` objects are equal
 *  false - otherwise
 */
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
    if ((a->last[i] && !b->last[i]) || (!a->last[i] && b->last[i]))
      return false;
  }

  for (i = 0; i < a->num_points; i++) {
    for (j = 0; j < DataFieldCount; j++) {
      if (a->data_points[i].data[j] != b->data_points[i].data[j]) return false;
    }
  }

  return true;
}
