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

void CallGraph::_add_new_calltree(NodeData& node_data)
{
    std::string label = node_data.label;
    auto call = std::find_if(_root_calls.begin(), _root_calls.end(),
            [&label](const std::pair<std::shared_ptr<TreeNode>, int>& call) {
                return call.first.get()->data.label == label;
            });
    if (call != _root_calls.end())
    {
        call->first->total_duration += node_data.get_duration();
        call->first->data = node_data;
        ++call->second;
        _current_call = call->first;
    }
    else
    {
        _current_call = std::make_shared<TreeNode>(node_data);
        _root_calls.emplace_back(_current_call, 1);
    }

}

std::shared_ptr<CallGraph::TreeNode> CallGraph::_add_new_event_call(std::shared_ptr<TreeNode>& caller,
                                                                    NodeData& node_data)
{
    caller->total_children_duration += node_data.get_duration();

    std::string label = node_data.label;
    auto call = std::find_if(caller->children.begin(), caller->children.end(),
            [&label](const std::pair<std::shared_ptr<TreeNode>, int>& child) {
                return child.first.get()->data.label == label;
            });
    if (call != caller->children.end())
    {
        call->first->total_duration += node_data.get_duration();
        call->first->data = node_data;
        ++call->second;

        return call->first;
    }
    else
    {
        auto eventNode = std::make_shared<TreeNode>(node_data);
        eventNode->parent = caller;
        caller->children.emplace_back(eventNode, 1);

        return eventNode;
    }
}

bool CallGraph::_try_add_event_to_existing_calltree(NodeData& node_data)
{
    bool event_added;

    while(_current_call.get() && !event_added)
    {
        bool current_call_contains_new_event = 
            _current_call->data.start_ts <= node_data.start_ts && node_data.stop_ts <= _current_call->data.stop_ts;

        if (current_call_contains_new_event)
        {
            _current_call = _add_new_event_call(_current_call, node_data);
            event_added = true;
        }
        else 
        {
            _current_call = _current_call->parent;
        }
    }
    return event_added;
}

void CallGraph::_add_event(NodeData node_data)
{
    bool added = _try_add_event_to_existing_calltree(node_data);
    if (!added) 
    {
        _add_new_calltree(node_data);
    }
}

} // namespace client
} // namespace HawkTracer
