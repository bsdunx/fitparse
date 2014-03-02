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
 */

#ifndef _FITPARSE_H_
#define _FITPARSE_H_

#include "activity.h"

typedef Activity *(*ReadFn)(char *);
typedef int (*WriteFn)(char *, Activity *);

Activity *fitparse_read(char *filename);
int fitparse_write(char *filename, Activity *activity);
/* helper functions - could just call the *_read or *_write function directly */
Activity *fitparse_read_format(char *filename, FileFormat format);
int fitparse_write_format(char *filename, FileFormat format, Activity *activity);

#endif /* _FITPARSE_H_ */
