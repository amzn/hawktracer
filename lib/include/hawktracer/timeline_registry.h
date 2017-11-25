#ifndef HAWKTRACER_TIMELINE_REGISTRY_H
#define HAWKTRACER_TIMELINE_REGISTRY_H

#include <hawktracer/timeline.h>

HT_DECLS_BEGIN

void ht_timeline_registry_register(
        const char* klass_id,
        size_t type_size,
        HT_Boolean thread_safe,
        HT_Boolean shared_listeners,
        void (*init)(HT_Timeline*, va_list),
        void (*deinit)(HT_Timeline*));

HT_DECLS_END

#endif /* HAWKTRACER_TIMELINE_REGISTRY_H */
