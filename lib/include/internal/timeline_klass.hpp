#ifndef HAWKTRACER_INTERNAL_TIMELINE_KLASS_HPP
#define HAWKTRACER_INTERNAL_TIMELINE_KLASS_HPP

#include "internal/timeline_klass.h"
#include <atomic>

struct _HT_TimelineKlass
{
    size_t type_size;
    HT_TimelineListenerContainer* listeners;
    void (*init)(HT_BaseTimeline*, va_list);
    void (*deinit)(HT_BaseTimeline*);
    std::atomic_int_fast32_t refcount;
    uint32_t id;
};

#endif /* HAWKTRACER_INTERNAL_TIMELINE_KLASS_HPP */
