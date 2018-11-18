#include "internal/listeners/tcp_server.hpp"
#include "internal/mutex.h"
#include "hawktracer/alloc.h"

#ifdef HT_CPP11
#  include <thread>
#  define HT_THREAD_IMPL_CPP11
#elif defined(_WIN32)
#  include <windows.h>
#  define HT_THREAD_IMPL_WIN32
#elif defined(HT_HAVE_UNISTD_H)
#  include <unistd.h>
#  ifdef _POSIX_VERSION
#    include <pthread.h>
#    define HT_THREAD_IMPL_POSIX
#  endif
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
#include <unistd.h>
#endif

#include <string.h>
#include <set>

static void *_ht_tcp_server_run(void *user_data);

struct _HT_TCPServer
{
    HT_Thread* _accept_client_thread = NULL;
    std::set<int> _client_sock_fd; // TODO replace this with bag so we can compile the code using C compiler
    HT_Mutex* _client_mutex = NULL;
    int _sock_fd = -1;

     OnClientConnected _client_connected_cb = NULL;
     void* _client_connected_ud = NULL;
};

HT_TCPServer*
ht_tcp_server_create(void)
{
    HT_TCPServer* server = HT_CREATE_TYPE(HT_TCPServer);
    new (server) HT_TCPServer();

    server->_client_mutex = ht_mutex_create();

    return server;
}

void
ht_tcp_server_destroy(HT_TCPServer* server)
{
    ht_tcp_server_stop(server);
    ht_mutex_destroy(server->_client_mutex);
    server->~_HT_TCPServer();
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
        return false;
    }
#endif
    server->_sock_fd = socket(AF_INET, SOCK_STREAM, 0);

    if (server->_sock_fd < 0)
    {
        return false;
    }

#ifndef _WIN32
    int optval = 1;
    if (setsockopt(server->_sock_fd, SOL_SOCKET, SO_REUSEADDR, (const void *)&optval , sizeof(int)) != 0)
    {
        ht_tcp_server_stop(server);
        return false;
    }
#endif

    sockaddr_in serveraddr;
    memset((char *) &serveraddr, 0, sizeof(serveraddr));
    serveraddr.sin_family = AF_INET;
    serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
    serveraddr.sin_port = htons((unsigned short)port);

    if (bind(server->_sock_fd, (struct sockaddr *) &serveraddr, sizeof(serveraddr)) < 0)
    {
        ht_tcp_server_stop(server);
        return false;
    }

    if (listen(server->_sock_fd, 5) < 0)
    {
        ht_tcp_server_stop(server);
        return false;
    }

    server->_client_connected_cb = client_connected_cb;
    server->_client_connected_ud = user_data;
    server->_accept_client_thread = ht_thread_create(_ht_tcp_server_run, server);

    return true;
}

void
ht_tcp_server_stop(HT_TCPServer* server)
{
    if (!ht_tcp_server_is_running(server))
    {
        return;
    }

    ht_mutex_lock(server->_client_mutex);
    server->_client_sock_fd.clear();
    ht_mutex_unlock(server->_client_mutex);

    int prev_sock_fd = server->_sock_fd;
    server->_sock_fd = -1;
#ifdef _WIN32
    closesocket(prev_sock_fd);
    WSACleanup();
#else
    shutdown(prev_sock_fd, 2);
#endif

    if (server->_accept_client_thread)
    {
        ht_thread_join(server->_accept_client_thread);
        ht_thread_destroy(server->_accept_client_thread);
        server->_accept_client_thread = NULL;
    }
}

HT_Boolean
ht_tcp_server_is_running(const HT_TCPServer* server)
{
    return server->_sock_fd != -1;
}

void
ht_tcp_server_write(HT_TCPServer* server, char* buffer, size_t size)
{
    ht_mutex_lock(server->_client_mutex);

    for (auto it = server->_client_sock_fd.begin(); it != server->_client_sock_fd.end();)
    {
        if (ht_tcp_server_write_to_socket(server, *it, buffer, size))
        {
            ++it;
        }
        else
        {
            it = server->_client_sock_fd.erase(it);
        }
    }

    ht_mutex_unlock(server->_client_mutex);
}

HT_Boolean
ht_tcp_server_write_to_socket(HT_TCPServer* /* server */, int sock_fd, char* buffer, size_t size)
{
    if (size == 0)
    {
        return true;
    }

    size_t sent = 0;

    while (sent < size)
    {
        int n = send(sock_fd, buffer + sent, size - sent, 0);

        if (n <= 0)
        {
            return false;
        }

        sent += n;
    }

    return true;
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
        int client_fd = accept(server->_sock_fd, (struct sockaddr *) &clientaddr, &client_len);

        if (client_fd >= 0)
        {
            server->_client_connected_cb(client_fd, server->_client_connected_ud);
            ht_mutex_lock(server->_client_mutex);
            server->_client_sock_fd.insert(client_fd);
            ht_mutex_unlock(server->_client_mutex);
        }
    }
    return NULL;
}
