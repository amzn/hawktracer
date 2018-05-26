#include "hawktracer/config.h"
#include "hawktracer/init.h"
#include "hawktracer/scoped_tracepoint.h"
#include "internal/registry.h"
#include "internal/feature.h"

#ifdef HT_USE_PTHREADS
#  include "hawktracer/posix_mapped_tracepoint.h"
#endif

void
ht_init(int argc, char** argv)
{
    /* For future use */
    HT_UNUSED(argc);
    HT_UNUSED(argv);

    ht_registry_init();

    ht_HT_Event_register_event_klass();
    ht_HT_EndiannessInfoEvent_register_event_klass();
    ht_HT_EventKlassInfoEvent_register_event_klass();
    ht_HT_EventKlassFieldInfoEvent_register_event_klass();
    ht_HT_CallstackBaseEvent_register_event_klass();
    ht_HT_CallstackIntEvent_register_event_klass();
    ht_HT_CallstackStringEvent_register_event_klass();
    ht_HT_StringMappingEvent_register_event_klass();
    ht_HT_SystemInfoEvent_register_event_klass();

    ht_feature_register_core_features();

#ifdef HT_USE_PTHREADS
    _ht_posix_mapped_tracepoint_init();
#endif
}


void ht_deinit(void)
{
#ifdef HT_USE_PTHREADS
    _ht_posix_mapped_tracepoint_deinit();
#endif

    ht_registry_deinit();
}
