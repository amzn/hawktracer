#include "hawktracer/listeners/file_dump_listener.h"
#include "hawktracer/events.h"

#include <stdio.h>

inline static void
_ht_file_dump_listener_flush(HT_FileDumpListener* listener)
{
    fwrite(listener->buffer, sizeof(HT_Byte), listener->buffer_usage, listener->p_file);
    listener->buffer_usage = 0;
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
ht_file_dump_listener_callback(TEventPtr events, size_t event_count, void* user_data)
{
    HT_FileDumpListener* listener = (HT_FileDumpListener*)user_data;

    for (size_t i = 0; i < event_count;)
    {
        HT_Event* event = HT_EVENT(events + i);

        if (HT_EVENT_GET_CLASS(event)->type_info->packed_size + listener->buffer_usage > HT_FILE_DUMP_LISTENER_BUFFER_SIZE)
        {
            _ht_file_dump_listener_flush(listener);
        }

        listener->buffer_usage += HT_EVENT_GET_CLASS(event)->serialize(event, listener->buffer + listener->buffer_usage);
        i += HT_EVENT_GET_CLASS(event)->type_info->size;
    }
}
