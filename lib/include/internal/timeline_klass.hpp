#ifndef HAWKTRACER_INTERNAL_TIMELINE_KLASS_HPP
#define HAWKTRACER_INTERNAL_TIMELINE_KLASS_HPP

struct _HT_TimelineKlass
{
    size_t type_size;
    HT_TimelineListenerContainer* listeners;
    void (*init)(HT_Timeline*, va_list);
    void (*deinit)(HT_Timeline*);
    HT_Boolean thread_safe;
};

#endif /* HAWKTRACER_INTERNAL_TIMELINE_KLASS_HPP */
