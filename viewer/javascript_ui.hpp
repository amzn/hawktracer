#ifndef HAWKTRACER_VIEWER_JAVASCRIPT_UI_HPP
#define HAWKTRACER_VIEWER_JAVASCRIPT_UI_HPP

#include "base_ui.hpp"
#include "websocket.hpp"

#include <parser/event_klass.hpp>

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
    void show_data(HT_EventKlassId event_klass, const char* field_name,
            const std::vector<EventRef>& events) override;
private:
    void _client_connected();
    void _client_message_received(const std::string& message);

    void _send_json_object(const jsonxx::Object& obj);
    void _send_request_error(const std::string& message, const std::string& raw_json);
    void _send_error(const std::string& message);
    void _send_missing_fields_error(const std::string& raw_json);

    template<typename T>
    std::vector<std::pair<size_t, double>> _create_data_set(
            std::vector<EventRef> events,
            const char* field,
            TimeRange time_range);

    std::thread _server_th;
    WebSocket _server;

    size_t _canvas_size = 0;
};

} // viewer
} // HawkTracer

#endif // HAWKTRACER_VIEWER_JAVASCRIPT_UI_HPP
