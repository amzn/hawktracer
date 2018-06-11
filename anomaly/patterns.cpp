#ifndef HAWKTRACER_ANOMALY_PATTERNS_CPP
#define HAWKTRACER_ANOMALY_PATTERNS_CPP

#include "patterns.hpp"

namespace HawkTracer
{
namespace anomaly
{

bool Patterns::init(const std::string& file_name)
{
    if (!_file_loader.init(file_name))
    {
        return false;
    }
    
    _patterns = _file_loader.get_trees();
    return true;
}

std::vector<std::pair<std::shared_ptr<client::CallGraph::TreeNode>, int>> Patterns::get_patterns()
{
    return _patterns;
}

} // namespace anomaly
} // namespace HawkTracer

#endif // HAWKTRACER_ANOMALY_PATTERNS_CPP
