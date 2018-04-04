#include "javascript_ui.hpp"

#include "jsonxx_utils.hpp"

#include <parser/klass_register.hpp>

#include <limits>
#include <algorithm>

namespace HawkTracer
{
namespace viewer
{

using namespace parser;

JavaScriptUI::JavaScriptUI(int port)
{
    // TODO error handling
    _server_th = std::thread([this, port] {
        _server.start(
                    port,
                    [this] { _client_connected(); },
        [this] (const std::string& msg) { _client_message_received(msg); });
    });
}

JavaScriptUI::~JavaScriptUI()
{
    _server.stop();
    _server_th.join();
}

template<typename T>
std::vector<std::pair<size_t, double>> JavaScriptUI::_create_data_set(
        std::vector<EventRef> events,
        const char* field,
        TimeRange time_range)
{
    // TODO this function could be done better...
    // TODO profile and optimize

    std::sort(events.begin(), events.end(), [] (const EventRef& e1, const EventRef& e2) {
        return e1.get().get_timestamp() < e2.get().get_timestamp();
    });

    std::vector<std::pair<size_t, double>> out;
    if (events.empty())
    {
        return out;
    }

    double step = static_cast<double>(time_range.get_duration()) / _canvas_size;

    double current_sum = 0;
    size_t current_cnt = 0;
    auto event_it = events.begin();
    size_t pos_in_canvas = 0;

    while (pos_in_canvas < _canvas_size && event_it != events.end())
    {
        if (event_it->get().get_timestamp() < time_range.start + step * (pos_in_canvas+1))
        {
            current_sum += static_cast<double>(event_it->get().get_value<T>(field));
            ++current_cnt;
            ++event_it;
        }
        else
        {
            if (current_cnt)
            {
                out.push_back(std::make_pair(pos_in_canvas, current_sum/current_cnt));
                current_sum = 0;
                current_cnt = 0;
            }
            pos_in_canvas++;
        }
    }

    if (current_cnt)
    {
        out.push_back(std::make_pair(pos_in_canvas, current_sum/current_cnt));
    }

    return out;
}

void JavaScriptUI::_client_connected()
{
    _request_klass_register();
}

void JavaScriptUI::_client_message_received(const std::string& message)
{
    jsonxx::Object obj;

    if (!obj.parse(message))
    {
        std::cerr << "Unable to parse message: " << std::endl << message << std::endl;
    }

    auto command = obj.get<jsonxx::String>("command", "");
    if (command == "track_field")
    {
        if (!check_required_fields<jsonxx::Number, jsonxx::String>(obj, {"klass", "field"}))
        {
            _send_missing_fields_error(message);
        }
        else
        {
            HT_EventKlassId klass_id = obj.get<jsonxx::Number>("klass");
            std::string field_name = obj.get<jsonxx::String>("field");
            if (!_track_field(klass_id, field_name.c_str()))
            {
                _send_request_error("Can't track field. Klass or field doesn't exist.", message);
            }
        }
    }
    else if (command == "set_canvas_size")
    {
        if (!check_required_fields<jsonxx::Number>(obj, {"value"}))
        {
            _send_missing_fields_error(message);
        }
        else
        {
            _canvas_size = obj.get<jsonxx::Number>("value");
        }
    }
    else if (command == "get_total_ts_range")
    {
        _send_json_object(make_json_object(
                              "command", "total_ts_range",
                              "start", _get_total_ts_range().start,
                              "stop", _get_total_ts_range().stop));
    }
    else if (command == "set_current_ts_range")
    {
        if (!check_required_fields<jsonxx::Number, jsonxx::Number>(obj, {"duration", "stop"}))
        {
            _send_missing_fields_error(message);
        }
        else
        {
            auto duration = static_cast<HT_DurationNs>(obj.get<jsonxx::Number>("duration", 0));
            auto stop_ts = static_cast<HT_TimestampNs>(obj.get<jsonxx::Number>("stop", 0));
            _set_time_range(duration, stop_ts);
        }
    }
    else
    {
        _request_data();
    }
}

void JavaScriptUI::add_klass(const parser::EventKlass* klass)
{
    _send_json_object(make_json_object(
                          "command", "add_klass", "name", klass->get_name(),
                          "id", klass->get_id()));
}

void JavaScriptUI::add_field(const parser::EventKlass* klass, const parser::EventKlassField* field)
{
    if (field->get_type_id() == FieldTypeId::STRUCT)
    {
        auto klass_id = KlassRegister::get().get_klass_id(field->get_type_name());
        for (const auto& klass_field : KlassRegister::get().get_klass(klass_id)->get_fields())
        {
            add_field(klass, klass_field.get());
        }
    }
    else
    {
        _send_json_object(make_json_object(
                              "command", "add_field",
                              "name", field->get_name(), "klass", klass->get_id()));
    }
}

void JavaScriptUI::show_data(
        HT_EventKlassId event_klass, const char* field_name,
        const std::vector<EventRef>& events)
{
    auto field = parser::KlassRegister::get().get_klass(event_klass)->get_field(field_name, true);
    if (!field || !field->is_numeric())
    {
        _send_error("Field type not supported");
        return;
    }

    jsonxx::Object obj;
    append_to_json_object(obj, "command", "data", "klass", event_klass, "field", std::string(field_name));

    std::vector<std::pair<size_t, double>> values;
    switch(field->get_type_id())
    {
#define CREATE_DATASET(TYPE, C_TYPE) \
    case FieldTypeId::TYPE: values = _create_data_set<C_TYPE>(events, field_name, _get_current_ts_range()); break
    CREATE_DATASET(UINT8, uint8_t);
    CREATE_DATASET(UINT16, uint16_t);
    CREATE_DATASET(UINT32, uint32_t);
    CREATE_DATASET(UINT64, uint64_t);
#undef CREATE_DATASET
    default:
        break;
    }

    auto min_value = std::numeric_limits<jsonxx::Number>::max();
    auto max_value = std::numeric_limits<jsonxx::Number>::min();

    jsonxx::Array point_values;
    for (const auto& val : values)
    {
        if (val.second > max_value) { max_value = val.second; }
        if (val.second < min_value) { min_value = val.second; }

        point_values << make_json_object("x", val.first, "y", val.second);;
    }

    obj << "data" << point_values;
    append_to_json_object(obj, "minValue", min_value, "maxValue", max_value);

    _send_json_object(obj);
}

void JavaScriptUI::_send_json_object(const jsonxx::Object& obj)
{
    _server.send_text(obj.json());
}

void JavaScriptUI::_send_request_error(const std::string& message, const std::string& raw_json)
{
    _send_error("Error:\n" + message + "\nRaw json: " + raw_json);
}

void JavaScriptUI::_send_error(const std::string& message)
{
    _send_json_object(make_json_object("command", "error", "message", message));
}

void JavaScriptUI::_send_missing_fields_error(const std::string& raw_json)
{
    _send_request_error("missing required fields in a request", raw_json);
}

} // namespace viewer
} // namespace HawkTracer
