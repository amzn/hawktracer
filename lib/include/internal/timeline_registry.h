#ifndef HAWKTRACER_INTERNAL_TIMELINE_REGISTRY_H
#define HAWKTRACER_INTERNAL_TIMELINE_REGISTRY_H

#include "hawktracer/timeline_registry.h"
#include "internal/timeline_klass.h"

HT_DECLS_BEGIN

void ht_timeline_registry_init(void);

HT_TimelineKlass* ht_timeline_registry_find_class(const char* klass_id);

void ht_timeline_registry_unregister_all(void);

HT_DECLS_END

#endif /* HAWKTRACER_INTERNAL_TIMELINE_REGISTRY_H */
