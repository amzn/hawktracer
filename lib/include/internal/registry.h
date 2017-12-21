#ifndef HAWKTRACER_INTERNAL_REGISTRY_H
#define HAWKTRACER_INTERNAL_REGISTRY_H

#include "hawktracer/registry.h"

HT_DECLS_BEGIN

void ht_registry_init(void);

void htregistry_unregister_all_event_klasses(void);

HT_DECLS_END

#endif /* HAWKTRACER_INTERNAL_REGISTRY_H */
