#include "hawktracer/client_utils/tcp_client_stream.hpp"

#ifdef _WIN32
#define NOMINMAX
#include <WinSock2.h>
#pragma comment(lib, "Ws2_32.lib")
#else
#include <arpa/inet.h>
#include <unistd.h>
#endif
#include <cstring>

namespace HawkTracer
{
namespace ClientUtils
{
#define BUFSIZE 1024

static int close_socket(int sock_fd)
{
#ifdef _WIN32
    return closesocket(sock_fd);
#else
    return close(sock_fd);
#endif
}

TCPClientStream::TCPClientStream(const std::string& ip_address, uint16_t port, bool wait_for_server) :
    _sock_fd(-1),
    _ip_address(ip_address),
    _port(port),
    _wait_for_server(wait_for_server)
{
}

TCPClientStream::~TCPClientStream()
{
    stop();
}

bool TCPClientStream::start()
{
    if (is_connected())
    {
        stop();
    }

#ifdef _WIN32
    WSAData wsa_data;
    if (WSAStartup(MAKEWORD(2, 2), &wsa_data) != 0)
    {
        return false;
    }
#endif

    struct sockaddr_in serveraddr;

    _sock_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (_sock_fd < 0)
    {
        return false;
    }

    memset((char*)&serveraddr, 0, sizeof(serveraddr));
    serveraddr.sin_family = AF_INET;
    serveraddr.sin_addr.s_addr = inet_addr(_ip_address.c_str());
    serveraddr.sin_port = htons(_port);

    if (_wait_for_server)
    {
        while (connect(_sock_fd, (struct sockaddr*)&serveraddr, sizeof(serveraddr)) < 0)
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
    }
    else
    {
        if (connect(_sock_fd, (struct sockaddr*)&serveraddr, sizeof(serveraddr)) < 0)
        {
            stop();
            return false;
        }
    }

    _thread = std::thread([this] { _run(); });

    return true;
}

void TCPClientStream::stop()
{
    if (is_connected())
    {
        close_socket(_sock_fd);
        _sock_fd = -1;
    }

#ifdef _WIN32
    WSACleanup();
#endif

    if (_thread.joinable())
    {
        _thread.join();
    }
}

void TCPClientStream::_run()
{
    char buf[BUFSIZE];

    while (is_connected())
    {
        int size = recv(_sock_fd, buf, BUFSIZE, 0);
        
        if (size == 0 || size == -1)
        {
            close_socket(_sock_fd);
            _sock_fd = -1;
        }
        else if (size > 0)
        {
            {
                std::lock_guard<std::mutex> l(_datas_mtx);
                _datas.push(std::make_pair(0u, std::vector<char>(buf, buf + size)));
            }
            _datas_cv.notify_one();
        }
    }

    _datas_cv.notify_one();
}

bool TCPClientStream::is_connected() const
{
    return _sock_fd != -1;
}

bool TCPClientStream::_wait_for_data(std::unique_lock<std::mutex>& l)
{
     _datas_cv.wait(l, [this] { return !_datas.empty() || !is_connected(); });
     return !_datas.empty();
}

int TCPClientStream::read_byte()
{
    std::unique_lock<std::mutex> l(_datas_mtx);

    if (!_wait_for_data(l))
    {
        return -1;
    }
    else
    {
        auto& buffer = _datas.front();
        int b = buffer.second[buffer.first++];
        _pop_if_used();
        return b;
    }
}

bool TCPClientStream::read_data(char* buff, size_t size)
{
    do {
        std::unique_lock<std::mutex> l(_datas_mtx);
        if (!_wait_for_data(l))
        {
            return false;
        }
        auto& buffer = _datas.front();
        size_t bytes_count = std::min(size, buffer.second.size() - buffer.first);
        memcpy(buff, &buffer.second[buffer.first], bytes_count);
        buffer.first += bytes_count;
        size -= bytes_count;
        buff += bytes_count;
        _pop_if_used();
    } while (size != 0);

    return true;
}

} // namespace ClientUtils
} // namespace HawkTracer
