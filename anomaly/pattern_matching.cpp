#include "ordered_tree_editing_distance.hpp"
#include "pattern_matching.hpp"

namespace HawkTracer
{
namespace anomaly
{

PatternMatching::PatternMatching(std::shared_ptr<Config> config,
                                 std::shared_ptr<Patterns> patterns) :
    _config(config),
    _patterns(patterns)
{
    _edit_distance.resize(_patterns->get_patterns().size());
}

std::vector<int> PatternMatching::get_matching_scores(std::shared_ptr<TreeNode> tree)
{
    _compute_matching_scores(tree);
    return _edit_distance;
}


void PatternMatching::_compute_matching_scores(std::shared_ptr<TreeNode> tree)
{
    if (_config->get_ordered_tree())
    {
        const auto& patterns = _patterns->get_patterns();
        for (size_t i = 0 ; i < patterns.size(); ++i)
        {
            OrderedTreeEditingDistance distance(_config, tree, patterns[i].first);
            _edit_distance[i] = distance.get(); 
        }
    }
}

} // namespace anomaly
} // namespace HawkTracer
