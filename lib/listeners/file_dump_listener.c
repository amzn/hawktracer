#include "hawktracer/listeners/file_dump_listener.h"
#include "hawktracer/events.h"

#include <stdio.h>
#include <string.h>

#define HT_MIN(a, b) (((a) > (b)) ? (b) : (a))

inline static void
_ht_file_dump_listener_flush(HT_FileDumpListener* listener)
{
    fwrite(listener->buffer, sizeof(HT_Byte), listener->buffer_usage, listener->p_file);
    listener->buffer_usage = 0;
}

inline static void
_ht_file_dump_listener_process_serialized_events(HT_FileDumpListener* listener, TEventPtr events, size_t size)
{
    size_t written = 0;
    while (written < size)
    {
        size_t actual_size = HT_MIN(size, HT_FILE_DUMP_LISTENER_BUFFER_SIZE - listener->buffer_usage);
        memcpy(listener->buffer + listener->buffer_usage, events + written, actual_size);
        written += actual_size;
        listener->buffer_usage+= actual_size;

        if (listener->buffer_usage == HT_FILE_DUMP_LISTENER_BUFFER_SIZE)
        {
            _ht_file_dump_listener_flush(listener);
        }
    }
}

inline static void
_ht_file_dump_listener_process_unserialized_events(HT_FileDumpListener* listener, TEventPtr events, size_t size)
{
    for (size_t i = 0; i < size;)
    {
        HT_Event* event = HT_EVENT(events + i);

        if (HT_EVENT_GET_CLASS(event)->get_size(event) + listener->buffer_usage > HT_FILE_DUMP_LISTENER_BUFFER_SIZE)
        {
            _ht_file_dump_listener_flush(listener);
        }

        listener->buffer_usage += HT_EVENT_GET_CLASS(event)->serialize(event, listener->buffer + listener->buffer_usage);
        i += HT_EVENT_GET_CLASS(event)->type_info->size;
    }
}

HT_Boolean
ht_file_dump_listener_init(HT_FileDumpListener* listener, const char* filename)
{
    listener->buffer_usage = 0;

    listener->p_file = fopen(filename, "wb");
    if (listener->p_file == NULL)
    {
        return HT_FALSE;
    }

    return HT_TRUE;
}

void
ht_file_dump_listener_deinit(HT_FileDumpListener* listener)
{
    if (listener->p_file != NULL)
    {
        _ht_file_dump_listener_flush(listener);
        fclose(listener->p_file);
        listener->p_file = NULL;
    }
}

void
ht_file_dump_listener_callback(TEventPtr events, size_t size, HT_Boolean serialized, void* user_data)
{
    HT_FileDumpListener* listener = (HT_FileDumpListener*)user_data;

    if (serialized)
    {
        _ht_file_dump_listener_process_serialized_events(listener, events, size);
    }
    else
    {
        _ht_file_dump_listener_process_unserialized_events(listener, events, size);
    }
}
