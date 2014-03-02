#ifndef _FIT_H_
#define _FIT_H_

#include "activity.h"

Activity *fit_read(char *filename);
int fit_write(char *filename, Activity *activity);

#endif /* _FIT_H_ */
