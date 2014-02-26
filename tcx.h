#ifndef _TCX_H_
#define _TCX_H_

#include "activity.h"

int tcx_read(char *filename, Activity *activity);
int tcx_write(char *filename, Activity *activity);

#endif /* _TCX_H_ */
