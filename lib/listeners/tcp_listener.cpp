#include "hawktracer/listeners/tcp_listener.h"
#include "hawktracer/alloc.h"
#include "hawktracer/registry.h"
#include "internal/listener_buffer.h"
#include "internal/listeners/tcp_server.hpp"

#include <new>

inline static void
_set_error(HT_ErrorCode error_code, HT_ErrorCode* out)
{
    if (out)
    {
        *out = error_code;
    }
}

struct _HT_TCPListener
{
public:
    ~_HT_TCPListener();
    HT_ErrorCode init(int port);
    void push_events(TEventPtr events, size_t size, HT_Boolean serialized);

private:
    void _flush()
    {
        _tcp_server.write((char*)_buffer.data, _buffer.usage);
        _buffer.usage = 0;
        _was_flushed = true;
    }

    static void _client_connected(int sock_fd, void* user_data)
    {
        HT_TCPListener* listener = reinterpret_cast<HT_TCPListener*>(user_data);
        listener->_last_client_sock_fd = sock_fd;
        ht_registry_push_registry_klasses_to_listener(ht_tcp_listener_callback, user_data, HT_TRUE);
    }

    static void _f_flush(void* listener)
    {
        HT_TCPListener* tcp_listener = reinterpret_cast<HT_TCPListener*>(listener);
        tcp_listener->_flush();
        tcp_listener->_was_flushed = true;
    }

    std::mutex _push_action_mutex;
    HT_ListenerBuffer _buffer;
    HawkTracer::TCPServer _tcp_server;
    int _last_client_sock_fd = 0;
    /* TODO: This is just a hack to prevent from sending half-events.
     * We should revisit this for next release */
    bool _was_flushed = false;
};

HT_ErrorCode HT_TCPListener::init(int port)
{
    HT_ErrorCode error_code = ht_listener_buffer_init(&_buffer, HT_TCP_LISTENER_BUFFER_SIZE);
    if (error_code != HT_ERR_OK)
    {
        return error_code;
    }

    if (_tcp_server.start(port, _client_connected, this))
    {
        return HT_ERR_OK;
    }

    ht_listener_buffer_deinit(&_buffer);

    return HT_ERR_CANT_START_TCP_SERVER;
}

_HT_TCPListener::~_HT_TCPListener()
{
    _flush();
    _tcp_server.stop();
    ht_listener_buffer_deinit(&_buffer);
}

void _HT_TCPListener::push_events(TEventPtr events, size_t size, HT_Boolean serialized)
{
    if (!_tcp_server.is_running())
    {
        return;
    }

    std::lock_guard<std::mutex> l(_push_action_mutex);

    if (serialized)
    {
        ht_listener_buffer_process_serialized_events(&_buffer, events, size, _f_flush, this);
    }
    else
    {
        ht_listener_buffer_process_unserialized_events(&_buffer, events, size, _f_flush, this);
    }

    if (_was_flushed)
    {
        _flush();
        _was_flushed = false;
    }
}

HT_TCPListener* ht_tcp_listener_create(int port, HT_ErrorCode* out_err)
{
    HT_TCPListener* listener = HT_CREATE_TYPE(HT_TCPListener);
    if (!listener)
    {
        _set_error(HT_ERR_OUT_OF_MEMORY, out_err);
        return nullptr;
    }

    new(listener) HT_TCPListener();

    HT_ErrorCode error_code = listener->init(port);
    if (error_code != HT_ERR_OK)
    {
        ht_tcp_listener_destroy(listener);
        listener = nullptr;
    }

    _set_error(error_code, out_err);
    return listener;
}

void ht_tcp_listener_destroy(HT_TCPListener* listener)
{
    listener->~HT_TCPListener();
    ht_free(listener);
}

void ht_tcp_listener_callback(TEventPtr events, size_t size, HT_Boolean serialized, void* user_data)
{
    ((HT_TCPListener*)user_data)->push_events(events, size, serialized);
}
