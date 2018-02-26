#include "chrome_tracing_listener.hpp"
#include "tcp_client_stream.hpp"
#include "parser/debug_event_listener.hpp"
#include "parser/file_stream.hpp"
#include "parser/protocol_reader.hpp"
#include "parser/event.hpp"
#include "parser/make_unique.hpp"

#include <iostream>
#include <cstring>

using namespace HawkTracer;

bool file_exists(const char* name)
{
    std::ifstream f(name);
    return f.good();
}

bool scan_ip_address(const std::string& address, std::string& out_ip, uint16_t& out_port, uint16_t default_port)
{
    unsigned n1, n2, n3, n4, port;
    int num = sscanf(address.c_str(), "%u.%u.%u.%u:%u", &n1, &n2, &n3, &n4, &port);

    if (num >= 4)
    {
        out_ip = address.substr(0, address.find(':'));
        out_port = (num == 5) ? port : default_port;
        return true;
    }
    return false;
}

std::string create_output_path(const char* path)
{
    time_t rawtime;
    time(&rawtime);
    struct tm* time_info = localtime (&rawtime);
    char file_name_buffer[128];
    strftime(file_name_buffer, 128, path, time_info);

    return file_name_buffer;
}

void print_help(const char* app_name)
{
    std::cout << "usage: " << app_name << " [OPTION]..." << std::endl
              << "  --source    data source description (either filename, or server address)" << std::endl
              << "  --output    an output Chrome Tracing Json file" << std::endl
              << "  --help      print this text" << std::endl;
}

int main(int argc, char** argv)
{
    std::string output_path = "hawktracer-trace-%d-%m-%Y-%H_%M_%S.httrace";
    std::string source;

    for (int i = 1; i < argc; i++)
    {
        if (strcmp(argv[i], "--output") == 0 && i < argc - 1)
        {
            output_path = argv[++i];
        }
        else if (strcmp(argv[i], "--source") == 0 && i < argc - 1)
        {
            source = argv[++i];
        }
        else if (strcmp(argv[i], "--map") == 0 && i < argc - 1)
        {
            // TODO
            std::cerr << "--map parameter is not supported yet" << std::endl;
            i++;
        }
        else
        {
            int ret = 0;
            if (strcmp(argv[i], "--help") != 0)
            {
                std::cerr << "unrecognized option '" << argv[i] << "'" << std::endl;
                ret = 1;
            }
            print_help(argv[0]);
            return ret;
        }
    }

    if (source.empty())
    {
        std::cerr << "--source parameter is mandatory" << std::endl;
        print_help(argv[0]);
        return 1;
    }

    std::unique_ptr<parser::Stream> stream;
    std::string ip;
    uint16_t port;
    bool is_network = false;
    if (file_exists(source.c_str()))
    {
        stream = parser::make_unique<parser::FileStream>(source);
    }
    else if (scan_ip_address(source, ip, port, 8765))
    {
        stream = parser::make_unique<client::TCPClientStream>(ip, port);
        is_network = true;
    }
    else
    {
        std::cerr << "Invalid stream: " << source << std::endl;
        return 1;
    }

    parser::ProtocolReader reader(std::move(stream), true);
    client::ChromeTraceListener chrome_listener;
    parser::DebugEventListener l;
    std::string out_file = create_output_path(output_path.c_str());
    chrome_listener.init(out_file);
    //reader.register_events_listener([&chrome_listener] (const parser::Event& event) { chrome_listener.process_event(event); });
    reader.register_events_listener([&l] (const parser::Event& event) { l(event); });

    std::cout << "Output will be written to a file: " << out_file << std::endl;

    if (!reader.start())
    {
        std::cerr << "Error on starting the reader!!!" << std::endl;
        return 1;
    }

    if (is_network)
    {
        std::cout << "Hit [Enter] to finish the trace..." << std::endl;
        getchar();
        reader.stop();
    }
    else
    {
        std::cout << "Processing the file..." << std::endl;
    }

    reader.wait_for_complete();
    reader.stop();

    std::cout << "Trace file has been saved to: " << out_file << std::endl;

    return 0;
}
