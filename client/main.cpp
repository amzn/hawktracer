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

struct SourceDescription
{
    std::string id;
    int64_t offset_ns;
    bool is_stream_continuous;
    parser::KlassRegister klass_register;
    std::unique_ptr<parser::ProtocolReader> reader;
};

int main(int argc, char** argv)
{
    FormatMap formats;

    init_supported_formats(formats);

    cxxopts::Options options("hawktracer-converter", "Converts HawkTracer stream to one of the well-known tracing formats.");

    options.add_options()
        ("f,format", "Output format. Supported formats: " + supported_formats(formats), cxxopts::value<std::string>()->default_value("chrome-tracing"))
        ("o,output", "Output file", cxxopts::value<std::string>()->default_value("hawktracer-trace-%d-%m-%Y-%H_%M_%S.httrace"))
        ("s,source", "Data source description (either filename, or server address)", cxxopts::value<std::vector<std::string>>()->default_value(""))
        ("source-id", "Data source identifier (corresponding to source)", cxxopts::value<std::vector<std::string>>()->default_value(""))
        ("source-offset", "Data source offset in nanoseconds (corresponding to source)", cxxopts::value<std::vector<int64_t>>()->default_value(""))
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

    auto source_values = result["source"].as<std::vector<std::string>>();
    auto source_ids = result["source-id"].as<std::vector<std::string>>();
    auto source_offsets = result["source-offset"].as<std::vector<int64_t>>();
    std::vector<std::unique_ptr<SourceDescription>> sources;

    for (size_t i = 0; i < source_values.size(); i++)
    {
        std::string id = source_ids.size() <= i ? "source_" + std::to_string(i) : source_ids[i];
        int64_t offset = source_offsets.size() <= i ? 0 : source_offsets[i];
        std::unique_ptr<parser::Stream> stream = ClientUtils::make_stream_from_string(source_values[i]);
        if (!stream)
        {
            return 1;
        }
        auto desc = parser::make_unique<SourceDescription>();
        desc->id = id;
        desc->offset_ns = offset;
        desc->is_stream_continuous = stream->is_continuous();
        desc->reader = parser::make_unique<parser::ProtocolReader>(&desc->klass_register, std::move(stream), true);
        sources.push_back(std::move(desc));
    }


    std::string format = result["format"].as<std::string>();
    std::string output_path = result["output"].as<std::string>();
    auto map_files = result["map"].as<std::vector<std::string>>();


    bool is_stream_continuous = false;
    for (const auto& src : sources)
    {
        is_stream_continuous = is_stream_continuous || src->is_stream_continuous;
    }

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
        for (const auto& src : sources)
        {
            auto offset = src->offset_ns;
            auto id = src->id;
            src->reader->register_events_listener([&converter, offset, id] (const parser::Event& event) { converter->process_event(event, id, offset); });
        }
    }
    else
    {
        std::cerr << "Can't open output file" << std::endl;
        return 1;
    }

    std::cout << "Output will be written to a file: " << out_file << std::endl;

    for (const auto& src : sources)
    {
        if (!src->reader->start())
        {
            std::cerr << "Error on starting the '" << src->id << "' reader!!!" << std::endl;
            return 1;
        }
    }

    if (is_stream_continuous)
    {
        std::cout << "Hit [Enter] to finish the trace..." << std::endl;
        getchar();
        for (const auto& src : sources)
        {
            src->reader->stop();
        }
    }
    else
    {
        std::cout << "Processing the file..." << std::endl;
    }

    for (const auto& src : sources)
    {
        src->reader->wait_for_complete();
    }
    for (const auto& src : sources)
    {
        src->reader->stop();
    }
    converter->stop();

    return 0;
}
