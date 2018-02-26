#ifndef HAWKTRACER_PARSER_STREAM_HPP
#define HAWKTRACER_PARSER_STREAM_HPP

#include <string>

namespace HawkTracer
{
namespace parser
{

class Stream
{
public:
    virtual ~Stream() {}

    template<typename T>
    bool read_value(T& value)
    {
        char buffer[sizeof(T)];
        bool ok = read_data(buffer, sizeof(T));
        if (ok)
        {
            value = *((T*)buffer);
        }
        return ok;
    }

    bool read_value(uint8_t& value)
    {
        int v = read_byte();
        if (v < 0)
        {
            return false;
        }
        value = (uint8_t)v;
    }

    bool read_value(int8_t& value)
    {
        uint8_t v;
        bool ok = read_value(v);
        value = (int8_t)v;
        return ok;
    }

    bool read_string(std::string& value)
    {
        int b;
        while ((b = read_byte()) > 0)
        {
            value += (char)b;
        }
        return b == 0;
    }

    virtual int read_byte() = 0;
    virtual bool read_data(char* buff, size_t size) = 0;

    virtual bool start()
    {
        return true;
    }

    virtual void stop()
    {
    }
};

} // namespace parser
} // namespace HawkTracer

#endif // HAWKTRACER_PARSER_STREAM_HPP
