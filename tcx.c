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

#include "tcx.h"

/**
 * tcx_read
 *
 * Description:
 *  Read in the TCX file pointed to by `f` and return an `Activity`.
 *
 * Parameters:
 *  f - The file descriptor for the TCX file to read.
 *
 * Return value:
 *  NULL - unable to read in TCX or invalid TCX file.
 *  valid pointer - a valid pointer to a newly allocated Activity instance.
 *                  The caller is responsible for freeing the activity.
 */
Activity *tcx_read(FILE *f) { return NULL; }

/**
 * tcx_write
 *
 * Description:
 *  Write the `Activity` to `f` in TCX format.
 *
 * Parameters:
 *  f - the file descriptor for the TCX file to write to.
 *  a - the `Activity` to write.
 *
 * Return value:
 *  0 - successfully wrote TCX file.
 *  1 - unable to write TCX.
 */
int tcx_write(FILE *f, Activity *activity) { return 1; }
