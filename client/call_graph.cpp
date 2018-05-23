#include "call_graph.hpp"

#include <algorithm>

namespace HawkTracer
{
namespace client
{

void CallGraph::make(std::vector<std::pair<HT_ThreadId, TreeNode>>& events)
{
    std::sort(events.begin(), events.end(), 
            [](const std::pair<HT_ThreadId, TreeNode>& e1, const std::pair<HT_ThreadId, TreeNode>& e2){
                return e1.second.start_ts < e2.second.start_ts;
            });
    for (const auto& event : events)
    {
        _add_event(event.first, event.second.label, event.second.start_ts, event.second.duration);
    }
}

void CallGraph::_add_new_calltree(HT_ThreadId thread_id,
                                  std::string label,
                                  HT_TimestampNs start_ts,
                                  HT_DurationNs duration)
{
    auto& root_calls_for_thread_id = root_calls[thread_id];
    auto call = std::find_if(root_calls_for_thread_id.begin(), root_calls_for_thread_id.end(),
            [&label](const std::pair<std::shared_ptr<TreeNode>, int>& call) {
            return call.first.get()->label == label;
            });
    if (call != root_calls_for_thread_id.end())
    {
        call->first->duration += duration;
        call->first->start_ts = start_ts;
        ++call->second;
    }
    else
    {
        _calls[thread_id] = std::make_shared<TreeNode>(label, start_ts, duration);
        root_calls_for_thread_id.emplace_back(_calls[thread_id], 1);
    }

}

void CallGraph::_add_event(HT_ThreadId thread_id,
                           std::string label,
                           HT_TimestampNs start_ts,
                           HT_DurationNs duration)
{
    auto thread_calls = _calls.find(thread_id);

    if (thread_calls == _calls.end())
    {
        _add_new_calltree(thread_id, label, start_ts, duration);
    }
    else
    {
        std::shared_ptr<TreeNode> previous_event = _calls[thread_id];

        while(previous_event.get())
        {
            bool previous_event_contains_current_event = 
                previous_event->start_ts <= start_ts && start_ts + duration <= previous_event->get_stop_ts();

            if (previous_event_contains_current_event)
            {
                previous_event->total_children_duration += duration;

                auto call = std::find_if(previous_event->children.begin(), previous_event->children.end(),
                        [&label](const std::pair<std::shared_ptr<TreeNode>, int>& child) {
                            return child.first.get()->label == label;
                        });
                if (call != previous_event->children.end())
                {
                    call->first->duration += duration;
                    call->first->start_ts = start_ts;
                    ++call->second;
                    _calls[thread_id] = call->first;
                }
                else
                {
                    auto eventNode = std::make_shared<TreeNode>(label, start_ts, duration);
                    eventNode->parent = previous_event;
                    previous_event->children.emplace_back(eventNode, 1);
                    _calls[thread_id] = eventNode;
                }

                break;
            }
            else
            {
               previous_event = previous_event->parent;
            }
        }
        if (previous_event.get() == nullptr)
        {
            _add_new_calltree(thread_id, label, start_ts, duration);
        }
    }
}

} // namespace client
} // namespace HawkTracer
