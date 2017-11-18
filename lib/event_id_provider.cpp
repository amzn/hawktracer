#include "hawktracer/event_id_provider.h"
#include "hawktracer/alloc.h"

#include <atomic>
#include <new>

struct _HT_EventIdProvider
{
    std::atomic<HT_EventId> current_identifier = ATOMIC_VAR_INIT(0);
};

HT_EventIdProvider*
ht_event_id_provider_get_default(void)
{
    static HT_EventIdProvider provider;

    return &provider;
}

HT_EventIdProvider*
ht_event_id_provider_create(void)
{
    HT_EventIdProvider* provider = HT_CREATE_TYPE(HT_EventIdProvider);
    new (provider) HT_EventIdProvider();
    return provider;
}

void
ht_event_id_provider_destroy(HT_EventIdProvider* provider)
{
    provider->~HT_EventIdProvider();
    ht_free(provider);
}

HT_EventId
ht_event_id_provider_next(HT_EventIdProvider* provider)
{
    return provider->current_identifier++;
}
