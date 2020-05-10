#include "callgrind_converter.hpp"
#include "chrome_trace_converter.hpp"
#include "cxxopts.hpp"

#include <hawktracer/parser/protocol_reader.hpp>
#include <hawktracer/parser/make_unique.hpp>

#include <hawktracer/client_utils/stream_factory.hpp>
#include <iostream>
#include <map>

using namespace HawkTracer;

using ConverterFactory = std::function<std::unique_ptr<client::Converter>(std::vector<std::string>)>;
using FormatMap = std::map<std::string, ConverterFactory>;

std::string create_output_path(const char* path)
{
    time_t rawtime;
    time(&rawtime);
    struct tm* time_info = localtime (&rawtime);
    char file_name_buffer[128];
    strftime(file_name_buffer, 128, path, time_info);

    return file_name_buffer;
}

std::string supported_formats(FormatMap& formats)
{
    std::string supported_formats;
    for (auto& format : formats)
    {
        supported_formats += format.first + " ";
    }
    return supported_formats;
}

void init_supported_formats(FormatMap& formats)
{
    formats["chrome-tracing"] = [] (std::vector<std::string> map_files) { return parser::make_unique<client::ChromeTraceConverter>(std::move(map_files)); };
    formats["callgrind"] = [](std::vector<std::string> map_files) { return parser::make_unique<client::CallgrindConverter>(std::move(map_files)); };
}

int main(int argc, char** argv)
{
    FormatMap formats;

    init_supported_formats(formats);

    cxxopts::Options options("hawktracer-converter", "Converts HawkTracer stream to one of the well-known tracing formats.");

    options.add_options()
        ("f,format", "Output format. Supported formats: " + supported_formats(formats), cxxopts::value<std::string>()->default_value("chrome-tracing"))
        ("o,output", "Output file", cxxopts::value<std::string>()->default_value("hawktracer-trace-%d-%m-%Y-%H_%M_%S.httrace"))
        ("s,source", "Data source description (either filename, or server address)", cxxopts::value<std::string>())
        ("m,map", "Comma-separated list of map files", cxxopts::value<std::vector<std::string>>()->default_value(""))
        ("h,help", "Print this help");

    auto result = options.parse(argc, argv);

    if (result.count("help"))
    {
        std::cerr << options.help() << std::endl;
        return 1;
    }

    if (!result.count("source"))
    {
        std::cerr << "source parameter not specified" << std::endl;
        return 1;
    }

    std::string format = result["format"].as<std::string>();
    std::string output_path = result["output"].as<std::string>();
    std::string source = result["source"].as<std::string>();
    auto map_files = result["map"].as<std::vector<std::string>>();

    std::unique_ptr<parser::Stream> stream = ClientUtils::make_stream_from_string(source);
    if (!stream)
    {
        return 1;
    }

    bool is_stream_continuous = stream->is_continuous();

    parser::KlassRegister klass_register;
    parser::ProtocolReader reader(&klass_register, std::move(stream), true);
    std::string out_file = create_output_path(output_path.c_str());

    auto converter_factory = formats.find(format);
    if (converter_factory == formats.end())
    {
        std::cerr << "Unknown format: " << format << ". Supported formats are: " << supported_formats(formats) << std::endl;
        return 1;
    }

    if (map_files.empty())
    {
        std::cerr << "map not specified... profiler will use numbers instead of human-readable names" << std::endl;
    }
    auto converter = converter_factory->second(map_files);

    if (converter->init(out_file))
    {
        reader.register_events_listener([&converter] (const parser::Event& event) { converter->process_event(event); });
    }
    else
    {
        std::cerr << "Can't open output file" << std::endl;
        return 1;
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
    converter->stop();

    return 0;
}
