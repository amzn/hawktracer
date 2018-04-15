#include "xy_graph.hpp"

#include "jsonxx_utils.hpp"

#include <algorithm>

namespace HawkTracer
{
namespace viewer
{

using namespace parser;

XYGraph::XYGraph(parser::KlassRegister* klass_register, std::string graph_id, const jsonxx::Object& graph_description) :
    Graph(std::move(graph_id))
{
    FieldMapping field_mapping;
    for (const auto& value : graph_description.get<jsonxx::Object>("fieldMap").kv_map())
    {
        field_mapping[value.first] = value.second->get<jsonxx::String>();
    }

    // TODO this might fail if klass/field doesn't exist
    // TODO do we really have to use klassregister here???
    _query.klass_id = static_cast<HT_EventKlassId>(graph_description.get<jsonxx::Number>("klassId"));

    _field = klass_register->get_klass(_query.klass_id)->get_field(field_mapping.at("value").c_str(), true);
}

jsonxx::Object XYGraph::create_graph_data(const std::vector<EventRef>& events, TimeRange time_range)
{
    jsonxx::Object obj;

    std::vector<std::pair<size_t, double>> values;

    switch(_field->get_type_id())
    {
#define CREATE_DATASET(TYPE, C_TYPE) \
    case FieldTypeId::TYPE: values = _create_data_set<C_TYPE>(events, time_range); break
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

    obj << "dataPoints" << point_values;
    append_to_json_object(obj, "minValue", min_value, "maxValue", max_value);

    return obj;
}

jsonxx::Object XYGraph::get_properties()
{
    return make_json_object("width", _width);
}

template<typename T>
std::vector<std::pair<size_t, double>> XYGraph::_create_data_set(
        std::vector<EventRef> events,
        TimeRange time_range)
{
    std::vector<std::pair<size_t, double>> out;
    if (events.empty())
    {
        return out;
    }

    std::sort(events.begin(), events.end(), [] (const EventRef& e1, const EventRef& e2) {
        return e1.get().get_timestamp() < e2.get().get_timestamp();
    });

    double step = static_cast<double>(time_range.get_duration()) / _width;

    double current_sum = 0;
    size_t current_cnt = 0;
    auto event_it = events.begin();
    size_t pos_in_canvas = 0;

    while (pos_in_canvas < _width && event_it != events.end())
    {
        if (event_it->get().get_timestamp() < time_range.start + step * (pos_in_canvas+1))
        {
            current_sum += static_cast<double>(event_it->get().get_value<T>(_field->get_name()));
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

} // namespace viewer
} // namespace HawkTracer
