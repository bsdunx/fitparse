#ifndef _CSV_H_
#define _CSV_H_

#include "activity.h"

#define DEFAULT_CSV_OPTIONS \
  { false, "NA" }
#define CSV_BUFSIZ 4096

typedef struct {
  bool remove_unset;
  char *unset_value;
  /* Something Lap related? */
} CSVOptions;

static int csv_write(char *filename, Activity *activity) {
  return csv_write(filename, activity, DEFAULT_CSV_OPTIONS);
}

int csv_read(char *filename, Activity *activity);
int csv_write(char *filename, Activity *activity, CSVOptions options);

#endif /* _CSV_H_ */
