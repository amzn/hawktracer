#include "call_graph.hpp"

#include <algorithm>

namespace HawkTracer
{
namespace client
{

std::unordered_map<HT_ThreadId, std::vector<std::pair<std::shared_ptr<CallGraph::TreeNode>, int>>> 
    CallGraph::make(std::vector<std::pair<HT_ThreadId, NodeData>>& events)
{
    std::sort(events.begin(), events.end(), 
            [](const std::pair<HT_ThreadId, NodeData>& e1, const std::pair<HT_ThreadId, NodeData>& e2){
                return e1.second.start_ts < e2.second.start_ts;
            });
    for (const auto& event : events)
    {
        _add_event(event.first, event.second);
    }

    return _root_calls;
}

void CallGraph::_add_new_calltree(HT_ThreadId thread_id,
                                  NodeData& node_data)
{
    auto& root_calls_for_thread_id = _root_calls[thread_id];
    std::string label = node_data.label;
    auto call = std::find_if(root_calls_for_thread_id.begin(), root_calls_for_thread_id.end(),
            [&label](const std::pair<std::shared_ptr<TreeNode>, int>& call) {
                return call.first.get()->data.label == label;
            });
    if (call != root_calls_for_thread_id.end())
    {
        call->first->total_duration += node_data.get_duration();
        call->first->data = node_data;
        ++call->second;
    }
    else
    {
        _calls[thread_id] = std::make_shared<TreeNode>(node_data);
        root_calls_for_thread_id.emplace_back(_calls[thread_id], 1);
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

bool CallGraph::_try_add_event_to_existing_calltree(HT_ThreadId thread_id,
                                                    NodeData& node_data)
{
    std::shared_ptr<TreeNode> previous_event = _calls[thread_id];
    bool event_added;

    while(previous_event.get() && !event_added)
    {
        bool previous_event_contains_current_event = 
            previous_event->data.start_ts <= node_data.start_ts && node_data.stop_ts <= previous_event->data.stop_ts;

        if (previous_event_contains_current_event)
        {
            _calls[thread_id] =  _add_new_event_call(previous_event, node_data);
            event_added = true;
        }
        else 
        {
            previous_event = previous_event->parent;
        }
    }
    return event_added;
}

void CallGraph::_add_event(HT_ThreadId thread_id,
                           NodeData node_data)
{
    auto thread_calls = _calls.find(thread_id);

    if (thread_calls == _calls.end())
    {
        _add_new_calltree(thread_id, node_data);
    }
    else
    {
        bool added = _try_add_event_to_existing_calltree(thread_id, node_data);
        if (!added) 
        {
            _add_new_calltree(thread_id, node_data);
        }
    }
}

} // namespace client
} // namespace HawkTracer
