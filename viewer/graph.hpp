#ifndef HAWKTRACER_VIEWER_GRAPH_HPP
#define HAWKTRACER_VIEWER_GRAPH_HPP

#include "base_ui.hpp" // TODO For TimeRange, should be moved somewhere else
#include "jsonxx.h" // TODO don't like it :(

#include <hawktracer/parser/make_unique.hpp>

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

    Graph(std::string graph_id) :
        _graph_id(std::move(graph_id))
    {
    }

    Id get_id() const { return _graph_id; }

    virtual Query get_query() = 0;

    virtual jsonxx::Object create_graph_data(const std::vector<EventRef>& events, TimeRange time_range) = 0;
    virtual jsonxx::Object get_properties() = 0;

protected:
    const std::string _graph_id;
};

class GraphFactory
{
public:
    using CreateFnc = std::unique_ptr<Graph>(*)(parser::KlassRegister*, std::string, const jsonxx::Object&);

    template<typename T>
    static std::unique_ptr<Graph> default_constructor(parser::KlassRegister* klass_register, std::string graph_id, const jsonxx::Object& graph_description)
    {
        return parser::make_unique<T>(klass_register, std::move(graph_id), graph_description);
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
    std::unique_ptr<Graph> create_graph(parser::KlassRegister* klass_register, Graph::TypeId type_id, std::string graph_id, const jsonxx::Object& graph_description);
    std::unordered_map<std::string, Info> get_graphs_info() const { std::lock_guard<std::mutex> l(_mtx); return _graph_types; }

private:
    mutable std::mutex _mtx;
    std::unordered_map<std::string, Info> _graph_types;
};

} // viewer
} // HawkTracer

#endif // HAWKTRACER_VIEWER_GRAPH_HPP
