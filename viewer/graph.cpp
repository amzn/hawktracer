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
    FakeGraph(std::string graph_id, const jsonxx::Object&) :
        Graph(std::move(graph_id))
    {
    }

    static constexpr const char* get_type_id() { return "Fake"; }
    static std::vector<std::string> get_type_fields() { return {"value1", "value2"}; }

    jsonxx::Object create_graph_data(const std::vector<EventRef>&, TimeRange) override {return jsonxx::Object{};}
    jsonxx::Object get_properties() {return jsonxx::Object{};}
    Query get_query() { return Query{}; }
};

class StatsGraph : public Graph
{
public:
    StatsGraph(std::string graph_id, const jsonxx::Object&) :
        Graph(std::move(graph_id))
    {
        _query.klass_id = (HT_EventKlassId)-1;
    }

    static constexpr const char* get_type_id() { return "STATS"; }
    static std::vector<std::string> get_type_fields() { return {}; }
    Query get_query() { return _query; }

    jsonxx::Object create_graph_data(const std::vector<EventRef>& events, TimeRange) override
    {
        jsonxx::Object response;

        response << "rangeEventCount" << events.size();

        return response;
    }

    jsonxx::Object get_properties() {return jsonxx::Object{};}

private:
    Query _query;
};

#define HT_VIEWER_EMBEDDED_GRAPH_TYPES \
    XYGraph, \
    FakeGraph, \
    StatsGraph

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

std::unique_ptr<Graph> GraphFactory::create_graph(Graph::TypeId type_id, std::string graph_id, const jsonxx::Object& graph_description)
{
    std::lock_guard<std::mutex> l(_mtx);

    auto type_it = _graph_types.find(type_id);
    return (type_it != _graph_types.end()) ?
                type_it->second.factory_method(std::move(graph_id), graph_description) :
                nullptr;
}

} // viewer
} // HawkTracer
