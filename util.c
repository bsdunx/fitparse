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

#include <string.h>
#include <stdint.h>
#include <time.h>
#include <ctype.h>

#include "activity.h"
#include "util.h"
#include "date.h"

/* Parse an ISO_8601 timestamp */
uint32_t parse_timestamp(const char *date) {
  unsigned long timestamp;
  int offset;
  return (parse_date_basic(date, &timestamp, &offset) < 0)
             ? UNSET_FIELD
             : (uint32_t)timestamp;
}

int format_timestamp(char *buf, uint32_t timestamp) {
  time_t time = (time_t)timestamp;
  struct tm *tm = gmtime(&time);
  return !strftime(buf, TIME_BUFSIZ, "%Y-%m-%dT%H:%M:%SZ", tm) ? -1 : 0;
}

char *change_extension(char *filename, char *ext) {
  char *cur = extension(filename);
  if (strlen(cur) != strlen(ext)) return NULL;
  strcpy(cur, ext);
  return filename;
}

double parse_field(DataField field, DataPoint *dp, const char *str) {
  char *end;

  if (!str) return (dp->data[field] = UNSET_FIELD);

  if (field == Timestamp) {
    dp->data[field] = parse_timestamp(str);
  } else {
    dp->data[field] = strtod(str, &end);
    if (*end && !isspace(*end)) dp->data[field] = UNSET_FIELD;
  }
  return dp->data[field];
}
