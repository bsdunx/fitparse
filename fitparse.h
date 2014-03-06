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
 */

#ifndef _FITPARSE_H_
#define _FITPARSE_H_

#define FITPARSE_VERSION "0.0.1"

#include "activity.h"

typedef Activity *(*ReadFn)(FILE *);
typedef int (*WriteFn)(FILE *, Activity *);

Activity *fitparse_read(char *filename);
Activity *fitparse_read_file(FILE *file);
int fitparse_write(char *filename, Activity *a);
/* helper functions - could just call the *_read or *_write function directly */
Activity *fitparse_read_format(char *filename, FileFormat format);
Activity *fitparse_read_format_file(FILE *file, FileFormat format);
int fitparse_write_format(char *filename, FileFormat format, Activity *a);
int fitparse_write_format_file(FILE *file, FileFormat format, Activity *a);

/*
//// TODO some things need athlete or options file...

//[> TODO these will also change how much memory is needed <]
//[> Creates new activity, doesnt erase old ones <]
//Activity *fitparse_merge(Actvity *a, Activity *b);
//[> Modifies old activity <]
//int fitparse_crop(Activity *a, int *TODO);
//[> Modifies old activity and returns new one <]
//Activity *fitparse_split(Activity *a, int TODO);
//[> Performs one or more fixes -> should use masks? <]
//int fitparse_fix(Activity *a, Fix TODO);
*/

#endif /* _FITPARSE_H_ */
