#ifndef WEBSOCKET_HPP
#define WEBSOCKET_HPP

#include <memory>
#include <functional>

class WebSocketServer;

class WebSocket
{
public:
    WebSocket();
    ~WebSocket();

    void start(int port, std::function<void()> on_connected,
               std::function<void(const std::string&)> on_receive);
    void stop();

    void send_text(const std::string& message);

private:
    std::unique_ptr<WebSocketServer> _server;
    std::function<void(const std::string&)> _on_receive;
};

#endif // WEBSOCKET_HPP
