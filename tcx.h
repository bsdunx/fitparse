#ifndef _TCX_H_
#define _TCX_H_

#include "activity.h"

Activity *tcx_read(char *filename);
int tcx_write(char *filename, Activity *activity);

#endif /* _TCX_H_ */
