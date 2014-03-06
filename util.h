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

#ifndef _UTIL_H_
#define _UTIL_H_

#include <ctype.h>
#include <float.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>

#define PI (3.141592653589793)
#define TIME_BUFSIZ 21

static inline double to_radians(double degrees) {
  return degrees * 2 * PI / 360;
}

static inline int doubles_equal(double a, double b) {
  double error_b = b * DBL_EPSILON;
  return (a >= b - error_b) && (a <= b + error_b);
}

static inline char *extension(char *filename) {
  char *s = strrchr(filename, '.');
  return s ? s + 1 : s;
}

static inline char *downcase(char *str) {
  for (; *str; ++str) *str = tolower(*str);
  return str;
}

uint32_t parse_timestamp(const char *date);
int format_timestamp(char *buf, uint32_t timestamp);
char *change_extension(char *filename, char *ext);
FileFormat *file_format(char *ext);
double parse_field(DataField field, DataPoint *dp, const char *str);

#endif /* _UTIL_H_ */
