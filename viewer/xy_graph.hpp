#ifndef HAWKTRACER_VIEWER_XY_GRAPH_HPP
#define HAWKTRACER_VIEWER_XY_GRAPH_HPP

#include "graph.hpp"

#include <hawktracer/parser/klass_register.hpp>

namespace HawkTracer
{
namespace viewer
{

class XYGraph : public Graph
{
public:
    XYGraph(parser::KlassRegister* klass_register, std::string graph_id, const jsonxx::Object& graph_description);

    static constexpr const char* get_type_id() { return "XY"; }
    static std::vector<std::string> get_type_fields() { return {"value"}; }

    jsonxx::Object create_graph_data(const std::vector<EventRef>& events, TimeRange time_range) override;
    jsonxx::Object get_properties() override;
    Query get_query() override { return _query; }

private:
    template<typename T>
    std::vector<std::pair<size_t, double>> _create_data_set(
            std::vector<EventRef> events,
            TimeRange time_range);

    std::shared_ptr<const parser::EventKlassField> _field;
    size_t _width = 500;
    Query _query;
};

} // namespace viewer
} // namespace HawkTracer

#endif // HAWKTRACER_VIEWER_XY_GRAPH_HPP
