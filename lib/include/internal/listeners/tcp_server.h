#ifndef HAWKTRACER_INTERNAL_LISTENERS_TCP_SERVER_H
#define HAWKTRACER_INTERNAL_LISTENERS_TCP_SERVER_H

#include "hawktracer/base_types.h"

HT_DECLS_BEGIN

typedef struct _HT_TCPServer HT_TCPServer;
typedef void(*OnClientConnected)(int, void*);

HT_TCPServer* ht_tcp_server_create(void);

void ht_tcp_server_destroy(HT_TCPServer* server);

void ht_tcp_server_write(HT_TCPServer* server, char* buffer, size_t size);

HT_Boolean ht_tcp_server_start(HT_TCPServer* server, int port, OnClientConnected client_connected_cb, void* user_data);

void ht_tcp_server_stop(HT_TCPServer* server);

HT_Boolean ht_tcp_server_is_running(const HT_TCPServer* server);

HT_Boolean ht_tcp_server_write_to_socket(HT_TCPServer* server, int sock_fd, char* buffer, size_t size);

HT_DECLS_END

#endif /* HAWKTRACER_INTERNAL_LISTENERS_TCP_SERVER_H */
