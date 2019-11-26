#ifndef E4_INLINE_H
#define E4_INLINE_H

#ifdef _MSC_VER
#define inline __inline
#endif

#if __STDC_VERSION__ >= 199901L
   #define INLINE static inline
#else
   #define INLINE static
#endif

#endif
