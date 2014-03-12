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
 *     Copyright (c) 2007-2008 Sean C. Rhea (srhea@srhea.net)
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

#include "fit.h"

/**
 * fit_read
 *
 * Description:
 *  Read in the FIT file pointed to by `f` and return an `Activity`.
 *
 * Parameters:
 *  f - The file descriptor for the FIT file to read.
 *
 * Return value:
 *  NULL - unable to read in FIT or invalid FIT file.
 *  valid pointer - a valid pointer to a newly allocated Activity instance.
 *                  The caller is responsible for freeing the activity.
 */
Activity *fit_read(FILE *f) { return NULL; }

/**
 * fit_write
 *
 * Description:
 *  Write the `Activity` to `f` in FIT format.
 *
 * Parameters:
 *  f - the file descriptor for the FIT file to write to.
 *  a - the `Activity` to write.
 *
 * Return value:
 *  0 - successfully wrote FIT file.
 *  1 - unable to write FIT.
 */
int fit_write(FILE *f, Activity *a) { return 1; }
