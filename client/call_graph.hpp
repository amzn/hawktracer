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
    struct NodeData
    {
        std::string label;
        HT_TimestampNs start_ts;
        HT_TimestampNs stop_ts;

        NodeData()
        {
        }

        NodeData(std::string name, HT_TimestampNs start, HT_DurationNs dur)
        {
            label = name;
            start_ts = start;
            stop_ts = start_ts + dur;
        }

        HT_DurationNs get_duration() const
        {
            return stop_ts - start_ts;
        }
    };

    struct TreeNode
    {
        NodeData data;
        HT_DurationNs total_duration;
        HT_DurationNs total_children_duration;

        std::vector<std::pair<std::shared_ptr<TreeNode>, int>> children;
        std::shared_ptr<TreeNode> parent;

        TreeNode(NodeData node_data)
        {
            data = node_data;
            total_duration = node_data.get_duration();
            total_children_duration = 0u;
        }
    };

    std::unordered_map<HT_ThreadId, std::vector<std::pair<std::shared_ptr<TreeNode>, int>>> make(
            std::vector<std::pair<HT_ThreadId, NodeData>>& events);

private:
    // stores for eached thread a vector with all the root_calls and how many times they are called
    std::unordered_map<HT_ThreadId, std::vector<std::pair<std::shared_ptr<TreeNode>, int>>> _root_calls;

    std::unordered_map<HT_ThreadId, std::shared_ptr<TreeNode>> _calls;

    void _add_event(HT_ThreadId thread_id,
                    NodeData node_data);
    void _add_new_calltree(HT_ThreadId thread_id, 
                           NodeData& node_data);
    bool _try_add_event_to_existing_calltree(HT_ThreadId thread_id,
                                             NodeData& node_data);
    std::shared_ptr<TreeNode> _add_new_event_call(std::shared_ptr<TreeNode>& caller,
                                                  NodeData& node_data);

};

} // namespace client
} // namespace HawkTracer

#endif //HAWKTRACER_CLIENT_CALL_GRAPH_HPP
