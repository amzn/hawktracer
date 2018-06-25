#include "ordered_tree_editing_distance.hpp"
#include "pattern_matching.hpp"

namespace HawkTracer
{
namespace anomaly
{

PatternMatching::PatternMatching(std::shared_ptr<Config> config,
                                 std::shared_ptr<Graphs> patterns) :
    _config(std::move(config)),
    _patterns(std::move(patterns))
{
    _edit_distance.resize(_patterns->get_trees().size());
}

std::vector<unsigned int> PatternMatching::get_matching_scores(std::shared_ptr<TreeNode> tree)
{
    _compute_matching_scores(tree);
    return _edit_distance;
}


void PatternMatching::_compute_matching_scores(std::shared_ptr<TreeNode> tree)
{
    const auto& patterns = _patterns->get_trees();
    for (size_t i = 0 ; i < patterns.size(); ++i)
    {
        OrderedTreeEditingDistance distance(_config, tree, patterns[i].first);
        _edit_distance[i] = distance.get_distance(); 
    }
}

} // namespace anomaly
} // namespace HawkTracer
