#include "hawktracer/init.h"
#include "hawktracer/base_timeline.h"
#include "hawktracer/callstack_timeline.h"
#include "internal/registry.h"

void
ht_init(int argc, char** argv)
{
    /* For future use */
    HT_UNUSED(argc);
    HT_UNUSED(argv);

    ht_registry_init();

    ht_HT_Event_register_event_klass();
    ht_HT_EventKlassInfoEvent_register_event_klass();
    ht_HT_EventKlassFieldInfoEvent_register_event_klass();
    ht_HT_CallstackBaseEvent_register_event_klass();
    ht_HT_CallstackIntEvent_register_event_klass();
    ht_HT_CallstackStringEvent_register_event_klass();

    /* TODO: should we take some action if register() fails? */
    ht_registry_register_timeline(
                HT_BASE_TIMELINE_IDENTIFIER, sizeof(HT_Timeline), HT_FALSE,
                ht_base_timeline_init, ht_base_timeline_deinit);

    ht_registry_register_timeline(
                "HT_GlobalCallstackTimeline", sizeof(HT_CallstackTimeline), HT_TRUE,
                ht_callstack_timeline_init, ht_callstack_timeline_deinit);
}

void ht_deinit(void)
{
    htregistry_unregister_all_timelines();
    htregistry_unregister_all_event_klasses();
}
