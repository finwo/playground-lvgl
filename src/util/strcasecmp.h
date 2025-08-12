#ifndef __UTIL_STRCASECMP_H__
#define __UTIL_STRCASECMP_H__

#ifdef __cplusplus
extern "C" {
#endif

#if defined(_WIN32) || defined(_WIN64)
int strcasecmp(const char *a, const char *b);
#else
#include <strings.h>
#endif

#ifdef __cplusplus
} // extern "C"
#endif

#endif // __UTIL_STRCASECMP_H__
