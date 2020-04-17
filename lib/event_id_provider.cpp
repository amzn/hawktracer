#include "hawktracer/event_id_provider.h"
#include "hawktracer/alloc.h"

#if defined(HT_ENABLE_THREADS) && defined(HT_CPP11)
#include <atomic>
typedef std::atomic<HT_EventId> HT_AtomicEventId;
#else
typedef volatile HT_EventId HT_AtomicEventId;
#endif

struct _HT_EventIdProvider
{
    HT_AtomicEventId current_identifier;
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
    provider->current_identifier = 0;
    return provider;
}

void
ht_event_id_provider_destroy(HT_EventIdProvider* provider)
{
    ht_free(provider);
}

HT_EventId
ht_event_id_provider_next(HT_EventIdProvider* provider)
{
#if defined(HT_CPP11) || !defined(HT_ENABLE_THREADS)
    return provider->current_identifier++;
#elif defined(__GNUC__)
#  if !defined(__GNUC_PREREQ) && defined(__GNUC_MINOR__)
#    define __GNUC_PREREQ(maj,min) ((__GNUC__ << 16) + __GNUC_MINOR__ >= ((maj) << 16) + (min))
#  endif
#  if defined(__GNUC_PREREQ) && __GNUC_PREREQ(4, 7)
    return __atomic_add_fetch(&provider->current_identifier, 1, __ATOMIC_SEQ_CST) - 1;
#  elif defined(__GNUC_PREREQ) && __GNUC_PREREQ(4, 1)
    return __sync_fetch_and_add(&provider->current_identifier, 1);
#  endif
#else
#  error "Atomic Fetch&Add is not supported"
#endif
}
