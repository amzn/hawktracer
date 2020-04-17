#include "hawktracer/ht_config.h"
#include "hawktracer/init.h"
#include "hawktracer/scoped_tracepoint.h"
#include "internal/registry.h"
#include "internal/feature.h"
#include "internal/command_line_parser.h"

#include "hawktracer/posix_mapped_tracepoint.h"


static int _ht_init_counter = 0;

void
ht_init(int argc, char** argv)
{
    ht_command_line_parse_args(argc, argv);

    ht_registry_init();

    HT_REGISTER_EVENT_KLASS(HT_EndiannessInfoEvent);
    HT_REGISTER_EVENT_KLASS(HT_Event);
    HT_REGISTER_EVENT_KLASS(HT_EventKlassInfoEvent);
    HT_REGISTER_EVENT_KLASS(HT_EventKlassFieldInfoEvent);
    HT_REGISTER_EVENT_KLASS(HT_CallstackBaseEvent);
    HT_REGISTER_EVENT_KLASS(HT_CallstackIntEvent);
    HT_REGISTER_EVENT_KLASS(HT_CallstackStringEvent);
    HT_REGISTER_EVENT_KLASS(HT_StringMappingEvent);
    HT_REGISTER_EVENT_KLASS(HT_SystemInfoEvent);

    ht_feature_register_core_features();

#ifdef HT_USE_PTHREADS
    _ht_posix_mapped_tracepoint_init();
#endif

    _ht_init_counter++;
}

HT_Boolean
ht_is_initialized(void)
{
    return _ht_init_counter > 0;
}

void
ht_deinit(void)
{
    if (_ht_init_counter > 0 && --_ht_init_counter == 0)
    {
#ifdef HT_USE_PTHREADS
        _ht_posix_mapped_tracepoint_deinit();
#endif

        ht_registry_deinit();
    }
}
