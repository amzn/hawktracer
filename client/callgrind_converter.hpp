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
    CallgrindConverter(std::vector<std::string> map_files) : Converter(std::move(map_files)) {}
    ~CallgrindConverter() override;

    bool init(const std::string& file_name) override;
    void process_event(const parser::Event& event, const std::string& reader_id, int64_t offset_ns) override;
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
