#ifndef HAWKTRACER_TEST_TEST_EVENTS_H
#define HAWKTRACER_TEST_TEST_EVENTS_H

#include <hawktracer/core_events.h>

HT_DECLS_BEGIN

HT_DECLARE_EVENT_KLASS(TestCallstackEvent, HT_CallstackBaseEvent, (INTEGER, int, info))

HT_DECLARE_EVENT_KLASS(RegistryTestEvent, HT_Event, (INTEGER, int, field))

HT_DECLARE_EVENT_KLASS(RegistryTestEvent_ID_ONE, HT_Event, (INTEGER, int, field))

HT_DECLARE_EVENT_KLASS(RegistryTestEvent_ID_TWO, HT_Event, (INTEGER, int, field))

HT_DECLS_END

#endif /* HAWKTRACER_TEST_TEST_EVENTS_H */
