#include "hawktracer/alloc.h"
#include "hawktracer/events.h"
#include "internal/listener_buffer.h"

#include <string.h>
#include <assert.h>

#define HT_MIN(a, b) (((a) > (b)) ? (b) : (a))
#define HT_IS_BYPASS_MODE(buffer) (buffer->max_size == 0)

HT_ErrorCode
ht_listener_buffer_init(HT_ListenerBuffer* buffer, size_t max_size)
{
    if (max_size == 0)
    {
        buffer->data = NULL;
    }
    else
    {
        buffer->data = (HT_Byte*)ht_alloc(max_size);

        if (buffer->data == NULL)
        {
            return HT_ERR_OUT_OF_MEMORY;
        }
    }

    buffer->max_size = max_size;
    buffer->usage = 0;

    return HT_ERR_OK;
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

    if (HT_IS_BYPASS_MODE(buffer))
    {
        flush_callback(listener, events, size);
        return;
    }

    while (written < size)
    {
        size_t actual_size = HT_MIN(size - written, buffer->max_size - buffer->usage);
        memcpy(buffer->data + buffer->usage, events + written, actual_size);
        written += actual_size;
        buffer->usage+= actual_size;

        if (buffer->usage == buffer->max_size)
        {
            ht_listener_buffer_flush(buffer, flush_callback, listener);
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

    assert(!HT_IS_BYPASS_MODE(buffer));

    for (i = 0; i < size;)
    {
        HT_Event* event = HT_EVENT(events + i);

        if (HT_EVENT_GET_KLASS(event)->get_size(event) + buffer->usage > buffer->max_size)
        {
            ht_listener_buffer_flush(buffer, flush_callback, listener);
        }

        buffer->usage += HT_EVENT_GET_KLASS(event)->serialize(event, buffer->data + buffer->usage);
        i += HT_EVENT_GET_KLASS(event)->type_info->size;
    }
}

void
ht_listener_buffer_flush(HT_ListenerBuffer* buffer,
                        HT_ListenerFlushCallback flush_callback,
                        void* listener)
{
    flush_callback(listener, buffer->data, buffer->usage);
    buffer->usage = 0;
}
