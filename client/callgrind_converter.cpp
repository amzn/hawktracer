#include "callgrind_converter.hpp"

namespace HawkTracer
{
namespace client
{

CallgrindConverter::CallgrindConverter(std::shared_ptr<TracepointMap> tracepoint_map) :
    IConverter(tracepoint_map)
{
}

CallgrindConverter::~CallgrindConverter()
{
    uninit();
}

bool CallgrindConverter::init(const std::string& file_name)
{
    file.open(file_name);
    if (file.is_open())
    {
        file << "# callgrind format\n";
        return true;
    }
    return false;
}


void CallgrindConverter::uninit()
{
    if (file.is_open())
    {
        file.close();
    }
}

void CallgrindConverter::process_event(const parser::Event& event)
{
}

} // namespace client
} // namespace HawkTracer
