#ifndef HAWKTRACER_ANOMALY_ORDERED_TREE_EDITING_DISTANCE_HPP
#define HAWKTRACER_ANOMALY_ORDERED_TREE_EDITING_DISTANCE_HPP

#include "config.hpp"

#include <client/call_graph.hpp>

#include <memory>

using TreeNode = HawkTracer::client::CallGraph::TreeNode;
using Mapping = std::vector<std::pair<unsigned int, unsigned int>>;

namespace HawkTracer
{
namespace anomaly
{

class OrderedTreeEditingDistance
{
public:
    enum OperationType
    {
        DELETE,
        INSERT,
        RELABEL,
        MATCH,
        SUBTREEMATCH
    };

    OrderedTreeEditingDistance(std::shared_ptr<Config> config,
                               std::shared_ptr<TreeNode> src_tree,
                               std::shared_ptr<TreeNode> dst_tree);
    unsigned int get_distance();
    Mapping get_mapping(); 
    static void compute_post_order(std::shared_ptr<TreeNode> node,
                                   std::vector<std::string>& post_order);

    static int compute_left_most_leaves(std::shared_ptr<TreeNode> node,
                                        int& index_last_node,
                                        std::vector<unsigned int>& left_most_leaf);
    static void compute_key_roots(std::shared_ptr<TreeNode> root,
                                  int& index_last_node,
                                  std::vector<unsigned int>& lr_key_roots);

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

    unsigned int _get_relabel_cost(const std::string& src, const std::string& dst);
    unsigned int _zhang_shasha_algorithm(std::shared_ptr<std::vector<std::pair<unsigned int, unsigned int>>> optimal_mapping);
    unsigned int _tree_dist(unsigned int src_node,
                            unsigned int dst_node,
                            std::vector<std::vector<int>>& treedist,
                            std::shared_ptr<std::vector<std::vector<Mapping>>> mapping);
    static void _compute_key_roots(std::shared_ptr<TreeNode> node,
                                   int& index_last_node,
                                   std::vector<unsigned int>& lr_key_roots);
    void _compute_mapping(unsigned int src_index,
                          unsigned int dst_index,
                          std::vector<std::vector<OperationType>>& forestoperation,
                          std::vector<std::vector<Mapping>>& mapping);
};

} // namespace anomaly
} // namespace HawkTracer

#endif // HAWKTRACER_ORDRED_TREE_EDITING_DISTANCE
