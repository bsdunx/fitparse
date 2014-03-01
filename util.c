#include <stdint.h>
#include <time.h>

#include "activity.h"
#include "util.h"
#include "date.h"

/* Parse an ISO_8601 timestamp */
uint32_t parse_timestamp(const char *date) {
  unsigned long timestamp;
  int offset;
  return (parse_date_basic(date, &timestamp, &offset) < 0) ? UNSET_FIELD : (uint32_t) timestamp;
}

int format_timestamp(char *buf, uint32_t timestamp) {
  time_t time = (time_t) timestamp;
  struct tm *tm = gmtime(&time);
  return !strftime(buf, 21, "%Y-%m-%dT%H:%M:%SZ", tm) ? -1 : 0;
}
