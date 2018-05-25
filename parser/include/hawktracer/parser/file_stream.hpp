#ifndef HAWKTRACER_PARSER_FILE_STREAM_HPP
#define HAWKTRACER_PARSER_FILE_STREAM_HPP

#include <hawktracer/parser/stream.hpp>

#include <cstdio>

namespace HawkTracer {
namespace parser {

class FileStream : public Stream
{
public:
    explicit FileStream(std::string file_name);
    ~FileStream();

    bool start() override;
    void stop() override;
    bool read_data(char* buff, size_t size) override;
    int read_byte() override;

private:
    FILE* _file = nullptr;
    std::string _file_name;
};

} // namespace parser
} // namespace HawkTracer

#endif // HAWKTRACER_PARSER_FILE_STREAM_HPP
