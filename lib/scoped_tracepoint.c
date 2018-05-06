#include "hawktracer/scoped_tracepoint.h"

#ifdef __GNUC__

void _ht_callstack_timeline_scoped_cleanup(HT_Timeline** timeline)
{
    ht_feature_callstack_stop(*timeline);
}

HT_Timeline* _ht_callstack_timeline_int_start_and_ret(HT_Timeline* t, HT_CallstackEventLabel l)
{
    ht_feature_callstack_start_int(t, l);
    return t;
}

HT_Timeline* _ht_callstack_timeline_string_start_and_ret(HT_Timeline* t, const char* l)
{
    ht_feature_callstack_start_string(t, l);
    return t;
}

#endif /* __GNUC__ */
