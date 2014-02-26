#ifndef _GPX_H_
#define _GPX_H_

#include "activity.h"

int gpx_read(char *filename, Activity *activity);
int gpx_write(char *filename, Activity *activity);

#endif /* _GPX_H_ */
