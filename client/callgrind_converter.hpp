#ifndef HAWKTRACER_CLIENT_CALLGRIND_CONVERTER_HPP
#define HAWKTRACER_CLIENT_CALLGRIND_CONVERTER_HPP

#include <hawktracer/parser/event.hpp>
#include "iconverter.hpp"
#include "tracepoint_map.hpp"

#include <fstream>
#include <stack>

namespace HawkTracer
{
namespace client
{

class CallgrindConverter : public IConverter
{
public:
    CallgrindConverter(std::shared_ptr<TracepointMap> tracepoint_map);
    ~CallgrindConverter();

    bool init(const std::string& file_name);
    void uninit();
    void process_event(const parser::Event& event);

private:
    std::ofstream file;
};

} // namespace client
} // namespace HawkTracer

#endif // HAWKTRACER_CLIENT_CALLGRIND_CONVERTER_HPP
