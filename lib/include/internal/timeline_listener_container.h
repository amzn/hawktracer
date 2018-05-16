#ifndef HAWKTRACER_INTERNAL_TIMELINE_LISTENER_CONTAINER_H
#define HAWKTRACER_INTERNAL_TIMELINE_LISTENER_CONTAINER_H

#include <hawktracer/bag.h>

HT_DECLS_BEGIN

struct _HT_TimelineListenerContainer
{
    /* TODO single struct with pair? */
    HT_Bag callbacks;
    HT_Bag user_datas;
    struct _HT_Mutex* mutex;
    uint32_t id;
    int refcount; /* TODO atomic */
};

HT_DECLS_END

#endif /* TIMELINE_LISTENER_CONTAINER_H */
