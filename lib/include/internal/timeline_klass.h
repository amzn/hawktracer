#ifndef HAWKTRACER_INTERNAL_TIMELINE_KLASS_H
#define HAWKTRACER_INTERNAL_TIMELINE_KLASS_H

#include "hawktracer/timeline.h"

HT_DECLS_BEGIN

typedef struct _HT_TimelineKlass HT_TimelineKlass;

void ht_timeline_klass_unref(HT_TimelineKlass* klass);

HT_TimelineKlass* ht_timeline_klass_create(
        uint32_t klass_id,
        size_t type_size,
        HT_Boolean shared_listeners,
        void (*init)(HT_Timeline*, va_list),
        void (*deinit)(HT_Timeline*));

uint32_t ht_timeline_klass_get_id(HT_TimelineKlass* klass);

HT_DECLS_END

#endif /* HAWKTRACER_INTERNAL_TIMELINE_KLASS_H */
