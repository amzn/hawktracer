#include "hawktracer/listeners/file_dump_listener.h"

inline static void
_ht_file_dump_listener_flush(void* listener)
{
    HT_FileDumpListener* fd_listener = (HT_FileDumpListener*) listener;

    fwrite(fd_listener->buffer.data, sizeof(HT_Byte), fd_listener->buffer.usage, fd_listener->p_file);
    fd_listener->buffer.usage = 0;
}

HT_Boolean
ht_file_dump_listener_init(HT_FileDumpListener* listener, const char* filename)
{
    listener->p_file = fopen(filename, "wb");
    if (listener->p_file == NULL)
    {
        return HT_FALSE;
    }

    ht_listener_buffer_init(&listener->buffer, HT_FILE_DUMP_LISTENER_BUFFER_SIZE); /* TODO expose to constructor */

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
        ht_listener_buffer_deinit(&listener->buffer);
    }
}

void
ht_file_dump_listener_callback(TEventPtr events, size_t size, HT_Boolean serialized, void* user_data)
{
    /* TODO: this method should be thread-safe (at least optionally) */
    HT_FileDumpListener* listener = (HT_FileDumpListener*)user_data;

    if (serialized)
    {
        ht_listener_buffer_process_serialized_events(&listener->buffer, events, size, _ht_file_dump_listener_flush, listener);
    }
    else
    {
        ht_listener_buffer_process_unserialized_events(&listener->buffer, events, size, _ht_file_dump_listener_flush, listener);
    }
}
