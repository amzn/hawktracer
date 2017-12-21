#ifndef HAWKTRACER_REGISTRY_H
#define HAWKTRACER_REGISTRY_H

#include <hawktracer/timeline.h>

HT_DECLS_BEGIN

HT_Boolean ht_registry_register_event_klass(HT_EventKlass* event_klass);

void ht_registry_push_klass_info_event(HT_Timeline* timeline, HT_EventKlass* klass);

void ht_registry_push_all_klass_info_events(HT_Timeline* timeline);

HT_EventKlass** ht_registry_get_event_klasses(size_t* out_klass_count);

HT_DECLS_END

#endif /* HAWKTRACER_REGISTRY_H */
