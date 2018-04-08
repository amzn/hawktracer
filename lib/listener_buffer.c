#include "hawktracer/listener_buffer.h"
#include "hawktracer/alloc.h"
#include "hawktracer/events.h"

#include <string.h>

#define HT_MIN(a, b) (((a) > (b)) ? (b) : (a))

void
ht_listener_buffer_init(HT_ListenerBuffer* buffer, size_t max_size)
{
    buffer->data = ht_alloc(max_size);
    buffer->max_size = max_size;
    buffer->usage = 0;
}

void
ht_listener_buffer_deinit(HT_ListenerBuffer* buffer)
{
    ht_free(buffer->data);
}

void
ht_listener_buffer_process_serialized_events(HT_ListenerBuffer* buffer,
                                             TEventPtr events,
                                             size_t size,
                                             HT_ListenerFlushCallback flush_callback,
                                             void* listener)
{
    size_t written = 0;
    while (written < size)
    {
        size_t actual_size = HT_MIN(size - written, buffer->max_size - buffer->usage);
        memcpy(buffer->data + buffer->usage, events + written, actual_size);
        written += actual_size;
        buffer->usage+= actual_size;

        if (buffer->usage == buffer->max_size)
        {
            flush_callback(listener);
            buffer->usage = 0;
        }
    }
}

void
ht_listener_buffer_process_unserialized_events(HT_ListenerBuffer* buffer,
                                               TEventPtr events,
                                               size_t size,
                                               HT_ListenerFlushCallback flush_callback,
                                               void* listener)
{
    size_t i;
    for (i = 0; i < size;)
    {
        HT_Event* event = HT_EVENT(events + i);

        if (HT_EVENT_GET_CLASS(event)->get_size(event) + buffer->usage > buffer->max_size)
        {
            flush_callback(listener);
            buffer->usage = 0;
        }

        buffer->usage += HT_EVENT_GET_CLASS(event)->serialize(event, buffer->data + buffer->usage);
        i += HT_EVENT_GET_CLASS(event)->type_info->size;
    }
}
