#ifndef HAWKTRACER_CLIENT_CALLGRIND_CONVERTER_HPP
#define HAWKTRACER_CLIENT_CALLGRIND_CONVERTER_HPP

#include <hawktracer/client_utils/converter.hpp>
#include <hawktracer/parser/event.hpp>
#include "call_graph.hpp"

#include <fstream>

namespace HawkTracer
{
namespace client
{

class CallgrindConverter : public ClientUtils::Converter
{
public:
    ~CallgrindConverter() override;

    bool init(const std::string& file_name) override;
    void process_event(const parser::Event& event) override;
    void stop() override;

private:
    const std::string _callgrind_header = "# callgrind format";
    std::string _file_name;
    std::unordered_map<HT_ThreadId, std::vector<CallGraph::NodeData>> _events;

    void _print_function(std::ofstream& file, std::shared_ptr<CallGraph::TreeNode> node);
};

} // namespace client
} // namespace HawkTracer

#endif // HAWKTRACER_CLIENT_CALLGRIND_CONVERTER_HPP
