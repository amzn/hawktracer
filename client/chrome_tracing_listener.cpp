#include "chrome_tracing_listener.hpp"

namespace HawkTracer
{
namespace client
{

ChromeTraceListener::~ChromeTraceListener()
{
    uninit();
}

bool ChromeTraceListener::init(const std::string& file_name)
{
    file.open(file_name);
    if (file.is_open())
    {
        file << "{\"traceEvents\": [ {} ";
        return true;
    }
    return false;
}

void ChromeTraceListener::uninit()
{
    if (file.is_open())
    {
        file << "]}";
        file.close();
    }
}

void ChromeTraceListener::process_event(const parser::Event& event)
{
    std::string label;
    if (event.has_value("label"))
    {
        label = event.get_value<char*>("label");
    }
    else if (event.has_value("name"))
    {
        label = event.get_value<char*>("name");
    }
    else
    {
        return;
    }
    file << ",{\"name\": \"" << label
         << "\", \"ph\": \"X\", \"ts\": " << event.get_value<uint64_t>("timestamp")
         << ", \"dur\": " << event.get_value_or_default<uint64_t>("duration", 0u)
         << ", \"pid\": 0, \"tid\": " << event.get_value_or_default<uint32_t>("thread_id", 0)
         << "}";
}

} // namespace client
} // namespace HawkTracer
