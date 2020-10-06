#include "hawktracer/listeners/tcp_listener.h"

#ifdef HT_ENABLE_TCP_LISTENER

#include "hawktracer/alloc.h"
#include "hawktracer/timeline_listener.h"

#include "internal/error.h"
#include "internal/listener_buffer.h"
#include "internal/listeners/tcp_server.h"
#include "internal/mutex.h"

#include <assert.h>

struct _HT_TCPListener
{
    HT_Mutex* _push_action_mutex;
    HT_ListenerBuffer _buffer;
    HT_TCPServer* _tcp_server;
    int _last_client_sock_fd;
    /* TODO: This is just a hack to prevent from sending half-events.
     * We should revisit this for next release */
    HT_Boolean _was_flushed;
    HT_TCPListenerOnClientConnectedCallback _client_connected_callback;
    void* _client_connected_user_data;

};

HT_INLINE static HT_Boolean
_ht_tcp_listener_is_stopped(HT_TCPListener* listener)
{
    return listener->_tcp_server == NULL;
}

static void
ht_tcp_listener_f_flush(void* listener, HT_Byte* data, size_t size)
{
    HT_TCPListener* tcp_listener = (HT_TCPListener*)listener;
    ht_tcp_server_write(tcp_listener->_tcp_server, (char*)data, size);
    tcp_listener->_was_flushed = HT_TRUE;
}

static void
ht_tcp_listener_flush(HT_TCPListener* listener)
{
    ht_listener_buffer_flush(&listener->_buffer, ht_tcp_listener_f_flush, listener);
}

static void
ht_tcp_listener_metadata_pusher(TEventPtr e, size_t c, HT_Boolean serialized, void* user_data)
{
    assert(serialized);
    (void) serialized;
    HT_TCPListener* listener = (HT_TCPListener*)user_data;
    ht_mutex_lock(listener->_push_action_mutex);
    ht_tcp_server_write_to_socket(listener->_tcp_server, listener->_last_client_sock_fd, (char*)e, c);
    ht_mutex_unlock(listener->_push_action_mutex);
}

static void
ht_tcp_listener_client_connected(int sock_fd, void* user_data)
{
    HT_TCPListener* listener = (HT_TCPListener*)user_data;
    listener->_last_client_sock_fd = sock_fd;
    ht_timeline_listener_push_metadata(ht_tcp_listener_metadata_pusher, user_data, HT_TRUE);

    if (listener->_client_connected_callback)
    {
        listener->_client_connected_callback(sock_fd, listener->_client_connected_user_data);
    }
}

static HT_ErrorCode
ht_tcp_listener_init(HT_TCPListener* listener, int port, size_t buffer_size)
{
    HT_ErrorCode error_code = ht_listener_buffer_init(&listener->_buffer, buffer_size);
    if (error_code != HT_ERR_OK)
    {
        return error_code;
    }

    listener->_last_client_sock_fd = 0;
    listener->_was_flushed = HT_FALSE;

    // TODO handle if return null
    listener->_push_action_mutex = ht_mutex_create();
    listener->_tcp_server = ht_tcp_server_create();

    if (ht_tcp_server_start(listener->_tcp_server, port, ht_tcp_listener_client_connected, listener))
    {
        return HT_ERR_OK;
    }

    return HT_ERR_CANT_START_TCP_SERVER;
}

static void
ht_tcp_listener_push_events(HT_TCPListener* listener, TEventPtr events, size_t size, HT_Boolean serialized)
{
    ht_mutex_lock(listener->_push_action_mutex);

    if (_ht_tcp_listener_is_stopped(listener))
    {
        goto push_event_done;
    }

    if (!ht_tcp_server_is_running(listener->_tcp_server))
    {
        goto push_event_done;
    }

    if (serialized)
    {
        ht_listener_buffer_process_serialized_events(&listener->_buffer, events, size, ht_tcp_listener_f_flush, listener);
    }
    else
    {
        ht_listener_buffer_process_unserialized_events(&listener->_buffer, events, size, ht_tcp_listener_f_flush, listener);
    }

    if (listener->_was_flushed)
    {
        ht_tcp_listener_flush(listener);
        listener->_was_flushed = HT_FALSE;
    }

push_event_done:
    ht_mutex_unlock(listener->_push_action_mutex);
}

HT_TCPListener*
ht_tcp_listener_create(int port, size_t buffer_size, HT_ErrorCode* out_err)
{
    HT_TCPListener* listener = HT_CREATE_TYPE(HT_TCPListener);
    if (!listener)
    {
        HT_SET_ERROR(out_err, HT_ERR_OUT_OF_MEMORY);
        return NULL;
    }

    HT_ErrorCode error_code = ht_tcp_listener_init(listener, port, buffer_size);
    if (error_code != HT_ERR_OK)
    {
        ht_tcp_listener_destroy(listener);
        listener = NULL;
    }

    HT_SET_ERROR(out_err, error_code);
    return listener;
}

HT_TCPListener*
ht_tcp_listener_register(HT_Timeline* timeline, int port, size_t buffer_size, HT_ErrorCode* out_err)
{
    HT_ErrorCode err = HT_ERR_OK;
    HT_TCPListener* listener = ht_tcp_listener_create(port, buffer_size, &err);

    if (!listener)
    {
        goto register_done;
    }

    err = ht_timeline_register_listener_full(
                timeline,
                ht_tcp_listener_callback,
                listener,
                (HT_DestroyCallback)ht_tcp_listener_destroy);
    if (err != HT_ERR_OK)
    {
        ht_tcp_listener_destroy(listener);
        listener = NULL;
    }

register_done:
    HT_SET_ERROR(out_err, err);
    return listener;
}

void
ht_tcp_listener_set_on_client_connected_callback(
    HT_TCPListener* listener, HT_TCPListenerOnClientConnectedCallback callback, void* user_data)
{
    assert(listener);

    listener->_client_connected_user_data = user_data;
    listener->_client_connected_callback = callback;
}

void
ht_tcp_listener_destroy(HT_TCPListener* listener)
{
    if (!_ht_tcp_listener_is_stopped(listener))
    {
        ht_tcp_listener_stop(listener);
    }
    ht_mutex_destroy(listener->_push_action_mutex);
    ht_free(listener);
}

void
ht_tcp_listener_callback(TEventPtr events, size_t size, HT_Boolean serialized, void* user_data)
{
    ht_tcp_listener_push_events((HT_TCPListener*)user_data, events, size, serialized);
}

void
ht_tcp_listener_stop(HT_TCPListener* listener)
{
    ht_mutex_lock(listener->_push_action_mutex);

    if (!_ht_tcp_listener_is_stopped(listener))
    {
        ht_mutex_unlock(listener->_push_action_mutex);
    }

    ht_tcp_listener_flush(listener);
    ht_tcp_server_stop(listener->_tcp_server);
    ht_listener_buffer_deinit(&listener->_buffer);
    ht_tcp_server_destroy(listener->_tcp_server);
    listener->_tcp_server = NULL;

    ht_mutex_unlock(listener->_push_action_mutex);

    ht_mutex_unlock(listener->_push_action_mutex);
}

#endif /* HT_ENABLE_TCP_LISTENER */
