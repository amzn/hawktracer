#ifndef HAWKTRACER_EVENT_ID_PROVIDER_H
#define HAWKTRACER_EVENT_ID_PROVIDER_H

#include <hawktracer/base_types.h>

HT_DECLS_BEGIN

/**
 * HT_EventIdProvider is a sequence number generator. It starts
 * the sequence from 0. Every next call of ht_event_id_provider_next()
 * increments the identifier.
 */
typedef struct _HT_EventIdProvider HT_EventIdProvider;

/**
 * Gets default provider.
 *
 * Default ID provider is currently used for all the timelines.
 *
 * @return a pointer to the default ID provider.
 */
HT_API HT_EventIdProvider* ht_event_id_provider_get_default(void);

/**
 * Creates a new instance of an ID provider.
 *
 * @return a pointer to the new ID provider.
 */
HT_API HT_EventIdProvider* ht_event_id_provider_create(void);

/**
 * Destroys a provider.
 * @param provider a provider to destroy.
 */
HT_API void ht_event_id_provider_destroy(HT_EventIdProvider* provider);

/**
 * Gets next identifier.
 *
 * @param provider a provider.
 *
 * @return next identifier from the @p provider.
 */
HT_API HT_EventId ht_event_id_provider_next(HT_EventIdProvider* provider);

HT_DECLS_END

#endif /* HAWKTRACER_EVENT_ID_PROVIDER_H */
