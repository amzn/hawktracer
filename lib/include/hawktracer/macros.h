#ifndef HAWKTRACER_MACROS_H
#define HAWKTRACER_MACROS_H

#include <hawktracer/ht_config.h>

/** Helper macro for suppressing unused value warnings. */
#define HT_UNUSED(x) (void)(x)

/** @def HT_DECLS_BEGIN
 * Defines a beginning of C linkage block.
 */
/** @def HT_DECLS_END
 * Defines an end of C linkage block.
 */
#ifdef __cplusplus
#  define HT_DECLS_BEGIN  extern "C" {
#  define HT_DECLS_END    }
#else
#  define HT_DECLS_BEGIN
#  define HT_DECLS_END
#endif

#if (defined(__cplusplus) && __cplusplus >= 201103L) || (defined(_MSVC_LANG) && _MSVC_LANG >= 201402L)
#  define HT_CPP11
#endif

/**
 * @def HT_LIKELY
 * Hints a compiler that the expression is likely to evaluate to true.
 *
 * This information may be used for compiler optimizations.
 */
/**
 * @def HT_UNLIKELY
 * Hints a compiler that the expression is likely to evaluate to false.
 *
 * This information may be used for compiler optimizations.
 */
#if defined(__GNUC__) && (__GNUC__ > 2) && defined(__OPTIMIZE__)
#  define HT_LIKELY(expression) (__builtin_expect ((expression), 1))
#  define HT_UNLIKELY(expression) (__builtin_expect ((expression), 0))
#else
#  define HT_LIKELY(expr) (expr)
#  define HT_UNLIKELY(expr) (expr)
#endif

/** @def HT_INLINE
 * Defines an inline directive for inlining functions.
 */
#ifndef __cplusplus
#  if defined (_MSC_VER) && (_MSC_VER < 1900)
#    define HT_INLINE __inline
#  elif !defined(__STDC_VERSION__) || (__STDC_VERSION__ < 199900)
#    define HT_INLINE __inline
#  else
#    define HT_INLINE inline
#  endif
#else
#  define HT_INLINE inline
#endif

#ifdef _WIN32
#  define HT_PLATFORM_WIN32
#endif

/** @def HT_API
 * Defines a directive for exporting/importing symbols from DLL.
 */
#if defined(HT_PLATFORM_WIN32) && !defined(HT_COMPILE_STATIC)
#  if defined(HT_COMPILE_SHARED_EXPORT)
#    define HT_API __declspec(dllexport)
#  else
#    define HT_API __declspec(dllimport)
#  endif
#else
#  define HT_API
#endif

#define HT_MACRO_CONCAT_(x, y) x ## y

/** Concatenates two values */
#define HT_MACRO_CONCAT(x, y) HT_MACRO_CONCAT_(x, y)

/** Creates unique variable name by appending a line number to a base name. */
#define HT_UNIQUE_VAR_NAME(BASE_VAR_NAME) HT_MACRO_CONCAT(BASE_VAR_NAME, __LINE__)

/**
 * @def HT_THREAD_LOCAL
 * A thread storage duration.
 */
#ifndef HT_ENABLE_THREADS
#  define HT_THREAD_LOCAL
#elif defined(HT_CPP11)
#  define HT_THREAD_LOCAL thread_local
#elif __STDC_VERSION__ >= 201112 && !defined __STDC_NO_THREADS__
#  define HT_THREAD_LOCAL _Thread_local
#elif defined _WIN32 && ( defined _MSC_VER || defined __ICL || defined __DMC__ || defined __BORLANDC__ )
#  define HT_THREAD_LOCAL __declspec(thread)
#elif defined __GNUC__ || defined __SUNPRO_C || defined __xlC__ /* ICC (linux) and Clang are covered by __GNUC__ */
#  define HT_THREAD_LOCAL __thread
#endif

#if defined(HT_HAVE_UNISTD_H) || (defined(__unix__) || (defined(__APPLE__) && defined(__MACH__)))
#  define HT_HAVE_UNISTD_H
#endif

/* Integer constants */
/* int32_t max value */
#define HT_INT32_MAX (2147483647)
/* size_t max value */
#define HT_SIZE_MAX ((size_t)-1)

#endif /* HAWKTRACER_MACROS_H */
