#ifndef HAWKTRACER_CLIENT_CALL_GRAPH_HPP
#define HAWKTRACER_CLIENT_CALL_GRAPH_HPP

#include <hawktracer/parser/event.hpp>
#include "tracepoint_map.hpp"

namespace HawkTracer
{
namespace client
{

class CallGraph
{
public:
    struct TreeNode
    {
        std::string label;
        HT_TimestampNs last_start_ts;
        HT_TimestampNs last_stop_ts;
        HT_DurationNs total_duration;
        HT_DurationNs total_children_duration;

        std::vector<std::pair<std::shared_ptr<TreeNode>, int>> children;
        std::shared_ptr<TreeNode> parent;

        TreeNode(std::string name, HT_TimestampNs start, HT_DurationNs dur)
        {
            label = name;
            last_start_ts = start;
            last_stop_ts = start + dur;
            total_duration = dur;
            total_children_duration = 0u;
        }
    };


    std::unordered_map<HT_ThreadId, std::vector<std::pair<std::shared_ptr<TreeNode>, int>>> root_calls;

    void make(std::vector<std::pair<HT_ThreadId, TreeNode>>& events);

private:
    std::unordered_map<HT_ThreadId, std::shared_ptr<TreeNode>> _calls;

    void _add_event(HT_ThreadId thread_id, 
                    std::string label,
                    HT_TimestampNs start_ts,
                    HT_DurationNs duration);
    void _add_new_calltree(HT_ThreadId thread_id, 
                           std::string label,
                           HT_TimestampNs start_ts,
                           HT_DurationNs duration);
};

} // namespace client
} // namespace HawkTracer

#endif //HAWKTRACER_CLIENT_CALL_GRAPH_HPP
