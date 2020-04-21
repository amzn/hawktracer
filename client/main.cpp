#include "callgrind_converter.hpp"
#include "chrome_trace_converter.hpp"

#include <hawktracer/parser/protocol_reader.hpp>
#include <hawktracer/parser/make_unique.hpp>

#include <hawktracer/client_utils/command_line_parser.hpp>
#include <hawktracer/client_utils/stream_factory.hpp>
#include <iostream>
#include <map>

using namespace HawkTracer;
using ClientUtils::CommandLineParser;

std::string create_output_path(const char* path)
{
    time_t rawtime;
    time(&rawtime);
    struct tm* time_info = localtime (&rawtime);
    char file_name_buffer[128];
    strftime(file_name_buffer, 128, path, time_info);

    return file_name_buffer;
}

std::string supported_formats(std::map<std::string, std::unique_ptr<ClientUtils::Converter>>& formats)
{
    std::string supported_formats;
    for (auto& format : formats)
    {
        supported_formats += format.first + " ";
    }
    return supported_formats;
}

void init_supported_formats(std::map<std::string, std::unique_ptr<ClientUtils::Converter>>& formats)
{
    formats["chrome-tracing"] = parser::make_unique<client::ChromeTraceConverter>();
    formats["callgrind"] = parser::make_unique<client::CallgrindConverter>();
}

int main(int argc, char** argv)
{
    std::map<std::string, std::unique_ptr<ClientUtils::Converter>> formats;

    init_supported_formats(formats);

    CommandLineParser parser("--", argv[0]);
    parser.register_option("format", CommandLineParser::OptionInfo(false, false, "Output format. Supported formats: " + supported_formats(formats)));
    parser.register_option("output", CommandLineParser::OptionInfo(false, false, "Output file"));
    parser.register_option("source", CommandLineParser::OptionInfo(false, true, "Data source description (either filename, or server address)"));
    parser.register_option("map", CommandLineParser::OptionInfo(false, false, "Comma-separated list of map files"));
    parser.register_option("help", CommandLineParser::OptionInfo(true, false, "Print this help"));

    if (!parser.parse(argc, argv) || parser.has_value("help"))
    {
        parser.print_help(std::cerr);
        return 1;
    }

    std::string format = parser.get_value("format", "chrome-tracing");
    std::string output_path = parser.get_value("output", "hawktracer-trace-%d-%m-%Y-%H_%M_%S.httrace");
    std::string source = parser.get_value("source", "");
    std::string map_files = parser.get_value("map", "");

    std::unique_ptr<parser::Stream> stream = ClientUtils::make_stream_from_string(source);
    if (!stream)
    {
        return 1;
    }

    bool is_stream_continuous = stream->is_continuous();

    parser::KlassRegister klass_register;
    parser::ProtocolReader reader(&klass_register, std::move(stream), true);
    std::string out_file = create_output_path(output_path.c_str());

    auto converter = formats.find(format);
    if (converter == formats.end())
    {
        std::cerr << "Unknown format: " << format << ". Supported formats are: " << supported_formats(formats) << std::endl;
        return 1;
    }
    if (converter->second->init(out_file))
    {
        reader.register_events_listener([&converter] (const parser::Event& event) { converter->second->process_event(event); });
    }
    else
    {
        std::cerr << "Can't open output file" << std::endl;
        return 1;
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

    if (is_stream_continuous)
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
    converter->second->stop();

    return 0;
}
