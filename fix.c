/*
 * Copyright (c) 2014 Kirk Scheibelhut <kjs@scheibo.com>
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
 *     Copyright (c) 2013 Jaime Jofre (jaimeajofre@hotmail.com)
 *     Copyright (c) 2010 Mark Liversedge (liversedge@gmail.com)
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

#include "activity.h"
#include "fix.h"

/* Remove GPS errors and interpolate positional data where the GPS device
 * did not record any data, or the data that was recorded is invalid. */

/* used to handle gaps in recording by inserting interpolated/zero samples to
 * ensure dataPoints are contiguous in time */
int fix_invalid_gps(Activity *a) {
  DataPoint dp, good;
  double delta_latitude, delta_longitude;
  int errors = 0, last_good = -1, i, j;

  assert(a != NULL);

  /* ignore files without GPS data */
  if (!a->last[Latitude] || !a->last[Longitude]) return -1;

  for (i = 0; i < a->num_points; i++) {
    dp = a->data_points[i];
    /* is this one decent? */
    if (!(dp.data[Latitude] >= -90 && dp.data[Latitude] <= 90 &&
          dp.data[Longitude] >= -180 && dp.data[Longitude] <= 180))
      continue;

    if (last_good != -1 && (last_good + 1) != i) {
      good = a->data_points[last_good];

      /* interpolate from last good to here then set last_good to here */
      delta_latitude =
          (dp.data[Latitude] - good.data[Latitude]) / (double)(i - last_good);
      delta_longitude =
          (dp.data[Longitude] - good.data[Latitude]) / (double)(i - last_good);

      for (j = last_good + 1; j < i; j++) {
        a->data_points[j].data[Latitude] =
            good.data[Latitude] + ((j - last_good) * delta_latitude);
        a->data_points[j].data[Longitude] =
            good.data[Longitude] + ((j - last_good) * delta_longitude);
        errors++;
      }

    } else if (last_good == -1) {
      /* fill to front */
      for (j = 0; j < i; j++) {
        a->data_points[j].data[Latitude] = dp.data[Latitude];
        a->data_points[j].data[Longitude] = dp.data[Longitude];
        errors++;
      }
    }

    last_good = i;
  }

  /* fill to end... */
  if (last_good != -1 && last_good != (a->num_points - 1)) {
    good = a->data_points[last_good];
    /* fill from last_good to end with last_good */
    for (j = last_good + 1; j < a->num_points; j++) {
      a->data_points[j].data[Latitude] = good.data[Latitude];
      a->data_points[j].data[Longitude] = good.data[Longitude];
      errors++;
    }
  }

  if (errors) {
    a->errors[InvalidGPS] = errors;
    return errors;
  } else {
    return 0;
  }
}

/*
   int fix_dropouts(Activity *a);
   int fix_power(Activity *a);
   int fix_heart_rate(Activity *a); // TODO probably needs HR max from athletes
   and
   other
   */
