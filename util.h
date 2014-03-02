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

#ifndef _UTIL_H_
#define _UTIL_H_

#include <stdint.h>
#include <ctype.h>
#include <string.h>

#define TIME_BUFSIZ 32

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

#endif /* _UTIL_H_ */
