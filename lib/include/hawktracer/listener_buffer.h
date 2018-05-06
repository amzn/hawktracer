#ifndef HAWKTRACER_LISTENER_BUFFER_H
#define HAWKTRACER_LISTENER_BUFFER_H

#include <hawktracer/base_types.h>

#include <stddef.h>

HT_DECLS_BEGIN

typedef struct
{
    HT_Byte* data;
    size_t max_size;
    size_t usage;
} HT_ListenerBuffer;

typedef void(*HT_ListenerFlushCallback)(void*);

HT_API HT_ErrorCode ht_listener_buffer_init(HT_ListenerBuffer* buffer, size_t max_size);

HT_API void ht_listener_buffer_deinit(HT_ListenerBuffer* buffer);

HT_API void ht_listener_buffer_process_serialized_events(HT_ListenerBuffer* buffer,
                                                         TEventPtr events,
                                                         size_t size,
                                                         HT_ListenerFlushCallback flush_callback,
                                                         void* listener);

HT_API void ht_listener_buffer_process_unserialized_events(HT_ListenerBuffer* buffer,
                                                           TEventPtr events,
                                                           size_t size,
                                                           HT_ListenerFlushCallback flush_callback,
                                                           void* listener);

HT_DECLS_END

#endif /* HAWKTRACER_LISTENER_BUFFER_H */
