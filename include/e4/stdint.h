#ifndef E4_STDINT_H
#define E4_STDINT_H
#ifdef __cplusplus
extern "C" {
#endif

#if __STDC_VERSION__ >= 199901L
   #include <stdint.h>
#else
   #include "e4/pstdint.h"
#endif

#ifdef __cplusplus
}
#endif
#endif
