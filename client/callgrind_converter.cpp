#include "callgrind_converter.hpp"

namespace HawkTracer
{
namespace client
{

CallgrindConverter::~CallgrindConverter()
{
    uninit();
}

bool CallgrindConverter::init(const std::string& file_name)
{
    _file.open(file_name);
    if (_file.is_open())
    {
        _file << "# callgrind format\n";
        return true;
    }
    return false;
}


void CallgrindConverter::uninit()
{
    if (_file.is_open())
    {
        _file.close();
    }
}

void CallgrindConverter::process_event(const parser::Event& event)
{
}

} // namespace client
} // namespace HawkTracer
