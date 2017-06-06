#ifndef HAWKTRACER_TCPSERVER_H_
#define HAWKTRACER_TCPSERVER_H_

#include <unordered_set>
#include <string>
#include <mutex>
#include <thread>

namespace hawktracer
{

class TCPServer
{
public:
    using BufferHandlerCallback = void(*)(char*, size_t, void*); // buffer, buffer size, user data

    ~TCPServer();

    void write(char* buffer, size_t size);
    bool start(int port);
    void stop();

    bool is_running() const { return _sock_fd != -1; }
    size_t client_count() const { return _client_sock_fd.size(); }

private:
    std::thread _accept_client_thread;
    std::unordered_set<int> _client_sock_fd;
    std::mutex _client_mutex;
    int _sock_fd = -1;
};

} // namespace hawktracer

#endif // HAWKTRACER_TCPSERVER_H_
