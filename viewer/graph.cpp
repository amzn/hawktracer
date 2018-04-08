#include "graph.hpp"
#include "xy_graph.hpp"

#include <parser/make_unique.hpp>

namespace HawkTracer
{
namespace viewer
{

class FakeGraph : public Graph
{
public:
    FakeGraph(HT_EventKlassId klass_id, std::string graph_id, const FieldMapping&) :
        Graph(klass_id, std::move(graph_id))
    {
    }

    static constexpr const char* get_type_id() { return "Fake"; }
    static std::vector<std::string> get_type_fields() { return {"value1", "value2"}; }

    jsonxx::Object create_graph_data(const std::vector<EventRef>&, TimeRange) override {return jsonxx::Object{};}
    jsonxx::Object get_properties() {return jsonxx::Object{};}
};


#define HT_VIEWER_EMBEDDED_GRAPH_TYPES \
    XYGraph, \
    FakeGraph

GraphFactory::GraphFactory()
{
#define HT_VIEWER_GRAPH_CREATE(TYPE, UNUSED) \
    register_type(default_constructor<TYPE>, TYPE::get_type_id(), TYPE::get_type_fields());

    MKCREFLECT_FOREACH(HT_VIEWER_GRAPH_CREATE, 0, HT_VIEWER_EMBEDDED_GRAPH_TYPES)

#undef HT_VIEWER_GRAPH_CREATE
}

void GraphFactory::register_type(CreateFnc factory_method, Graph::TypeId type_id, std::vector<std::string> fields)
{
    std::lock_guard<std::mutex> l(_mtx);

    if (_graph_types.find(type_id) != _graph_types.end())
    {
        return;
    }

    _graph_types.emplace(type_id, Info(std::move(fields), type_id, factory_method));
}

std::unique_ptr<Graph> GraphFactory::create_graph(Graph::TypeId type_id, HT_EventKlassId klass_id, std::string graph_id, const Graph::FieldMapping& mapping)
{
    std::lock_guard<std::mutex> l(_mtx);

    auto type_it = _graph_types.find(type_id);
    return (type_it != _graph_types.end()) ?
                type_it->second.factory_method(klass_id, std::move(graph_id), mapping) :
                nullptr;
}

} // viewer
} // HawkTracer
