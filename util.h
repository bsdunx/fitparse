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

#include "activity.h"

/* Standard growing factor for reallocation */
#define alloc_nr(x) (((x) + 16) * 3 / 2)

/* Realloc the buffer pointed at by variable 'x' so that it can hold
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

#define PI (3.141592653589793)
#define TIME_BUFSIZ 21

typedef struct {
  uint32_t *data;
  size_t size;
  size_t alloc;
} Vector;

static inline int vector_add(Vector *v, uint32_t p) {
  ALLOC_GROW(v->data, v->size + 1, v->alloc);
  if (!v->data) {
    return 1;
  }
  v->data[v.size] = p;
  v->size++;
  return 0;
}

static inline void vector_destroy(Vector *v) {
  if (v->data) free(v->data);
}

static inline double to_radians(double degrees) {
  return degrees * 2 * PI / 360;
}

static inline int doubles_equal(double a, double b) {
  double error_b = b * DBL_EPSILON;
  return (a >= b - error_b) && (a <= b + error_b);
}

static inline char *extension(char *filename) {
  char *s = strrchr(filename, '.');
  return s ? s + 1 : "";
}

static inline char *downcase(char *str) {
  for (; *str; ++str) *str = tolower(*str);
  return str;
}

uint32_t parse_timestamp(const char *date);
int format_timestamp(char *buf, uint32_t timestamp);
char *change_extension(char *filename, char *ext);
FileFormat file_format(char *ext);
double parse_field(DataField field, DataPoint *dp, const char *str);

#endif /* _UTIL_H_ */
