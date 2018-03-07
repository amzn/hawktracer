#include "internal/listeners/tcp_server.hpp"

#ifdef _WIN32
#include <WinSock2.h>
#pragma comment(lib, "Ws2_32.lib")
#else
#include <arpa/inet.h>
#include <unistd.h>
#endif

#include <cstring>
#include <vector>

namespace HawkTracer
{

TCPServer::~TCPServer()
{
    stop();
}

void TCPServer::stop()
{
    if (!is_running())
    {
        return;
    }

    {
        std::lock_guard<std::mutex> l(_client_mutex);
        _client_sock_fd.clear();
    }

    int prev_sock_fd = _sock_fd;
    _sock_fd = -1;
#ifdef _WIN32
    closesocket(prev_sock_fd);
    WSACleanup();
#else
    shutdown(prev_sock_fd, 2);
#endif

    if (_accept_client_thread.joinable())
    {
        _accept_client_thread.join();
    }
}

bool TCPServer::start(int port, OnClientConnected client_connected, void* user_data)
{
    if (is_running())
    {
        stop();
    }
#ifdef _WIN32
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
    {
        return false;
    }
#endif
    _sock_fd = socket(AF_INET, SOCK_STREAM, 0);

    if (_sock_fd < 0)
    {
        return false;
    }

#ifndef _WIN32
    int optval = 1;
    if (setsockopt(_sock_fd, SOL_SOCKET, SO_REUSEADDR, (const void *)&optval , sizeof(int)) != 0)
    {
        stop();
        return false;
    }
#endif

    sockaddr_in serveraddr;
    memset((char *) &serveraddr, 0, sizeof(serveraddr));
    serveraddr.sin_family = AF_INET;
    serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
    serveraddr.sin_port = htons((unsigned short)port);

    if (bind(_sock_fd, (struct sockaddr *) &serveraddr, sizeof(serveraddr)) < 0)
    {
        stop();
        return false;
    }

    if (listen(_sock_fd, 5) < 0)
    {
        stop();
        return false;
    }

    _accept_client_thread = std::thread([this, client_connected, user_data]
    {
        _run(client_connected, user_data);
    });

    return true;
}

void TCPServer::write(char* buffer, size_t size)
{
    std::lock_guard<std::mutex> l(_client_mutex);

    for (auto it = _client_sock_fd.begin(); it != _client_sock_fd.end();)
    {
        if (write_to_socket(*it, buffer, size))
        {
            ++it;
        }
        else
        {
            it = _client_sock_fd.erase(it);
        }
    }
}

bool TCPServer::write_to_socket(int sock_fd, char* buffer, size_t size)
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

void TCPServer::_run(OnClientConnected client_connected, void* user_data)
{
    while (is_running())
    {
        struct sockaddr_in clientaddr;
#ifdef _WIN32
        int client_len;
#else
        socklen_t client_len;
#endif
        client_len = sizeof(clientaddr);
        int client_fd = accept(_sock_fd, (struct sockaddr *) &clientaddr, &client_len);

        if (client_fd >= 0)
        {
            std::lock_guard<std::mutex> l(_client_mutex);
            client_connected(client_fd, user_data);
            _client_sock_fd.insert(client_fd);
        }
    }
}

} /* namespace HawkTracer */
