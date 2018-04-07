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

class GraphInfo // TODO: remove this class
{
public:
    using Id = uint32_t;

    GraphInfo(std::vector<std::string> fields, std::string name, Id id):
        _fields(std::move(fields)), _name(std::move(name)), _id(id)
    {}

    const std::string& get_name() const { return _name; }
    const std::vector<std::string>& get_fields() const { return _fields; }
    Id get_id() const { return _id; }

private:
    const std::vector<std::string> _fields;
    const std::string _name;
    const Id _id;
};

class JavaScriptUI : public BaseUI
{
public:
    JavaScriptUI(int port);
    ~JavaScriptUI() override;

    void add_klass(const parser::EventKlass* klass) override;
    void add_field(const parser::EventKlass* klass, const parser::EventKlassField* field) override;

private:
    void _client_connected();
    void _client_message_received(const std::string& message);

    void _send_json_object(const jsonxx::Object& obj);
    void _send_request_error(const std::string& message, const std::string& raw_json);
    void _send_error(const std::string& message);
    void _send_missing_fields_error(const std::string& raw_json);
    void _send_graphs_info();

    std::thread _server_th;
    WebSocket _server;
    std::unordered_map<GraphInfo::Id, GraphInfo> _graph_types;
    std::unordered_map<Graph::Id, std::unique_ptr<Graph>> _graphs;

    size_t _canvas_size = 0;
};

} // viewer
} // HawkTracer

#endif // HAWKTRACER_VIEWER_JAVASCRIPT_UI_HPP
