#include "TCPServer.h"

#include <arpa/inet.h>
#include <unistd.h>

#include <vector>
#include <cstring>

namespace hawktracer
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
    setsockopt(_sock_fd, SOL_SOCKET, SO_REUSEADDR, (const void *)&optval , sizeof(int));

    bzero((char *) &serveraddr, sizeof(serveraddr));
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

    _accept_client_thread = std::thread([this] {
        while (is_running())
        {
            struct sockaddr_in clientaddr;
            socklen_t clientlen = sizeof(clientaddr);
            int clientFd = accept(_sock_fd, (struct sockaddr *) &clientaddr, &clientlen);

            if (clientFd >= 0)
            {
                std::lock_guard<std::mutex> l(_client_mutex);
                _client_sock_fd.insert(clientFd);
            }
        }
    });

    return true;
}


void TCPServer::write(char* buffer, size_t size)
{
    std::lock_guard<std::mutex> l(_client_mutex);
    std::vector<int> toRemoveClientFd;

    for (const auto& clientFd : _client_sock_fd)
    {
        size_t sent = 0;

        while (sent < size)
        {
            int n = ::write(clientFd, buffer + sent, size - sent);

            if (n <= 0)
            {
                toRemoveClientFd.push_back(clientFd);
                break;
            }

            sent += n;
        }
    }

    for (const auto& clientFd : toRemoveClientFd)
    {
        _client_sock_fd.erase(clientFd);
    }
}

} // namespace hawktracer
