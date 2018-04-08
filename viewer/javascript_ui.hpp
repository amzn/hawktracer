#ifndef HAWKTRACER_VIEWER_JAVASCRIPT_UI_HPP
#define HAWKTRACER_VIEWER_JAVASCRIPT_UI_HPP

#include "base_ui.hpp"
#include "websocket.hpp"
#include "graph.hpp"

#include <parser/event_klass.hpp>
#include <parser/klass_register.hpp>

#include <thread>

namespace jsonxx
{
class Object;
}

namespace HawkTracer
{
namespace viewer
{

class JavaScriptUI : public BaseUI
{
public:
    JavaScriptUI(int port);
    ~JavaScriptUI() override;

    void add_klass(const parser::EventKlass* klass) override;
    void add_field(const parser::EventKlass* klass, const parser::EventKlassField* field) override;
    int run() override;

private:
    void _client_connected();
    void _client_message_received(const std::string& message);

    void _send_json_object(const jsonxx::Object& obj);
    void _send_request_error(const std::string& message, const std::string& raw_json);
    void _send_error(const std::string& message);
    void _send_missing_fields_error(const std::string& raw_json);
    void _send_graphs_info();

    WebSocket _server;
    std::unordered_map<Graph::Id, std::unique_ptr<Graph>> _graphs;

    GraphFactory _graph_factory;

    int _port;
};

} // viewer
} // HawkTracer

#endif // HAWKTRACER_VIEWER_JAVASCRIPT_UI_HPP
