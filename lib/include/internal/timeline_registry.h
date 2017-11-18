#ifndef HAWKTRACER_INTERNAL_TIMELINE_REGISTRY_H
#define HAWKTRACER_INTERNAL_TIMELINE_REGISTRY_H

#include "hawktracer/timeline_listener.h"

#include "hawktracer/timeline.h"
#include "hawktracer/timeline_registry.h"

#include <stdarg.h>

HT_DECLS_BEGIN

typedef struct _HT_TimelineConstructParams _HT_TimelineConstructParams;

struct _HT_TimelineKlass
{
    size_t type_size;
    HT_TimelineListenerContainer* listeners;
    void (*init)(HT_Timeline*, va_list);
    void (*deinit)(HT_Timeline*);
    HT_Boolean thread_safe;
};

HT_TimelineKlass* ht_timeline_registry_find_class(const char* klass_id);

void ht_timeline_registry_unregister_all(void);

HT_DECLS_END

#endif /* HAWKTRACER_INTERNAL_TIMELINE_REGISTRY_H */
