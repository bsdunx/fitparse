#ifndef _CSV_H_
#define _CSV_H_

#include "activity.h"

#define DEFAULT_CSV_OPTIONS \
  { false, "NA" }
#define CSV_BUFSIZ 4096
#define CSV_FIELD_SIZE 32

typedef struct {
  bool remove_unset;
  char *unset_value;
  /* Something Lap related? */
} CSVOptions;

int csv_read(char *filename, Activity *activity);
int csv_write_options(char *filename, Activity *activity, CSVOptions options);

static inline int csv_write(char *filename, Activity *activity) {
  CSVOptions options = DEFAULT_CSV_OPTIONS;
  return csv_write_options(filename, activity, options);
}

#endif /* _CSV_H_ */
