#include "callgrind_converter.hpp" 
#include "chrome_trace_converter.hpp" 
#include "tcp_client_stream.hpp" 
#include <hawktracer/parser/file_stream.hpp>
#include <hawktracer/parser/protocol_reader.hpp>
#include <hawktracer/parser/make_unique.hpp>

#include <iostream>
#include <cstring>
#include <map>

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
        out_port = (num == 5) ? (uint16_t)port : default_port;
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

std::string supported_formats(std::map<std::string, std::unique_ptr<client::Converter>>& formats)
{
    std::string supported_formats;
    for (auto& format : formats)
    {
        supported_formats += format.first + " ";
    }
    return supported_formats;
}

void print_help(const char* app_name, std::map<std::string, std::unique_ptr<client::Converter>>& formats)
{
    std::cout << "usage: " << app_name << " [OPTION]..." << std::endl
              << "  --format    supported formats are: " << supported_formats(formats) << "(default is chrome-tracing)" << std::endl
              << "  --source    data source description (either filename, or server address)" << std::endl
              << "  --output    an output Chrome Tracing Json file" << std::endl
              << "  --map       comma-separated list of map files" << std::endl
              << "  --help      print this text" << std::endl;
}

void init_supported_formats(std::map<std::string, std::unique_ptr<client::Converter>>& formats)
{
    formats["chrome-tracing"] = parser::make_unique<client::ChromeTraceConverter>();
    formats["callgrind"] = parser::make_unique<client::CallgrindConverter>();
}

int main(int argc, char** argv)
{
    std::string format = "chrome-tracing";
    std::string output_path = "hawktracer-trace-%d-%m-%Y-%H_%M_%S.httrace";
    std::string source;
    std::string map_files;
    std::map<std::string, std::unique_ptr<client::Converter>> formats;

    init_supported_formats(formats);
    
    for (int i = 1; i < argc; i++)
    {
        if (strcmp(argv[i], "--format") == 0 && i < argc - 1)
        {
            format = argv[++i];
        }
        else if (strcmp(argv[i], "--output") == 0 && i < argc - 1)
        {
            output_path = argv[++i];
        }
        else if (strcmp(argv[i], "--source") == 0 && i < argc - 1)
        {
            source = argv[++i];
        }
        else if (strcmp(argv[i], "--map") == 0 && i < argc - 1)
        {
            map_files = argv[++i];
        }
        else
        {
            int ret = 0;
            if (strcmp(argv[i], "--help") != 0)
            {
                std::cerr << "unrecognized option '" << argv[i] << "'" << std::endl;
                ret = 1;
            }
            print_help(argv[0], formats);
            return ret;
        }
    }

    if (source.empty())
    {
        std::cerr << "--source parameter is mandatory" << std::endl;
        print_help(argv[0], formats);
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

    parser::KlassRegister klass_register;
    parser::ProtocolReader reader(&klass_register, std::move(stream), true);
    std::string out_file = create_output_path(output_path.c_str());

    auto converter = formats.find(format);
    if (converter == formats.end())
    {
        std::cerr << "Unknown format: " << format << ". Supported formats are: " << supported_formats(formats) << std::endl;
        return 1;
    }
    else
    {
        auto& converter = formats[format];
        if (converter->init(out_file))
        {
            reader.register_events_listener([&converter] (const parser::Event& event) { converter->process_event(event); });
        }
        else
        {
            std::cerr << "Can't open output file" << std::endl;
            return 1;
        }
    }

    if (map_files.empty())
    {
        std::cerr << "map not specified... profiler will use numbers instead of human-readable names" << std::endl;
    }
    else
    {
        bool map_set = formats[format]->set_tracepoint_map(map_files);
        if (!map_set)
        {
            std::cerr << "Map could not be set" << std::endl;
        }
    }

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
    formats[format]->stop();
    return 0;
}
