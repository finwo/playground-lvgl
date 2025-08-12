#ifdef __cplusplus
extern "C" {
#endif

#include "strcasecmp.h"

#if defined(_WIN32) || defined(_WIN64)

int strcasecmp(const char *a, const char *b) {
  int ca, cb;
  do {
    // Different diacritic options with upper and lower, so merge with both
    ca = tolower(toupper(*(unsigned char *)a));
    cb = tolower(toupper(*(unsigned char *)b));
    a++;
    b++;
  } while(ca == cb && ca != '\0');
  return ca - cb;
}

#endif


#ifdef __cplusplus
} // extern "C"
#endif
