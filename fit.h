#ifndef _FIT_H_
#define _FIT_H_

#include "activity.h"

int fit_read(char *filename, Activity *activity);
int fit_write(char *filename, Activity *activity);

#endif /* _FIT_H_ */
