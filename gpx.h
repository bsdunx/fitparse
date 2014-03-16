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
 *     Copyright (c) 2010 Greg Lonnon (greg.lonnon@gmail.com)
 *     Copyright (c) 2008 Sean C. Rhea (srhea@srhea.net),
 *     Copyright (c) 2008 J.T Conklin (jtc@acorntoolworks.com)
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

#ifndef _GPX_H_
#define _GPX_H_

#include "activity.h"

#define DEFAULT_GPX_OPTIONS { true, false }

/**
 * GPXOptions
 *
 * Description:
 *  Structure use to specify options for how the GPX should be written.
 *
 * Fields:
 *  add_laps - whether or not to add laps as 'wpt' fields.
 *  lap_trksegs - whether to write a trkseg for each lap.
 */
typedef struct {
  bool add_laps;
  bool lap_trksegs;
} GPXOptions;

Activity *gpx_read(FILE *f);
int gpx_write_options(FILE *f, Activity *a, GPXOptions *o);

/**
 * gpx_write
 *
 * Description:
 *  Write the `Activity` to `f` in GPX format.
 *
 * Parameters:
 *  f - the file descriptor for the GPX file to write to.
 *  a - the `Activity` to write.
 *
 * Return value:
 *  0 - successfully wrote GPX file.
 *  1 - unable to write GPX.
 */
static inline int gpx_write(FILE *f, Activity *a) {
  GPXOptions o = DEFAULT_GPX_OPTIONS;
  return gpx_write(f, a, &o);;
}

#endif /* _GPX_H_ */
