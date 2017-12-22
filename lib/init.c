#include "hawktracer/init.h"
#include "hawktracer/callstack_scoped_tracepoint.h"
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
    ht_HT_StringMappingEvent_register_event_klass();
}

void ht_deinit(void)
{
    ht_registry_deinit();
}
