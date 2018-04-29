#ifndef HAWKTRACER_MONOTONIC_CLOCK_H
#define HAWKTRACER_MONOTONIC_CLOCK_H

#include <hawktracer/base_types.h>

HT_DECLS_BEGIN

/**
 * Gets a current timestamp of a monotonic clock.
 *
 * The timestamp of this clock can not decrease as physical
 * time moves forward. Although the timestamp doesn't represent wall
 * clock time, returned values can be used e.g. for measuring
 * intervals.
 *
 * @return a current timestamp of the clock.
 */
HT_API HT_TimestampNs ht_monotonic_clock_get_timestamp(void);

HT_DECLS_END

#endif /* HAWKTRACER_MONOTONIC_CLOCK_H */
