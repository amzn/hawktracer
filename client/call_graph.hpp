#ifndef HAWKTRACER_CLIENT_CALL_GRAPH_HPP
#define HAWKTRACER_CLIENT_CALL_GRAPH_HPP

#include <hawktracer/parser/event.hpp>

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

        NodeData(std::string name, HT_TimestampNs start, HT_DurationNs dur) :
            label(std::move(name)),
            start_ts(start),
            stop_ts(start + dur)
        {
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
        std::weak_ptr<TreeNode> parent;

        TreeNode(NodeData node_data)
        {
            data = node_data;
            total_duration = node_data.get_duration();
            total_children_duration = 0u;
        }
    };

    std::vector<std::pair<std::shared_ptr<TreeNode>, int>> make(std::vector<NodeData>& events);

private:
    //  vector with all the root calls and how many times they are called
    std::vector<std::pair<std::shared_ptr<TreeNode>, int>> _root_calls;

    std::shared_ptr<TreeNode> _current_call;

    void _add_event(const NodeData& node_data);
    void _add_new_calltree(const NodeData& node_data);
    bool _try_add_event_to_existing_calltree(const NodeData& node_data);
    void _add_new_event_call(const std::shared_ptr<TreeNode>& caller,
                             const NodeData& node_data);
    std::shared_ptr<TreeNode> _add_new_call(const NodeData& node_data,
                                            const std::shared_ptr<TreeNode>& parent,
                                            std::vector<std::pair<std::shared_ptr<TreeNode>, int>>& calls);

};

} // namespace client
} // namespace HawkTracer

#endif //HAWKTRACER_CLIENT_CALL_GRAPH_HPP
