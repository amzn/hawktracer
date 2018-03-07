#include "file_stream.hpp"

namespace HawkTracer {
namespace parser {

FileStream::FileStream(std::string file_name) :
    _file_name(std::move(file_name))
{
}

FileStream::~FileStream()
{
    stop();
}

bool FileStream::start()
{
    _file = fopen(_file_name.c_str(), "rb");
    return _file != nullptr;
}

void FileStream::stop()
{
    if (_file)
    {
        fclose(_file);
        _file = nullptr;
    }
}

bool FileStream::read_data(char* buff, size_t size)
{
    fread(buff, 1, size, _file);
    return !feof(_file);
}

int FileStream::read_byte()
{
    return fgetc(_file);
}

} // namespace parser
} // namespace HawkTracer
