#ifndef _FITPARSE_H_
#define _FITPARSE_H_

#include "activity.h"

typedef int (*ReadFn)(char *, Activity *);
typedef int (*WriteFn)(char *, Activity *);

typedef enum {
  CSV,
  GPX,
  TCX,
  FIT,
  UnknownFileFormat
} FileFormat;

int fitparse_read(Activity *activity, char *filename);
int fitparse_write(Activity *activity, char *filename);
/* helper functions - could just call the *_read or *_write function directly */
int fitparse_read_format(Activity *activity, char *filename, FileFormat format);
int fitparse_write_format(Activity *activity, char *filename,
                          FileFormat format);

#endif /* _FITPARSE_H_ */
