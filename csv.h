#ifndef _CSV_H_
#define _CSV_H_

#include "activity.h"

int csv_read(char *filename, Activity *activity);
int csv_write(char *filename, Activity *activity);

#endif /* _CSV_H_ */
