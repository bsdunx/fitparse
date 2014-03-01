#ifndef _FITPARSE_H_
#define _FITPARSE_H_

#include "activity.h"

typedef int (*ReadFn)(char *, Activity *);
typedef int (*WriteFn)(char *, Activity *);

int fitparse_read(char *filename, Activity *activity);
int fitparse_write(char *filename, Activity *activity);
/* helper functions - could just call the *_read or *_write function directly */
int fitparse_read_format(char *filename, FileFormat format, Activity *activity);
int fitparse_write_format(char *filename, FileFormat format, Activity *activity);

#endif /* _FITPARSE_H_ */
