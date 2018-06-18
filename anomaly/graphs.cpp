#include "graphs.hpp"

namespace HawkTracer
{
namespace anomaly
{

bool Graphs::load_from_file(const std::string& file_name)
{
    if (!_file_loader.init(file_name))
    {
        return false;
    }
    
    _trees = _file_loader.get_trees();
    return true;
}

std::vector<std::pair<std::shared_ptr<client::CallGraph::TreeNode>, int>> Graphs::get_trees()
{
    return _trees;
}

} // namespace anomaly
} // namespace HawkTracer

