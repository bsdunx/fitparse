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
  Activity *a;
  if (!(a = malloc(sizeof(*a)))) {
    return NULL;
  }
  a->sport = UnknownSport;
  a->format = UnknownFileFormat;
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
/* TODO see gpx parser for additional checks we must perform */
int activity_add_point(Activity *a, DataPoint *dp) {
  unsigned i;

  ALLOC_GROW(a->data_points, a->num_points + 1, a->points_alloc);
  if (!(a->data_points)) {
    return 1;
  }

  /* TODO fill in inferred missing values a la gpx/tcx */
  for (i = 0; i < DataFieldCount; i++) {
    a->data_points[a->num_points].data[i] = dp->data[i];
    if (dp->data[i] != UNSET_FIELD && !a->has_data[i]) {
      a->has_data[i] = true;
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

bool activity_equal(Activity *a, Activity *b) {
  return false; /* TODO */
}
