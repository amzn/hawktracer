#ifndef HAWKTRACER_ANOMALY_PATTERNS_HPP
#define HAWKTRACER_ANOMALY_PATTERNS_HPP

#include "tree_file_loader.hpp"

#include <fstream>

namespace HawkTracer
{
namespace anomaly
{

class Patterns
{
public:
    bool init(const std::string& file_name);
    std::vector<std::pair<std::shared_ptr<client::CallGraph::TreeNode>, int>> get_patterns();

private:
    TreeFileLoader _file_loader;
    std::ifstream file;
    std::vector<std::pair<std::shared_ptr<client::CallGraph::TreeNode>, int>> _patterns;
};

} // namespace anomaly
} // namespace HawkTracer

#endif // HAWKTRACER_ANOMALY_PATTERNS_HPP
