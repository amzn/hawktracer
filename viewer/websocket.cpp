#include "websocket.hpp"

#include <websocketpp/server.hpp>
#include <websocketpp/config/asio_no_tls.hpp>

class WebSocketServer : public websocketpp::server<websocketpp::config::asio>
{
public:
    websocketpp::connection_hdl _handler;
};

using message_ptr = WebSocketServer::message_ptr;

WebSocket::WebSocket() :
    _server(std::unique_ptr<WebSocketServer>(new WebSocketServer()))
{

}

WebSocket::~WebSocket()
{

}

void WebSocket::send_text(const std::string& message)
{
    if (_server->_handler.lock())
    {
        _server->send(_server->_handler, message, websocketpp::frame::opcode::TEXT);
    }
}

void WebSocket::start(int port, std::function<void()> on_connected,
                      std::function<void(const std::string&)> on_receive)
{
    try
    {
        _on_receive = on_receive;
        _server->set_access_channels(websocketpp::log::alevel::all);
        _server->clear_access_channels(websocketpp::log::alevel::frame_payload);

        _server->init_asio();

        _server->set_reuse_addr(true);
        _server->set_open_handler([this, on_connected] (websocketpp::connection_hdl hdl) {
            _server->_handler = hdl;
            on_connected();
        });
        _server->set_message_handler([this](websocketpp::connection_hdl, message_ptr message) {
            _on_receive(message->get_payload());
        });
        _server->set_close_handler([this] (websocketpp::connection_hdl){
            _server->_handler.reset();
        });

        _server->set_listen_backlog(5);
        _server->listen(port);
        _server->start_accept();
        _server->run();
    }
    catch (const websocketpp::lib::error_code& error_code)
    {
        throw std::runtime_error(error_code.message());
    }
}

void WebSocket::stop()
{
    if (_server->_handler.lock())
    {
        _server->_handler.reset();
    }
    _server->stop_listening();
}
