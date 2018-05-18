#include "callgrind_listener.hpp"

namespace HawkTracer
{
namespace client
{

CallgrindListener::CallgrindListener(std::unique_ptr<TracepointMap> tracepoint_map) :
    _tracepoint_map(std::move(tracepoint_map))
{
}

CallgrindListener::~CallgrindListener()
{
    uninit();
}

bool CallgrindListener::init(const std::string& file_name)
{
    file.open(file_name);
    if (file.is_open())
    {
        file << "# callgrind format\n";
        return true;
    }
    return false;
}


void CallgrindListener::uninit()
{
    if (file.is_open())
    {
        file.close();
    }
}

void CallgrindListener::process_event(const parser::Event& event)
{
}

} // namespace client
} // namespace HawkTracer
