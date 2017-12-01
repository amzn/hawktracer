#ifndef HAWKTRACER_MACROS_H
#define HAWKTRACER_MACROS_H

#define HT_UNUSED(x) (void)(x)

#ifdef __cplusplus
#  define HT_DECLS_BEGIN  extern "C" {
#  define HT_DECLS_END    }
#else
#  define HT_DECLS_BEGIN
#  define HT_DECLS_END
#endif

#if defined(__GNUC__) && (__GNUC__ > 2) && defined(__OPTIMIZE__)
#  define HT_LIKELY(expr) (__builtin_expect ((expr), 1))
#  define HT_UNLIKELY(expr) (__builtin_expect ((expr), 0))
#else
#  define HT_LIKELY(expr) (expr)
#  define HT_UNLIKELY(expr) (expr)
#endif

#ifndef __cplusplus
#  ifdef _MSC_VER
#    if (_MSC_VER < 1900)
#      define inline __inline
#    endif
#    elif !defined(__STDC_VERSION__) || (__STDC_VERSION__ < 199900)
#      define inline __inline
#    endif
#endif

#endif /* HAWKTRACER_MACROS_H */
