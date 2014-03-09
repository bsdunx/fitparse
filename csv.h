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

/**
 * CSVOptions
 *
 * Description:
 *  Structure use to specify options for how the CSV should be written.
 *
 * Fields:
 *  remove_unset - whether or not to write unset fields.
 *  unset_value - the string to use to denote an unset field.
 */
typedef struct {
  bool remove_unset;
  char *unset_value;
  /* TODO Something Lap related? */
} CSVOptions;

Activity *csv_read(FILE *f);
int csv_write_options(FILE *f, Activity *activity, CSVOptions options);

/**
 * csv_write
 *
 * Description:
 *  Write the `Activity` to `f` in CSV format.
 *  We default the CSV to print all fields and use 'NA' as the unset value.
 *
 * Parameters:
 *  f - the file descriptor for the CSV file to write to.
 *  a - the `Activity` to write.
 *
 * Return value:
 *  0 - successfully wrote CSV file.
 *  1 - unable to write CSV.
 */
static inline int csv_write(FILE *f, Activity *activity) {
  CSVOptions options = DEFAULT_CSV_OPTIONS;
  return csv_write_options(f, activity, options);
}

#endif /* _CSV_H_ */
