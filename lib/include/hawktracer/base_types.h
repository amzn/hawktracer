#ifndef HAWKTRACER_BASE_TYPES_H
#define HAWKTRACER_BASE_TYPES_H

#include <hawktracer/macros.h>

#include <inttypes.h>

HT_DECLS_BEGIN

typedef uint32_t HT_EventKlassId;
typedef uint64_t HT_TimestampNs;
typedef uint64_t HT_EventId;
typedef uint64_t HT_DurationNs;
typedef int HT_Boolean;
typedef uint8_t* TEventPtr;
typedef uint32_t HT_ThreadId;
typedef uint8_t HT_Byte;

#define HT_TRUE 1
#define HT_FALSE 0

HT_DECLS_END

#endif /* HAWKTRACER_BASE_TYPES_H */
