#include "get_bin_path.h"

#include <stdlib.h>

#if defined(_WIN32) || defined(_WIN64)
#include <windows.h>
#else
#include <unistd.h>
#include <linux/limits.h>
#endif

#ifndef MIN
#define MIN(a,b) (a<b?a:b)
#endif


char * get_bin_path() {
#if defined(_WIN32) || defined(_WIN64)
  int len = MAX_PATH + 1;
#else
  int len = PATH_MAX + 1;
#endif
  char *output = calloc(len, sizeof(char));

#if defined(_WIN32) || defined(_WIN64)
  int nread = GetModuleFileName(NULL, output, len);
#else
  int nread = MIN(readlink("/proc/self/exe", output, len), len - 1);
  if(nread >= 0) output[nread] = '\0';
#endif

  if (nread < 0) {
    free(output);
    output = NULL;
  }

  return output;
};
