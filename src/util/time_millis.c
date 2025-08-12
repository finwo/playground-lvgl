#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

#if defined(_WIN32) || defined(_WIN64)
#include <sys/timeb.h>
#include <sys/types.h>
#else
#include <sys/time.h>
#endif

#ifndef NULL
#define NULL ((void*)0)
#endif

int64_t time_millis() {
#if defined(_WIN32) || defined(_WIN64)
  struct _timeb timebuffer;
  _ftime(&timebuffer);
  return (int64_t)(((timebuffer.time * 1000) + timebuffer.millitm));
#else
  struct timeval tv;
  gettimeofday(&tv, NULL);
  return (tv.tv_sec * ((int64_t)1000)) + (tv.tv_usec / 1000);
#endif
}

#ifdef __cplusplus
} // extern "C"
#endif
