#ifndef _UTIL_H_
#define _UTIL_H_

#include <stdint.h>

uint32_t parse_timestamp(const char *date);
int format_timestamp(char *buf, uint32_t timestamp);

#endif /* _UTIL_H_ */
