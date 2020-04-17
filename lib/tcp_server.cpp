#include "hawktracer/ht_config.h"

#ifdef HT_ENABLE_TCP_LISTENER

#include "internal/listeners/tcp_server.h"
#include "internal/bag.h"
#include "internal/mutex.h"
#include "hawktracer/alloc.h"
#include "hawktracer/ht_config.h"

#if defined(HT_THREAD_IMPL_CPP11) || defined(HT_THREAD_IMPL_WIN32) || defined(HT_THREAD_IMPL_POSIX)
#  define HT_THREAD_FORCE_SELECTED
#endif

#if !defined(HT_THREAD_FORCE_SELECTED) && defined(HT_CPP11)
#  include <thread>
#  define HT_THREAD_IMPL_CPP11
#elif !defined(HT_THREAD_FORCE_SELECTED) && defined(_WIN32)
#  include <windows.h>
#  define HT_THREAD_IMPL_WIN32
#elif !defined(HT_THREAD_FORCE_SELECTED) && defined(HT_HAVE_UNISTD_H)
#  include <unistd.h>
#  ifdef _POSIX_VERSION
#    include <pthread.h>
#    define HT_THREAD_IMPL_POSIX
#  endif
#endif

#ifdef HT_THREAD_IMPL_CPP11
#  include <thread>
#elif defined(HT_THREAD_IMPL_WIN32)
#  include <windows.h>
#elif defined(HT_THREAD_IMPL_POSIX)
#  include <pthread.h>
#endif

#ifdef HT_THREAD_IMPL_WIN32
typedef DWORD(*ht_thread_callback_t)(void*);
#else
typedef void*(*ht_thread_callback_t)(void*);
#endif

typedef struct
{
#ifdef HT_THREAD_IMPL_CPP11
    std::thread th;
#elif defined(HT_THREAD_IMPL_POSIX)
    pthread_t th;
#elif defined(HT_THREAD_IMPL_WIN32)
    HANDLE th;
#endif
} HT_Thread;

static HT_Thread*
ht_thread_create(ht_thread_callback_t callback, void* user_data)
{
    HT_Thread* th = HT_CREATE_TYPE(HT_Thread);

#ifdef HT_THREAD_IMPL_CPP11
    new(&th->th) HT_Thread();
    th->th = std::thread(callback, user_data);
#elif defined(HT_THREAD_IMPL_POSIX)
    pthread_create(&th->th, NULL, callback, user_data);
#elif defined(HT_THREAD_IMPL_WIN32)
    th->th = CreateThread(NULL, 0, callback, user_data, 0, NULL);
#endif

    return th;
}

static void
ht_thread_join(HT_Thread* th)
{
#ifdef HT_THREAD_IMPL_CPP11
    if (th->th.joinable())
    {
        th->th.join();
    }
#elif defined(HT_THREAD_IMPL_POSIX)
    pthread_join(th->th, NULL);
#elif defined(HT_THREAD_IMPL_WIN32)
    WaitForSingleObject(th->th, INFINITE);
#endif
}

static void
ht_thread_destroy(HT_Thread* th)
{
    ht_thread_join(th);
#ifdef HT_THREAD_IMPL_CPP11
    th->th.~thread();
#elif defined(HT_THREAD_IMPL_WIN32)
    CloseHandle(th->th);
#endif

    ht_free(th);
}

#ifdef _WIN32
#include <WinSock2.h>
#pragma comment(lib, "Ws2_32.lib")
#else
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
#endif

#include <string.h>

static void *_ht_tcp_server_run(void *user_data);

struct _HT_TCPServer
{
    HT_Thread* accept_client_thread;
    HT_BagInt client_sock_fd;
    HT_Mutex* client_mutex;
    int server_sock_fd;

     OnClientConnected client_connected_cb;
     void* client_connected_ud;
};

HT_TCPServer*
ht_tcp_server_create(void)
{
    HT_TCPServer* server = HT_CREATE_TYPE(HT_TCPServer);

    ht_bag_int_init(&server->client_sock_fd, 8);
    server->accept_client_thread = NULL;
    server->client_mutex = ht_mutex_create();
    server->server_sock_fd = -1;
    server->client_connected_cb = NULL;
    server->client_connected_ud = NULL;

    return server;
}

void
ht_tcp_server_destroy(HT_TCPServer* server)
{
    ht_tcp_server_stop(server);
    ht_bag_int_deinit(&server->client_sock_fd);
    ht_mutex_destroy(server->client_mutex);
    ht_free(server);
}

HT_Boolean
ht_tcp_server_start(HT_TCPServer* server, int port, OnClientConnected client_connected_cb, void* user_data)
{
    if (ht_tcp_server_is_running(server))
    {
        ht_tcp_server_stop(server);
    }
#ifdef _WIN32
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
    {
        return HT_FALSE;
    }
#endif
    server->server_sock_fd = socket(AF_INET, SOCK_STREAM, 0);

    if (server->server_sock_fd < 0)
    {
        return HT_FALSE;
    }

#ifndef _WIN32
    int optval = 1;
    if (setsockopt(server->server_sock_fd, SOL_SOCKET, SO_REUSEADDR, (const void *)&optval , sizeof(int)) != 0)
    {
        ht_tcp_server_stop(server);
        return HT_FALSE;
    }
#endif

    struct sockaddr_in serveraddr;
    memset((char *) &serveraddr, 0, sizeof(serveraddr));
    serveraddr.sin_family = AF_INET;
    serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
    serveraddr.sin_port = htons((unsigned short)port);

    if (bind(server->server_sock_fd, (struct sockaddr *) &serveraddr, sizeof(serveraddr)) < 0)
    {
        ht_tcp_server_stop(server);
        return HT_FALSE;
    }

    if (listen(server->server_sock_fd, 5) < 0)
    {
        ht_tcp_server_stop(server);
        return HT_FALSE;
    }

    server->client_connected_cb = client_connected_cb;
    server->client_connected_ud = user_data;
    server->accept_client_thread = ht_thread_create(_ht_tcp_server_run, server);

    return HT_TRUE;
}

void
ht_tcp_server_stop(HT_TCPServer* server)
{
    if (!ht_tcp_server_is_running(server))
    {
        return;
    }

    ht_mutex_lock(server->client_mutex);
    ht_bag_int_clear(&server->client_sock_fd);
    ht_mutex_unlock(server->client_mutex);

    int prev_sock_fd = server->server_sock_fd;
    server->server_sock_fd = -1;
#ifdef _WIN32
    closesocket(prev_sock_fd);
    WSACleanup();
#else
    shutdown(prev_sock_fd, 2);
#endif

    if (server->accept_client_thread)
    {
        ht_thread_join(server->accept_client_thread);
        ht_thread_destroy(server->accept_client_thread);
        server->accept_client_thread = NULL;
    }
}

HT_Boolean
ht_tcp_server_is_running(const HT_TCPServer* server)
{
    return server->server_sock_fd != -1;
}

void
ht_tcp_server_write(HT_TCPServer* server, char* buffer, size_t size)
{
    size_t i;

    ht_mutex_lock(server->client_mutex);

    for (i = 0; i < ht_bag_size(server->client_sock_fd); i++)
    {
        if (!ht_tcp_server_write_to_socket(server, ht_bag_nth(server->client_sock_fd, i), buffer, size))
        {
            ht_bag_int_remove_nth(&server->client_sock_fd, i);
            i--;
        }
    }

    ht_mutex_unlock(server->client_mutex);
}

HT_Boolean
ht_tcp_server_write_to_socket(HT_TCPServer* server, int sock_fd, char* buffer, size_t size)
{
    (void) server;
    if (size == 0)
    {
        return HT_TRUE;
    }

    size_t sent = 0;

    while (sent < size)
    {
        int n = send(sock_fd, buffer + sent, size - sent, 0);

        if (n <= 0)
        {
            return HT_FALSE;
        }

        sent += n;
    }

    return HT_TRUE;
}

static void*
_ht_tcp_server_run(void* user_data)
{
    HT_TCPServer* server = (HT_TCPServer*)user_data;
    while (ht_tcp_server_is_running(server))
    {
        struct sockaddr_in clientaddr;
#ifdef _WIN32
        int client_len;
#else
        socklen_t client_len;
#endif
        client_len = sizeof(clientaddr);
        int client_fd = accept(server->server_sock_fd, (struct sockaddr *) &clientaddr, &client_len);

        if (client_fd >= 0)
        {
            server->client_connected_cb(client_fd, server->client_connected_ud);
            ht_mutex_lock(server->client_mutex);
            ht_bag_int_add(&server->client_sock_fd, client_fd);
            ht_mutex_unlock(server->client_mutex);
        }
    }
    return NULL;
}

#endif /* HT_ENABLE_TCP_LISTENER */
