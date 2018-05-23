#ifndef HAWKTRACER_CLIENT_CALLGRIND_CONVERTER_HPP
#define HAWKTRACER_CLIENT_CALLGRIND_CONVERTER_HPP

#include <hawktracer/parser/event.hpp>
#include "converter.hpp"
#include "call_graph.hpp"
#include "tracepoint_map.hpp"

#include <fstream>

namespace HawkTracer
{
namespace client
{

class CallgrindConverter : public Converter
{
public:
    ~CallgrindConverter() override;

    bool init(const std::string& file_name) override;
    void process_event(const parser::Event& event) override;
    void stop() override;

private:
    const std::string callgrind_header = "# callgrind format";
    std::string _file_name;
    std::vector<std::pair<HT_ThreadId, CallGraph::NodeData>> _events;
    CallGraph _call_graph;

    void _print_function(std::ofstream& file, std::shared_ptr<CallGraph::TreeNode> node, std::string label);
};

} // namespace client
} // namespace HawkTracer

#endif // HAWKTRACER_CLIENT_CALLGRIND_CONVERTER_HPP
