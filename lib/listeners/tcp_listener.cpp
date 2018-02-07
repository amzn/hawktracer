#include "hawktracer/listeners/tcp_listener.h"
#include "hawktracer/alloc.h"
#include "hawktracer/listener_buffer.h"
#include "hawktracer/registry.h"
#include "internal/listeners/tcp_server.hpp"

#include <new>

struct _HT_TCPListenerPimpl
{
public:
    HT_Boolean init(int port);
    ~_HT_TCPListenerPimpl();

    void push_events(TEventPtr events, size_t size, HT_Boolean serialized);

private:
    void _flush()
    {
        _tcp_server.write((char*)_buffer.data, _buffer.usage);
        _buffer.usage = 0;
    }

    static void _client_connected(int sock_fd, void* user_data)
    {
        HT_TCPListenerPimpl* listener = reinterpret_cast<HT_TCPListenerPimpl*>(user_data);
        listener->_last_client_sock_fd = sock_fd;
        ht_registry_push_all_klass_info_events(&listener->_reg_timeline);
    }

    static void _f_flush(void* listener)
    {
        reinterpret_cast<HT_TCPListenerPimpl*>(listener)->_flush();
    }

    std::mutex _push_action_mutex;
    HT_ListenerBuffer _buffer;
    HawkTracer::TCPServer _tcp_server;
    HT_Timeline _reg_timeline;
    int _last_client_sock_fd = 0;
};

HT_Boolean HT_TCPListenerPimpl::init(int port)
{
    if (_tcp_server.start(port, _client_connected, this))
    {
        ht_listener_buffer_init(&_buffer, HT_TCP_LISTENER_BUFFER_SIZE);

        ht_timeline_init(&_reg_timeline, 4096, HT_FALSE, HT_TRUE, NULL);
        ht_timeline_register_listener(&_reg_timeline, [] (TEventPtr e, size_t c, HT_Boolean, void* ud) {
            HT_TCPListenerPimpl* listener = reinterpret_cast<HT_TCPListenerPimpl*>(ud);
            listener->_tcp_server.write_to_socket(listener->_last_client_sock_fd, (char*)e, c);
        }, this);

        return HT_TRUE;
    }

    return HT_FALSE;
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
}

HT_Boolean ht_tcp_listener_init(HT_TCPListener* listener, int port)
{
    listener->pimpl = (HT_TCPListenerPimpl*) ht_alloc(sizeof(HT_TCPListenerPimpl));
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
