#include "TCPClient.h"

#include <arpa/inet.h>
#include <unistd.h>
#include <errno.h>

#include <stdexcept>
#include <cstring>
#include <thread>

#define BUFSIZE 1024

namespace hawktracer
{

TCPClient::TCPClient(bool wait_for_server) :
    _wait_for_server(wait_for_server)
{
}

TCPClient::~TCPClient()
{
    stop();
}

bool TCPClient::start(const std::string &ipAddress, uint16_t port)
{
    if (is_connected())
    {
        stop();
    }

    struct sockaddr_in serveraddr;

    _sockFd = socket(AF_INET, SOCK_STREAM, 0);
    if (_sockFd < 0)
    {
        return false;
    }

    bzero((char*)&serveraddr, sizeof(serveraddr));
    serveraddr.sin_family = AF_INET;
    serveraddr.sin_addr.s_addr = inet_addr(ipAddress.c_str());
    serveraddr.sin_port = htons(port);

    if (_wait_for_server)
    {
        while (connect(_sockFd, (struct sockaddr*)&serveraddr, sizeof(serveraddr)) < 0)
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
    }
    else
    {
        if (connect(_sockFd, (struct sockaddr*)&serveraddr, sizeof(serveraddr)) < 0)
        {
            stop();
            return false;
        }
    }

    return true;
}

void TCPClient::stop()
{
    if (!is_connected())
    {
        return;
    }

    bool err = close(_sockFd) < 0;

    _sockFd = -1;

    if (err)
    {
        _throwSystemError("error on closing connection");
    }
}

bool TCPClient::handle_responses(ResponseCallback callback, void* userData)
{
    char buf[BUFSIZE];

    while (true)
    {
        int size = read(_sockFd, buf, BUFSIZE);
        if (size < 0)
        {
            return false;
        }

        callback(buf, size, userData);
    }

    return true;
}

bool TCPClient::write(const char *buffer, size_t size)
{
    size_t sent = 0;

    while (sent < size)
    {
        int n = ::write(_sockFd, buffer + sent, size - sent);

        if (n <= 0)
        {
            return false;
        }
        sent += n;
    }

    return true;
}

bool TCPClient::is_connected() const
{
    return _sockFd != -1;
}

void TCPClient::_throwSystemError(const std::string &errorMsg)
{
    throw std::runtime_error("TCPClient error(" + std::to_string(errno) + "): " + errorMsg);
}

} // namespace hawktracer
