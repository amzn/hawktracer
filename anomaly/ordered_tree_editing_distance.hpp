#ifndef HAWKTRACER_ANOMALY_ORDERED_TREE_EDITING_DISTANCE_HPP
#define HAWKTRACER_ANOMALY_ORDERED_TREE_EDITING_DISTANCE_HPP

#include "config.hpp"

#include <client/call_graph.hpp>

#include <memory>

using TreeNode = HawkTracer::client::CallGraph::TreeNode;

namespace HawkTracer
{
namespace anomaly
{

class OrderedTreeEditingDistance
{
public:
    OrderedTreeEditingDistance(std::shared_ptr<Config> config,
                               std::shared_ptr<TreeNode> src_tree,
                               std::shared_ptr<TreeNode> dst_tree);
    int get();

private:
    bool _computed = false;
    int _result;
    std::shared_ptr<Config> _config;
    std::shared_ptr<TreeNode> _src_tree;
    std::shared_ptr<TreeNode> _dst_tree;
    std::vector<std::string> _post_order_src;
    std::vector<std::string> _post_order_dst;
    std::vector<int> _lr_key_roots_src;
    std::vector<int> _lr_key_roots_dst;
    std::vector<int> _left_most_leaf_src;
    std::vector<int> _left_most_leaf_dst;

    int _zhang_shasha_algorithm();
    int _tree_dist(int src_node,
                   int dst_node,
                   std::vector<std::vector<int>>& treedist,
                   std::vector<std::vector<bool>>& computed);
};

} // namespace anomaly
} // namespace HawkTracer

#endif // HAWKTRACER_ORDRED_TREE_EDITING_DISTANCE
