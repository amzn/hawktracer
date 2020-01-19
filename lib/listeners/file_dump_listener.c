#include "hawktracer/listeners/file_dump_listener.h"
#include "hawktracer/alloc.h"
#include "hawktracer/timeline_listener.h"

#include "internal/error.h"
#include "internal/listener_buffer.h"
#include "internal/mutex.h"

struct _HT_FileDumpListener
{
    HT_ListenerBuffer buffer;
    FILE* p_file;
    HT_Mutex* mtx;
};

HT_INLINE static HT_Boolean
_ht_file_dump_listener_is_stopped(HT_FileDumpListener* listener)
{
    return listener->p_file == NULL;
}

HT_INLINE static void
_ht_file_dump_listener_flush(void* listener, HT_Byte* data, size_t size)
{
    HT_FileDumpListener* fd_listener = (HT_FileDumpListener*) listener;

    fwrite(data, sizeof(HT_Byte), size, fd_listener->p_file);
}

HT_FileDumpListener*
ht_file_dump_listener_create(const char* filename, size_t buffer_size, HT_ErrorCode* out_err)
{
    HT_ErrorCode error_code = HT_ERR_OK;
    HT_FileDumpListener* listener = HT_CREATE_TYPE(HT_FileDumpListener);

    if (listener == NULL)
    {
        error_code = HT_ERR_OUT_OF_MEMORY;
        goto done;
    }

    listener->p_file = fopen(filename, "wb");
    if (listener->p_file == NULL)
    {
        error_code = HT_ERR_CANT_OPEN_FILE;
        goto error_open_file;
    }

    listener->mtx = ht_mutex_create();
    if (listener->mtx == NULL)
    {
        error_code = HT_ERR_OUT_OF_MEMORY;
        goto error_create_mutex;
    }

    error_code = ht_listener_buffer_init(&listener->buffer, buffer_size);
    if (error_code != HT_ERR_OK)
    {
        ht_mutex_destroy(listener->mtx);
    }
    else
    {
        ht_timeline_listener_push_metadata(ht_file_dump_listener_callback, listener, HT_TRUE);
        goto done;
    }

error_create_mutex:
    fclose(listener->p_file);

error_open_file:
    ht_free(listener);
    listener = NULL;

done:
    HT_SET_ERROR(out_err, error_code);

    return listener;
}

void
ht_file_dump_listener_destroy(HT_FileDumpListener* listener)
{
    if (listener == NULL)
    {
        return;
    }

    if (!_ht_file_dump_listener_is_stopped(listener))
    {
        ht_file_dump_listener_stop(listener);
    }

    ht_mutex_destroy(listener->mtx);
    ht_free(listener);
}

HT_ErrorCode
ht_file_dump_listener_flush(HT_FileDumpListener* listener, HT_Boolean flush_stream)
{
    HT_ErrorCode ret = HT_ERR_OK;

    ht_mutex_lock(listener->mtx);

    if (_ht_file_dump_listener_is_stopped(listener))
    {
        ht_mutex_unlock(listener->mtx);
        return ret;
    }

    ht_listener_buffer_flush(&listener->buffer, _ht_file_dump_listener_flush, listener);

    if (flush_stream == HT_TRUE)
    {
        if (fflush(listener->p_file) != 0)
        {
            ret = HT_ERR_UNKNOWN;
        }
    }

    ht_mutex_unlock(listener->mtx);

    return ret;
}

void
ht_file_dump_listener_callback(TEventPtr events, size_t size, HT_Boolean serialized, void* user_data)
{
    HT_FileDumpListener* listener = (HT_FileDumpListener*)user_data;

    ht_mutex_lock(listener->mtx);

    if (_ht_file_dump_listener_is_stopped(listener))
    {
        ht_mutex_unlock(listener->mtx);
        return;
    }

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

void
ht_file_dump_listener_stop(HT_FileDumpListener* listener)
{
    ht_mutex_lock(listener->mtx);

    if (_ht_file_dump_listener_is_stopped(listener))
    {
        ht_mutex_unlock(listener->mtx);
        return;
    }

    ht_listener_buffer_flush(&listener->buffer, _ht_file_dump_listener_flush, listener);
    ht_listener_buffer_deinit(&listener->buffer);
    fclose(listener->p_file);
    listener->p_file = NULL;

    ht_mutex_unlock(listener->mtx);
}

HT_FileDumpListener*
ht_file_dump_listener_register(
        HT_Timeline* timeline, const char* filename, size_t buffer_size, HT_ErrorCode *out_err)
{
    HT_ErrorCode err = HT_ERR_OK;
    HT_FileDumpListener* listener = ht_file_dump_listener_create(filename, buffer_size, &err);

    if (!listener)
    {
        goto register_done;
    }

    err = ht_timeline_register_listener_full(
                timeline,
                ht_file_dump_listener_callback,
                listener,
                (HT_DestroyCallback)ht_file_dump_listener_destroy);
    if (err != HT_ERR_OK)
    {
        ht_file_dump_listener_destroy(listener);
        listener = NULL;
    }

register_done:
    HT_SET_ERROR(out_err, err);
    return listener;
}
