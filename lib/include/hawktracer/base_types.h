#ifndef HAWKTRACER_BASE_TYPES_H
#define HAWKTRACER_BASE_TYPES_H

#include <hawktracer/macros.h>

#include <inttypes.h>

HT_DECLS_BEGIN

/** An unsigned integer used for event klass identifiers. */
typedef uint32_t HT_EventKlassId;
/** An unsigned integer used for representing timestamps in nanoseconds. */
typedef uint64_t HT_TimestampNs;
/** An unsigned integer used for event identifiers. */
typedef uint64_t HT_EventId;
/** An unsigned integer used for representing duration in nanoseconds. */
typedef uint64_t HT_DurationNs;
/** A standard boolean type, possible values: #HT_TRUE, #HT_FALSE */
typedef int HT_Boolean;
/** A pointer to a list of HT_Event (or derived) objects. */
typedef uint8_t* TEventPtr;
/** An unsigned integer used for thread identifiers. */
typedef uint32_t HT_ThreadId;
/** An unsigned integer guaranteed to be 8 bits on all platforms. */
typedef uint8_t HT_Byte;

/** Defines @b true value for the HT_Boolean type. */
#define HT_TRUE 1
/** Defines @b false value for the HT_Boolean type. */
#define HT_FALSE 0

HT_DECLS_END

#endif /* HAWKTRACER_BASE_TYPES_H */
