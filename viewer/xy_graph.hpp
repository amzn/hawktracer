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
    XYGraph(HT_EventKlassId klass_id, std::string graph_id, const FieldMapping& field_mapping) :
        Graph(klass_id, std::move(graph_id))
    {
        // TODO this might fail if klass/field doesn't exist
        // TODO do we really have to use klassregister here???
        _field = parser::KlassRegister::get().get_klass(klass_id)->get_field(field_mapping.at("value").c_str(), true);
    }

    jsonxx::Object create_graph_data(const std::vector<EventRef>& events, TimeRange time_range, size_t canvas_size) override;

private:
    template<typename T>
    std::vector<std::pair<size_t, double>> _create_data_set(
            std::vector<EventRef> events,
            TimeRange time_range,
            size_t canvas_size);

    std::shared_ptr<const parser::EventKlassField> _field;
};

} // namespace viewer
} // namespace HawkTracer

#endif // HAWKTRACER_VIEWER_XY_GRAPH_HPP
