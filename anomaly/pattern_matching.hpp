#ifndef HAWKTRACER_ANOMALY_PATTERN_MATCHING_HPP
#define HAWKTRACER_ANOMALY_PATTERN_MATCHING_HPP

#include "config.hpp"
#include "graphs.hpp"

using TreeNode = HawkTracer::client::CallGraph::TreeNode;

namespace HawkTracer
{
namespace anomaly
{

class PatternMatching
{
public:
    PatternMatching(std::shared_ptr<Config> config,
                    std::shared_ptr<Graphs> patterns);
    std::vector<unsigned int> get_matching_scores(std::shared_ptr<TreeNode> tree);

private:
    std::shared_ptr<Config> _config;
    std::shared_ptr<Graphs> _patterns;
    std::vector<unsigned int> _edit_distance;

    void _compute_matching_scores(std::shared_ptr<TreeNode> tree);
};

} // namespace anomaly
} // namespace HawkTracer

#endif // HAWKTRACER_ANOMALY_PATTERN_MATCHING_HPP
