#ifndef HAWKTRACER_TEST_TEST_EVENTS_H
#define HAWKTRACER_TEST_TEST_EVENTS_H

#include <hawktracer/core_events.h>

HT_DECLS_BEGIN

HT_DECLARE_EVENT_KLASS(TestCallstackEvent, HT_CallstackBaseEvent, (INTEGER, int, info))

HT_DECLARE_EVENT_KLASS(RegistryTestEvent, HT_Event, (INTEGER, int, field))

HT_DECLARE_EVENT_KLASS(RegistryTestEvent_ID_ONE, HT_Event, (INTEGER, int, field))

HT_DECLARE_EVENT_KLASS(RegistryTestEvent_ID_TWO, HT_Event, (INTEGER, int, field))

HT_DECLARE_EVENT_KLASS(DoubleTestEvent, HT_Event, (DOUBLE, double, field))

HT_DECLARE_EVENT_KLASS(LargeTestEvent, HT_Event,
                       (DOUBLE, double, field1),
                       (DOUBLE, double, field2),
                       (DOUBLE, double, field3),
                       (DOUBLE, double, field4),
                       (DOUBLE, double, field5),
                       (DOUBLE, double, field6),
                       (DOUBLE, double, field7),
                       (DOUBLE, double, field8),
                       (DOUBLE, double, field9),
                       (DOUBLE, double, field10))

HT_DECLARE_EVENT_KLASS(SuperLargeTestEvent, LargeTestEvent,
                       (DOUBLE, double, field11),
                       (DOUBLE, double, field12),
                       (DOUBLE, double, field13),
                       (DOUBLE, double, field14),
                       (DOUBLE, double, field15),
                       (DOUBLE, double, field16),
                       (DOUBLE, double, field17),
                       (DOUBLE, double, field18),
                       (DOUBLE, double, field19),
                       (DOUBLE, double, field20))

HT_DECLS_END

#endif /* HAWKTRACER_TEST_TEST_EVENTS_H */
