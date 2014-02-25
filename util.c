#include <stdint.h>
#include <time.h>

#include "date.h"

/* Parse an ISO_8601 timestamp */
uint32_t parse_timestamp(const char *date) {
  unsigned long timestamp;
  int offset;
  return (parse_date_basic(date, &timestamp, &offset) < 0) ? 0 : (uint32_t)timestamp;
}

int format_timestamp(char *buf, uint32_t timestamp) {
  time_t time = (time_t)timestamp;
  struct tm *tm = gmtime(&time);
  return !strftime(buf, 21, "%Y-%m-%dT%H:%M:%SZ", tm) ? -1 : 0;
}

#include <stdio.h>
int main(int argc, char *argv[]) {
  char date[] = "2014-02-23T14:35:18+01:00";
  char buf[21];
  format_timestamp(buf, parse_timestamp(date));
  printf("%s\n", buf);
  return 0;
}

