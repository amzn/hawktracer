#include "hawktracer/listeners/file_dump_listener.h"

#include "internal/mutex.h"

inline static void
_ht_file_dump_listener_flush(void* listener)
{
    HT_FileDumpListener* fd_listener = (HT_FileDumpListener*) listener;

    fwrite(fd_listener->buffer.data, sizeof(HT_Byte), fd_listener->buffer.usage, fd_listener->p_file);
    fd_listener->buffer.usage = 0;
}

HT_ErrorCode
ht_file_dump_listener_init(HT_FileDumpListener* listener, const char* filename, size_t buffer_size)
{
    HT_ErrorCode error_code = HT_ERR_OK;

    listener->p_file = fopen(filename, "wb");
    if (listener->p_file == NULL)
    {
        error_code = HT_ERR_CANT_OPEN_FILE;
        goto done;
    }

    listener->mtx = ht_mutex_create();
    if (listener->mtx == NULL)
    {
        error_code = HT_ERR_OUT_OF_MEMORY;
        goto error_create_mutex;
    }

    error_code = ht_listener_buffer_init(&listener->buffer, buffer_size);
    if (error_code == HT_ERR_OK)
    {
        goto done;
    }

    ht_mutex_destroy(listener->mtx);

error_create_mutex:
    fclose(listener->p_file);

done:
    return error_code;
}

void
ht_file_dump_listener_deinit(HT_FileDumpListener* listener)
{
    if (listener->p_file != NULL)
    {
        ht_mutex_lock(listener->mtx);
        _ht_file_dump_listener_flush(listener);
        ht_listener_buffer_deinit(&listener->buffer);
        fclose(listener->p_file);
        listener->p_file = NULL;
        ht_mutex_unlock(listener->mtx);
        ht_mutex_destroy(listener->mtx);
    }
}

void
ht_file_dump_listener_callback(TEventPtr events, size_t size, HT_Boolean serialized, void* user_data)
{
    HT_FileDumpListener* listener = (HT_FileDumpListener*)user_data;

    ht_mutex_lock(listener->mtx);

    if (serialized)
    {
        ht_listener_buffer_process_serialized_events(&listener->buffer, events, size, _ht_file_dump_listener_flush, listener);
    }
    else
    {
        ht_listener_buffer_process_unserialized_events(&listener->buffer, events, size, _ht_file_dump_listener_flush, listener);
    }

    ht_mutex_unlock(listener->mtx);
}
