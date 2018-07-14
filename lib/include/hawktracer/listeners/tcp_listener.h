#ifndef HAWKTRACER_LISTENERS_TCP_LISTENER_H
#define HAWKTRACER_LISTENERS_TCP_LISTENER_H

#include <hawktracer/base_types.h>

#include <stddef.h>

HT_DECLS_BEGIN

typedef struct _HT_TCPListener HT_TCPListener;

HT_API HT_TCPListener* ht_tcp_listener_create(int port, size_t buffer_size, HT_ErrorCode* out_err);

HT_API void ht_tcp_listener_destroy(HT_TCPListener* listener);

HT_API void ht_tcp_listener_callback(TEventPtr events, size_t size, HT_Boolean serialized, void* user_data);

HT_DECLS_END

#endif /* HAWKTRACER_LISTENERS_TCP_LISTENER_H */
