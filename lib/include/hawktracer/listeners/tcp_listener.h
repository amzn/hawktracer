#ifndef HAWKTRACER_LISTENERS_TCP_LISTENER_H
#define HAWKTRACER_LISTENERS_TCP_LISTENER_H

#include <hawktracer/base_types.h>

#include <stddef.h>

HT_DECLS_BEGIN

struct _HT_TCPListenerPimpl;

typedef struct _HT_TCPListenerPimpl HT_TCPListenerPimpl;

#define HT_TCP_LISTENER_BUFFER_SIZE 4096 /* TODO make configurable */

typedef struct
{
    HT_TCPListenerPimpl *pimpl;
} HT_TCPListener;

HT_API HT_ErrorCode ht_tcp_listener_init(HT_TCPListener* listener, int port);

HT_API void ht_tcp_listener_deinit(HT_TCPListener* listener);

HT_API void ht_tcp_listener_callback(TEventPtr events, size_t size, HT_Boolean serialized, void* user_data);

HT_DECLS_END

#endif /* HAWKTRACER_LISTENERS_TCP_LISTENER_H */
