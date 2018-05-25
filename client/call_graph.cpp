#include "call_graph.hpp"

#include <algorithm>

namespace HawkTracer
{
namespace client
{

std::vector<std::pair<std::shared_ptr<CallGraph::TreeNode>, int>> 
    CallGraph::make(std::vector<NodeData>& events)
{
    std::sort(events.begin(), events.end(), 
            [](const NodeData& e1, const NodeData& e2){
                return e1.start_ts < e2.start_ts;
            });
    for (const auto& event : events)
    {
        _add_event(event);
    }

    return _root_calls;
}

std::shared_ptr<CallGraph::TreeNode> CallGraph::_add_new_call(const NodeData& node_data,
                                                              const std::shared_ptr<TreeNode>& parent,
                                                              std::vector<std::pair<std::shared_ptr<TreeNode>, int>>& calls)
{
    std::string label = node_data.label;
    auto call = std::find_if(calls.begin(), calls.end(),
            [&label](const std::pair<std::shared_ptr<TreeNode>, int>& call) {
                return call.first->data.label == label;
            });
    if (call != calls.end())
    {
        call->first->total_duration += node_data.get_duration();
        call->first->data = node_data;
        ++call->second;

        return call->first;
    }
    else
    {
        std::shared_ptr<TreeNode> event_node = std::make_shared<TreeNode>(node_data);
        event_node->parent = parent;
        calls.emplace_back(event_node, 1);

        return event_node;
    }
}

void CallGraph::_add_new_calltree(const NodeData& node_data)
{
    _current_call = _add_new_call(node_data, nullptr, _root_calls);
}

void CallGraph::_add_new_event_call(const std::shared_ptr<TreeNode>& caller,
                                    const NodeData& node_data)
{
    caller->total_children_duration += node_data.get_duration();
    _current_call = _add_new_call(node_data, caller, caller->children);
}

bool CallGraph::_try_add_event_to_existing_calltree(const NodeData& node_data)
{
    while(_current_call) 
    {
        bool current_call_contains_new_event = 
            _current_call->data.start_ts <= node_data.start_ts && node_data.stop_ts <= _current_call->data.stop_ts;

        if (current_call_contains_new_event)
        {
            _add_new_event_call(_current_call, node_data);
            return true;
        }
        else 
        {
            _current_call = _current_call->parent.lock();
        }
    }
    return false;
}

void CallGraph::_add_event(const NodeData& node_data)
{
    bool added = _try_add_event_to_existing_calltree(node_data);
    if (!added) 
    {
        _add_new_calltree(node_data);
    }
}

} // namespace client
} // namespace HawkTracer
