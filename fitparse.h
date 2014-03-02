#ifndef _FITPARSE_H_
#define _FITPARSE_H_

#include "activity.h"

typedef Activity *(*ReadFn)(char *);
typedef int (*WriteFn)(char *, Activity *);

Activity *fitparse_read(char *filename);
int fitparse_write(char *filename, Activity *activity);
/* helper functions - could just call the *_read or *_write function directly */
Activity *fitparse_read_format(char *filename, FileFormat format);
int fitparse_write_format(char *filename, FileFormat format, Activity *activity);

#endif /* _FITPARSE_H_ */
