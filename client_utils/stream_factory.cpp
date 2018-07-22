#include "hawktracer/client_utils/stream_factory.hpp"
#include "hawktracer/client_utils/tcp_client_stream.hpp"

#include "hawktracer/parser/file_stream.hpp"
#include "hawktracer/parser/make_unique.hpp"

#include <fstream>
#include <iostream>

namespace HawkTracer
{
namespace ClientUtils
{

static bool scan_ip_address(const std::string& address, std::string& out_ip, uint16_t& out_port, uint16_t default_port)
{
    unsigned n1, n2, n3, n4, port;
    int num = sscanf(address.c_str(), "%u.%u.%u.%u:%u", &n1, &n2, &n3, &n4, &port);

    if (num >= 4)
    {
        out_ip = address.substr(0, address.find(':'));
        out_port = (num == 5) ? (uint16_t)port : default_port;
        return true;
    }
    return false;
}

static bool file_exists(const char* name)
{
    std::ifstream f(name);
    return f.good();
}

std::unique_ptr<parser::Stream> make_stream_from_string(const std::string& source_description)
{
    std::string ip;
    uint16_t port;

    if (file_exists(source_description.c_str()))
    {
        return parser::make_unique<parser::FileStream>(source_description);
    }
    else if (scan_ip_address(source_description, ip, port, 8765))
    {
        return parser::make_unique<TCPClientStream>(ip, port);
    }
    else
    {
        std::cerr << "Invalid stream: " << source_description << std::endl;
        std::cerr << "Stream must be either a file name, or IP address (e.g. 127.0.0.1:8765)" << std::endl;
        return nullptr;
    }
}

} // namespace ClientUtils
} // namespace HawkTracer
