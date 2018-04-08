#ifndef HAWKTRACER_VIEWER_XY_GRAPH_HPP
#define HAWKTRACER_VIEWER_XY_GRAPH_HPP

#include "graph.hpp"

#include <parser/klass_register.hpp>

namespace HawkTracer
{
namespace viewer
{

class XYGraph : public Graph
{
public:
    XYGraph(std::string graph_id, const jsonxx::Object& graph_description) :
        Graph(std::move(graph_id))
    {
        FieldMapping field_mapping = {};
        for (const auto& value : graph_description.kv_map())
        {
            field_mapping[value.first] = value.second->get<jsonxx::String>();
        }

        // TODO this might fail if klass/field doesn't exist
        // TODO do we really have to use klassregister here???
        _klass_id = static_cast<HT_EventKlassId>(graph_description.get<jsonxx::Number>("klassId"));

        _field = parser::KlassRegister::get().get_klass(_klass_id)->get_field(field_mapping.at("value").c_str(), true);
    }

    static constexpr const char* get_type_id() { return "XY"; }
    static std::vector<std::string> get_type_fields() { return {"value"}; }

    jsonxx::Object create_graph_data(const std::vector<EventRef>& events, TimeRange time_range) override;
    jsonxx::Object get_properties() override;

private:
    template<typename T>
    std::vector<std::pair<size_t, double>> _create_data_set(
            std::vector<EventRef> events,
            TimeRange time_range);

    std::shared_ptr<const parser::EventKlassField> _field;
    size_t _width = 500;
    HT_EventKlassId _klass_id;
};

} // namespace viewer
} // namespace HawkTracer

#endif // HAWKTRACER_VIEWER_XY_GRAPH_HPP
