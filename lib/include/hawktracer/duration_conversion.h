#ifndef HAWKTRACER_DURATION_CONVERSION_H
#define HAWKTRACER_DURATION_CONVERSION_H

/** Converts @p X value to nanoseconds. */
#define HT_DUR_NS(X) X
/** Converts @p X value to microseconds. */
#define HT_DUR_US(X) (1000u * X)
/** Converts @p X value to milliseconds. */
#define HT_DUR_MS(X) (1000u * HT_DUR_US(X))
/** Converts @p X value to seconds. */
#define HT_DUR_S(X) (1000u * HT_DUR_MS(X))

#endif /* HAWKTRACER_DURATION_CONVERSION_H */
