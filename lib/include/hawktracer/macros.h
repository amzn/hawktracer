#ifndef HAWKTRACER_MACROS_H
#define HAWKTRACER_MACROS_H

#include <hawktracer/config.h>

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

#if __cplusplus >= 201103L
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

/** @def inline
 * Defines an inline directive for inlining functions.
 */
#ifndef __cplusplus
#  ifdef _MSC_VER
#    if (_MSC_VER < 1900)
#      define inline __inline
#    endif
#    elif !defined(__STDC_VERSION__) || (__STDC_VERSION__ < 199900)
#      define inline __inline
#    endif
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

#endif /* HAWKTRACER_MACROS_H */
