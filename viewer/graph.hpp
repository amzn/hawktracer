#ifndef HAWKTRACER_VIEWER_GRAPH_HPP
#define HAWKTRACER_VIEWER_GRAPH_HPP

#include "base_ui.hpp" // TODO For TimeRange, should be moved somewhere else
#include "jsonxx.h" // TODO don't like it :(

#include <hawktracer/base_types.h>

#include <unordered_map>
#include <string>
#include <vector>

namespace HawkTracer
{
namespace viewer
{

enum class GraphType
{
    XY,
    CALLSTACK
};

class Graph
{
public:
    using Id = std::string;
    using FieldMapping = std::unordered_map<std::string, std::string>;

    Graph(HT_EventKlassId klass_id, std::string graph_id) :
        _klass_id(klass_id), _graph_id(std::move(graph_id))
    {
    }

    HT_EventKlassId get_klass_id() const { return _klass_id; }
    Id get_id() const { return _graph_id; }

    virtual jsonxx::Object create_graph_data(const std::vector<EventRef>& events, TimeRange time_range, size_t canvas_size) = 0;

    static std::unique_ptr<Graph> create(GraphType type, HT_EventKlassId klass_id, std::string graph_id, const FieldMapping& mapping);

protected:
    const HT_EventKlassId _klass_id;
    const std::string _graph_id;
};

} // viewer
} // HawkTracer

#endif // HAWKTRACER_VIEWER_GRAPH_HPP
