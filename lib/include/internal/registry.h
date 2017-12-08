#ifndef HAWKTRACER_INTERNAL_REGISTRY_H
#define HAWKTRACER_INTERNAL_REGISTRY_H

#include "hawktracer/registry.h"
#include "internal/timeline_klass.h"

HT_DECLS_BEGIN

void ht_registry_init(void);

HT_TimelineKlass* ht_registry_find_timeline_class(const char* klass_id);

void htregistry_unregister_all_timelines(void);

void htregistry_unregister_all_event_klasses(void);

HT_DECLS_END

#endif /* HAWKTRACER_INTERNAL_REGISTRY_H */
