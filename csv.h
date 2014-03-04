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

#ifndef _CSV_H_
#define _CSV_H_

#include "activity.h"

#define DEFAULT_CSV_OPTIONS \
  { false, "NA" }
#define CSV_BUFSIZ 4096
#define CSV_FIELD_SIZE 32
#define CSV_MAX_FIELDS 1024

typedef struct {
  bool remove_unset;
  char *unset_value;
  /* Something Lap related? */
} CSVOptions;

Activity *csv_read(char *filename);
int csv_write_options(char *filename, Activity *activity, CSVOptions options);

static inline int csv_write(char *filename, Activity *activity) {
  CSVOptions options = DEFAULT_CSV_OPTIONS;
  return csv_write_options(filename, activity, options);
}

#endif /* _CSV_H_ */
