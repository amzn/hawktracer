#include "graph.hpp"
#include "xy_graph.hpp"

#include <parser/make_unique.hpp>

namespace HawkTracer
{
namespace viewer
{

std::unique_ptr<Graph> Graph::create(GraphType type, HT_EventKlassId klass_id, std::string graph_id, const FieldMapping& mapping)
{
    switch (type)
    {
    case GraphType::XY:
        return parser::make_unique<XYGraph>(klass_id, std::move(graph_id), mapping);
    default:
        return nullptr;
    }
}

} // viewer
} // HawkTracer
