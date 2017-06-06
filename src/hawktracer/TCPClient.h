#ifndef HAWKTRACER_TCPCLIENT_H_
#define HAWKTRACER_TCPCLIENT_H_

#include <string>

namespace hawktracer
{

class TCPClient
{
public:
    using ResponseCallback = void(*)(char*, std::size_t, void*); // buffer, buffer size, user data

    explicit TCPClient(bool wait_for_server = true);
    ~TCPClient();

    bool start(const std::string& ipAddress, uint16_t port);
    void stop();

    bool is_connected() const;

    bool handle_responses(ResponseCallback callback, void* userData);
    bool write(const char* buffer, size_t size);

private:
    void _throwSystemError(const std::string &errorMsg);

    bool _wait_for_server;
    int _sockFd = -1;
};

} // namespace hawktracer

#endif // HAWKTRACER_TCPCLIENT_H_
