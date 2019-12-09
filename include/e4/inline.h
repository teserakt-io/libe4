#ifndef E4_INLINE_H
#define E4_INLINE_H

#ifdef __cplusplus
extern "C" {
#endif

#ifdef _MSC_VER
#define inline __inline
#endif

#if __STDC_VERSION__ >= 199901L
   #define INLINE static inline
#else
   #define INLINE static
#endif

#ifdef __cplusplus
}
#endif

#endif
