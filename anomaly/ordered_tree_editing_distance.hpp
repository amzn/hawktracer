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
    unsigned int get_distance();

private:
    bool _computed = false;
    unsigned int _result;
    std::shared_ptr<Config> _config;
    std::shared_ptr<TreeNode> _src_tree;
    std::shared_ptr<TreeNode> _dst_tree;
    std::vector<std::string> _post_order_src;
    std::vector<std::string> _post_order_dst;
    std::vector<unsigned int> _lr_key_roots_src;
    std::vector<unsigned int> _lr_key_roots_dst;
    std::vector<unsigned int> _left_most_leaf_src;
    std::vector<unsigned int> _left_most_leaf_dst;

    unsigned int _zhang_shasha_algorithm();
    unsigned int _tree_dist(unsigned int src_node,
                            unsigned int dst_node,
                            std::vector<std::vector<unsigned int>>& treedist,
                            std::vector<std::vector<bool>>& computed);
};

} // namespace anomaly
} // namespace HawkTracer

#endif // HAWKTRACER_ORDRED_TREE_EDITING_DISTANCE
