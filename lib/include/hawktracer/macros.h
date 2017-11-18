#ifndef HAWKTRACER_MACROS_H
#define HAWKTRACER_MACROS_H

#define HT_UNUSED(x) (void)(x)

#ifdef __cplusplus
#define HT_DECLS_BEGIN  extern "C" {
#define HT_DECLS_END    }
#else
#define HT_DECLS_BEGIN
#define HT_DECLS_END
#endif

#if defined(__GNUC__) && (__GNUC__ > 2) && defined(__OPTIMIZE__)
#define HT_LIKELY(expr) (__builtin_expect ((expr), 1))
#define HT_UNLIKELY(expr) (__builtin_expect ((expr), 0))
#else
#define HT_LIKELY(expr) (expr)
#define HT_UNLIKELY(expr) (expr)
#endif

#endif /* HAWKTRACER_MACROS_H */
