#ifndef HAWKTRACER_LISTENERS_TCP_LISTENER_H
#define HAWKTRACER_LISTENERS_TCP_LISTENER_H

#include <hawktracer/ht_config.h>

#ifdef HT_ENABLE_TCP_LISTENER

#include <hawktracer/timeline.h>

#include <stddef.h>

HT_DECLS_BEGIN

typedef struct _HT_TCPListener HT_TCPListener;

/**
 * Type of callback being called when a new client is connected to the listener.
 *
 * @param sock_fd file descriptor of the connected client.
 * @param user_data custom user value provided as parameter in ht_tcp_listener_set_on_client_connected_callback().
 */
typedef void(*HT_TCPListenerOnClientConnectedCallback)(int sock_fd, void* user_data);

/**
 * Creates a tcp listener and registers it to a timeline.
 *
 * This is a helper function that wrapps ht_tcp_listener_create() and
 * ht_timeline_register_listener_full(). The user can achieve the same result
 * by explicitly creating a listener and registering it to a timeline:
 *
 * @code
 * HT_TCPListener* listener = ht_tcp_listener_create(9876, 2048, &error_code);
 * ht_timeline_register_listener_full(
 *             timeline,
 *             ht_tcp_listener_callback,
 *             listener,
 *             (HT_DestroyCallback)ht_tcp_listener_destroy);
 * @endcode
 *
 * Please note the example above doesn't implement error handling, which would make the code
 * even more complicated.
 *
 * @param timeline the timeline where the listener will be attached to.
 * @param port the port of the TCP server.
 * @param buffer_size a size of the internal buffer.
 * @param out_err a pointer to an error code variable where the error will be stored if the operation fails.
 *
 * @return a pointer to a new instance of the listener.
 */
HT_API HT_TCPListener* ht_tcp_listener_register(
        HT_Timeline* timeline, int port, size_t buffer_size, HT_ErrorCode* out_err);

HT_API HT_TCPListener* ht_tcp_listener_create(int port, size_t buffer_size, HT_ErrorCode* out_err);

/**
 * Sets a callback which gets called when new client is connected to TCP server.
 *
 * @param listener the listener.
 * @param callback a callback.
 * @param user_data custom user value which will be passed to the callback.
 */
HT_API void ht_tcp_listener_set_on_client_connected_callback(
        HT_TCPListener* listener,
        HT_TCPListenerOnClientConnectedCallback callback,
        void* user_data);

HT_API void ht_tcp_listener_destroy(HT_TCPListener* listener);

HT_API void ht_tcp_listener_callback(TEventPtr events, size_t size, HT_Boolean serialized, void* user_data);

/**
 * Stops listening to new events.
 *
 * The function is very similar to ht_tcp_listener_destroy() except it does not
 * release the memory allocated for @a listener object.
 * After stopping the listener, there's no way to resume it. User needs to
 * create another listener and register it for the timeline instead.
 *
 * @param listener the listener.
 */
HT_API void ht_tcp_listener_stop(HT_TCPListener* listener);

HT_DECLS_END

#endif /* HT_ENABLE_TCP_LISTENER */

#endif /* HAWKTRACER_LISTENERS_TCP_LISTENER_H */
