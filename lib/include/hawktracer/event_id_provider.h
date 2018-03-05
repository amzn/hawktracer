#ifndef HAWKTRACER_EVENT_ID_PROVIDER_H
#define HAWKTRACER_EVENT_ID_PROVIDER_H

#include <hawktracer/base_types.h>

HT_DECLS_BEGIN

typedef struct _HT_EventIdProvider HT_EventIdProvider;

HT_API HT_EventIdProvider* ht_event_id_provider_get_default(void);

HT_API HT_EventIdProvider* ht_event_id_provider_create(void);

HT_API void ht_event_id_provider_destroy(HT_EventIdProvider* provider);

HT_API HT_EventId ht_event_id_provider_next(HT_EventIdProvider* provider);

HT_DECLS_END

#endif /* HAWKTRACER_EVENT_ID_PROVIDER_H */
