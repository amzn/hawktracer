#include "hawktracer/listeners/tcp_listener.h"
#include "hawktracer/alloc.h"
#include "hawktracer/listener_buffer.h"
#include "hawktracer/registry.h"
#include "internal/listeners/tcp_server.hpp"

#include <new>

struct _HT_TCPListenerPimpl
{
public:
    ~_HT_TCPListenerPimpl();
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
        HT_TCPListenerPimpl* listener = reinterpret_cast<HT_TCPListenerPimpl*>(user_data);
        listener->_last_client_sock_fd = sock_fd;
        ht_registry_push_all_klass_info_events(&listener->_reg_timeline);
    }

    static void _f_flush(void* listener)
    {
        HT_TCPListenerPimpl* tcp_listener = reinterpret_cast<HT_TCPListenerPimpl*>(listener);
        tcp_listener->_flush();
        tcp_listener->_was_flushed = true;
    }

    std::mutex _push_action_mutex;
    HT_ListenerBuffer _buffer;
    HawkTracer::TCPServer _tcp_server;
    HT_Timeline _reg_timeline;
    int _last_client_sock_fd = 0;
    /* TODO: This is just a hack to prevent from sending half-events.
     * We should revisit this for next release */
    bool _was_flushed = false;
};

HT_ErrorCode HT_TCPListenerPimpl::init(int port)
{
    HT_ErrorCode error_code = ht_listener_buffer_init(&_buffer, HT_TCP_LISTENER_BUFFER_SIZE);
    if (error_code != HT_ERR_OK)
    {
        return error_code;
    }
    error_code = ht_timeline_init(&_reg_timeline, 4096, HT_FALSE, HT_TRUE, NULL);
    if (error_code != HT_ERR_OK)
    {
        ht_listener_buffer_deinit(&_buffer);
        return error_code;
    }

    if (_tcp_server.start(port, _client_connected, this))
    {
        /* TODO: handle ht_timeline_register_listener() error */
        error_code = ht_timeline_register_listener(&_reg_timeline, [] (TEventPtr e, size_t c, HT_Boolean, void* ud) {
            HT_TCPListenerPimpl* listener = reinterpret_cast<HT_TCPListenerPimpl*>(ud);
            std::lock_guard<std::mutex> l(listener->_push_action_mutex);
            listener->_tcp_server.write_to_socket(listener->_last_client_sock_fd, (char*)e, c);
        }, this);

        if (error_code == HT_ERR_OK)
        {
            return error_code;
        }
        else
        {
            _tcp_server.stop();
        }
    }
    else
    {
        error_code = HT_ERR_CANT_START_TCP_SERVER;
    }

    ht_listener_buffer_deinit(&_buffer);
    ht_timeline_deinit(&_reg_timeline);

    return error_code;
}

HT_TCPListenerPimpl::~_HT_TCPListenerPimpl()
{
    _flush();
    _tcp_server.stop();
    ht_listener_buffer_deinit(&_buffer);
    ht_timeline_deinit(&_reg_timeline);
}

void HT_TCPListenerPimpl::push_events(TEventPtr events, size_t size, HT_Boolean serialized)
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

HT_ErrorCode ht_tcp_listener_init(HT_TCPListener* listener, int port)
{
    listener->pimpl = (HT_TCPListenerPimpl*) ht_alloc(sizeof(HT_TCPListenerPimpl));
    if (!listener->pimpl)
    {
        return HT_ERR_OUT_OF_MEMORY;
    }

    new(listener->pimpl) HT_TCPListenerPimpl();

    return listener->pimpl->init(port);
}

void ht_tcp_listener_deinit(HT_TCPListener* listener)
{
    listener->pimpl->~HT_TCPListenerPimpl();
    ht_free(listener->pimpl);
}

void ht_tcp_listener_callback(TEventPtr events, size_t size, HT_Boolean serialized, void* user_data)
{
    ((HT_TCPListener*)user_data)->pimpl->push_events(events, size, serialized);
}
