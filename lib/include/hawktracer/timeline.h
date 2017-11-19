#ifndef HAWKTRACER_TIMELINE_H
#define HAWKTRACER_TIMELINE_H

#include <hawktracer/macros.h>
#include <hawktracer/events.h>
#include <hawktracer/timeline_listener.h>
#include <hawktracer/event_id_provider.h>
#include <hawktracer/mutex.h>

#include <stdarg.h>
#include <stddef.h>

HT_DECLS_BEGIN

#define HT_TIMELINE(timeline) ((HT_Timeline*)timeline)

typedef struct _HT_TimelineKlass HT_TimelineKlass;

typedef struct
{
    size_t buffer_capacity;
    size_t buffer_usage;
    uint8_t* buffer;
    HT_Mutex* locking_policy;
    HT_EventIdProvider* id_provider;
    HT_TimelineListenerContainer* listeners;
    HT_TimelineKlass* klass;
} HT_Timeline;

void ht_timeline_register_listener(
        HT_Timeline* timeline,
        HT_TimelineListenerCallback callback,
        void* user_data);
void ht_timeline_unregister_all_listeners(HT_Timeline* timeline);

HT_Timeline* ht_timeline_create(const char* klass_id, ...);

void ht_timeline_destroy(HT_Timeline* timeline);

void ht_timeline_init(HT_Timeline* timeline, va_list);

void ht_timeline_deinit(HT_Timeline* timeline);

void ht_timeline_init_event(HT_Timeline* timeline, HT_Event* event);

void ht_timeline_push_event(HT_Timeline* timeline, HT_Event* event);

void ht_timeline_flush(HT_Timeline* timeline);

HT_DECLS_END

#endif /* HAWKTRACER_TIMELINE_H */
