#include <stdio.h>

#include "activity.h"
#include "mxml.h"
#include "util.h"

/*
Activity *activity =  activity_new();
int error = gpx_parse("file.gpx", &activity);
activity_destroy(activity);
*/

int gpx_parse(char *filename, Activity *activity) {
  FILE *f = NULL;
  if (!(f = fopen(filename, "r"))) {
    return 1;
  }
}

/*
int main(int argc, char *argv[]) {
  char date[] = "2014-02-23T14:35:18+01:00";
  char buf[21];
  format_timestamp(buf, parse_timestamp(date));
  printf("%s\n", buf);
  return 0;
}
*/
