#ifndef HAWKTRACER_VIEWER_GRAPH_HPP
#define HAWKTRACER_VIEWER_GRAPH_HPP

#include "base_ui.hpp" // TODO For TimeRange, should be moved somewhere else
#include "jsonxx.h" // TODO don't like it :(

#include <parser/make_unique.hpp>

#include <hawktracer/base_types.h>

#include <unordered_map>
#include <string>
#include <vector>

namespace HawkTracer
{
namespace viewer
{

class Graph
{
public:
    using TypeId = std::string;
    using Id = std::string;
    using FieldMapping = std::unordered_map<std::string, std::string>;

    Graph(HT_EventKlassId klass_id, std::string graph_id) :
        _klass_id(klass_id), _graph_id(std::move(graph_id))
    {
    }

    HT_EventKlassId get_klass_id() const { return _klass_id; }
    Id get_id() const { return _graph_id; }

    virtual jsonxx::Object create_graph_data(const std::vector<EventRef>& events, TimeRange time_range, size_t canvas_size) = 0;

protected:
    const HT_EventKlassId _klass_id;
    const std::string _graph_id;
};

class GraphFactory
{
public:
    using CreateFnc = std::unique_ptr<Graph>(*)(HT_EventKlassId klass_id, std::string graph_id, const Graph::FieldMapping& mapping);

    template<typename T>
    static std::unique_ptr<Graph> default_constructor(HT_EventKlassId klass_id, std::string graph_id, const Graph::FieldMapping& mapping)
    {
        return parser::make_unique<T>(klass_id, std::move(graph_id), std::move(mapping));
    }

    class Info
    {
    public:
        Info(std::vector<std::string> fields, Graph::TypeId type_id, CreateFnc factory_method):
            fields(std::move(fields)), type_id(std::move(type_id)), factory_method(factory_method)
        {}

        const std::vector<std::string> fields;
        const Graph::TypeId type_id;
        const CreateFnc factory_method;
    };

    GraphFactory();

    void register_type(CreateFnc factory_method, Graph::TypeId type_id, std::vector<std::string> fields);
    std::unique_ptr<Graph> create_graph(Graph::TypeId type_id, HT_EventKlassId klass_id, std::string graph_id, const Graph::FieldMapping& mapping);
    std::unordered_map<std::string, Info> get_graphs_info() const { std::lock_guard<std::mutex> l(_mtx); return _graph_types; }

private:
    mutable std::mutex _mtx;
    std::unordered_map<std::string, Info> _graph_types;
};

} // viewer
} // HawkTracer

#endif // HAWKTRACER_VIEWER_GRAPH_HPP
