#include "internal/listeners/tcp_server.hpp"

#include <arpa/inet.h>
#include <unistd.h>

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
    shutdown(prev_sock_fd, 2);

    if (_accept_client_thread.joinable())
    {
        _accept_client_thread.join();
    }
}

bool TCPServer::start(int port)
{
    if (is_running())
    {
        stop();
    }

    _sock_fd = socket(AF_INET, SOCK_STREAM, 0);

    if (_sock_fd < 0)
    {
        return false;
    }

    int optval = 1;
    sockaddr_in serveraddr;
    if (setsockopt(_sock_fd, SOL_SOCKET, SO_REUSEADDR, (const void *)&optval , sizeof(int)) != 0)
    {
        stop();
        return false;
    }

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

    _accept_client_thread = std::thread([this] { _run(); });

    return true;
}

void TCPServer::write(char* buffer, size_t size)
{
    std::lock_guard<std::mutex> l(_client_mutex);

    for (auto it = _client_sock_fd.begin(); it != _client_sock_fd.end();)
    {
        if (_safe_write(*it, buffer, size))
        {
            ++it;
        }
        else
        {
            it = _client_sock_fd.erase(it);
        }
    }
}

bool TCPServer::_safe_write(int sock_fd, char* buffer, size_t size)
{
    size_t sent = 0;

    while (sent < size)
    {
        int n = ::write(sock_fd, buffer + sent, size - sent);

        if (n <= 0)
        {
            return false;
        }

        sent += n;
    }

    return true;
}

void TCPServer::_run()
{
    while (is_running())
    {
        struct sockaddr_in clientaddr;
        socklen_t client_len = sizeof(clientaddr);
        int client_fd = accept(_sock_fd, (struct sockaddr *) &clientaddr, &client_len);

        if (client_fd >= 0)
        {
            std::lock_guard<std::mutex> l(_client_mutex);
            _client_sock_fd.insert(client_fd);
        }
    }
}

} /* namespace HawkTracer */
