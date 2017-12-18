#include <hawktracer/TCPClient.h>
#include <hawktracer/Timeline.h>

#include <signal.h>

#include <fstream>
#include <cstring>
#include <iostream>
#include <map>

class TracepointMapper
{
public:
    enum Category : uint32_t
    {
        Unknown = 0,
        Lua = 1,
        Native = 2,
    };

    struct MapInfo
    {
        std::string label;
        std::string category;
    };

    TracepointMapper::MapInfo get_label_info(hawktracer::Action::Label label);

    bool load_map(const std::string& map_file);

    void enable_finding_nearest_label();

private:
    std::map<hawktracer::Action::Label, MapInfo> _input_map;
    std::unordered_map<hawktracer::Action::Label, MapInfo> _cached_map;

    bool _finding_nearest_label_enabled = false;

    static std::string category_to_string(Category category);
};

TracepointMapper mapper;

bool TracepointMapper::load_map(const std::string& map_file)
{
    std::ifstream file;
    file.open(map_file);

    if (!file.is_open())
    {
        std::cerr << "Cannot open file " << map_file << std::endl;
        return false;
    }

    std::string label_str;
    hawktracer::Action::Label label;
    uint32_t category;
    while (file >> label >> category >> label_str)
    {
        MapInfo info = { label_str, category_to_string((Category)category) };

        if (_finding_nearest_label_enabled)
        {
            _input_map[label] = info;
        }
        else
        {
            _cached_map[label] = info;
        }
    }

    return true;
}

void TracepointMapper::enable_finding_nearest_label()
{
    _finding_nearest_label_enabled = true;
}

TracepointMapper::MapInfo TracepointMapper::get_label_info(hawktracer::Action::Label label)
{
    auto cached_it = _cached_map.find(label);

    if (cached_it != _cached_map.end())
    {
        return cached_it->second;
    }

    if (_finding_nearest_label_enabled)
    {
        auto it = _input_map.lower_bound(label);
        bool found = false;
        if (it->first == label)
        {
            found = true;
        }
        else if (it != _input_map.begin())
        {
            --it;
            found = true;
        }

        if (found)
        {
            _cached_map[label] = it->second;
            return it->second;
        }
    }

    MapInfo info = { std::to_string(label), category_to_string(Unknown) };
    _cached_map[label] = info;

    std::cerr << "Cannot find mapping for label " << label << std::endl;
    return info;
}

std::string TracepointMapper::category_to_string(Category category)
{
    switch (category)
    {
    case Lua:
        return "lua";
    case Native:
        return "native";
    default:
        return "default";
    }
}

class BufferHandler
{
public:
    using ActionHandlerCallback = void(*)(const hawktracer::Action&, void*); // action, user data

    static void process_buffer(char* buffer, std::size_t bufferSize, void* userData);

    void add_action_handler(ActionHandlerCallback callback, void* userData);

private:
    void _process_buffer(char* buffer, std::size_t bufferSize);
    void _broadcast_action(const hawktracer::Action& action);

    std::size_t _offset = 0;
    char _actionBuffer[hawktracer::Action::STRUCT_SIZE];
    std::vector<std::pair<ActionHandlerCallback, void*>> _actionHandlers;
};

void BufferHandler::process_buffer(char* buffer, std::size_t bufferSize, void* userData)
{
    reinterpret_cast<BufferHandler*>(userData)->_process_buffer(buffer, bufferSize);
}

void BufferHandler::add_action_handler(ActionHandlerCallback callback, void* userData)
{
    _actionHandlers.emplace_back(callback, userData);
}

void BufferHandler::_broadcast_action(const hawktracer::Action &action)
{
    for (const auto& handler : _actionHandlers)
    {
        handler.first(action, handler.second);
    }
}

void BufferHandler::_process_buffer(char* buffer, std::size_t bufferSize)
{
    for (size_t i = 0; i < bufferSize; i++)
    {
        _actionBuffer[_offset++] = buffer[i];
        if (_offset == hawktracer::Action::STRUCT_SIZE)
        {
            hawktracer::Action action;
            action.deserialize(_actionBuffer, _offset);
            _offset = 0;
            _broadcast_action(action);
        }
    }
}

class ChromeTracing
{
public:
    bool open(const std::string& filename);
    static void action_handler(const hawktracer::Action &action, void* userData);

    void finalize();

private:
    void _action_handler(const hawktracer::Action &action);
    static double to_microseconds(hawktracer::NanoTime_t time);

    bool _initialized = false;
    std::ofstream ofs;
};

bool ChromeTracing::open(const std::string &filename)
{
    ofs.open(filename);

    return ofs.is_open();
}

void ChromeTracing::action_handler(const hawktracer::Action &action, void* userData)
{
    reinterpret_cast<ChromeTracing*>(userData)->_action_handler(action);
}

double ChromeTracing::to_microseconds(hawktracer::NanoTime_t time)
{
    return time / 1000.0;
}

void ChromeTracing::_action_handler(const hawktracer::Action &action)
{
    if (_initialized)
    {
        ofs << ",";
    }

    if (!_initialized)
    {
        ofs << "{\"traceEvents\":[" << std::endl;
        _initialized = true;
    }

    TracepointMapper::MapInfo info = mapper.get_label_info(action.label);

    ofs << std::fixed
              << "{\"name\":\"" << info.label << "\","
              << "\"cat\":\"" << info.category << "\","
              << "\"pid\": 1,"
              << "\"ts\":\"" << to_microseconds(action.startTime) << "\","
              << "\"tid\":\"" << +action.threadId << "\","
              << "\"ph\":\"X\","
              << "\"dur\":\"" << to_microseconds(action.stopTime - action.startTime) << "\"}" << std::endl;
}

void ChromeTracing::finalize()
{
    if (_initialized)
    {
        ofs << "]}" << std::flush;
    }
}

class CommandLineParser
{
public:
    struct OptionInfo
    {
        std::string help;
        std::string default_value;
        bool is_flag;
        bool is_mandatory;
    };

    CommandLineParser(const std::string& example_usage);

    void parse(char** argv, int argc, bool throw_on_invalid_argument);

    void add_option(const std::string& identifier, const std::string& help, bool is_flag, bool is_mandatory, std::string default_value = "");

    bool option_specified(const std::string& option) const;
    std::string get_value(const std::string& option) const;

    void print_help() const;

private:
    void _validate_mandatory_fields();

    std::string _app_name;
    std::string _example_usage;
    std::unordered_map<std::string, OptionInfo> _options;
    std::unordered_map<std::string, std::string> _values;
};

CommandLineParser::CommandLineParser(const std::string& example_usage) :
    _example_usage(example_usage)
{
}

void CommandLineParser::add_option(const std::string& option, const std::string& help, bool is_flag, bool is_mandatory, std::string default_value)
{
    _options[option] = { help, default_value, is_flag, is_mandatory };
    if (!default_value.empty() && !is_flag)
    {
        _values[option] = default_value;
    }
}

bool CommandLineParser::option_specified(const std::string& option) const
{
    return _values.find(option) != _values.end();
}

std::string CommandLineParser::get_value(const std::string& option) const
{
    return _values.at(option);
}

void CommandLineParser::print_help() const
{
    std::cerr << "Usage: " << _app_name << " [OPTIONS]..." << std::endl;
    std::cerr << "Example: " << _app_name << " " << _example_usage << std::endl;
    std::cerr << " OPTIONS:" << std::endl;

    for (const auto& option : _options)
    {
        std::cerr << " " << option.first << " ";
        if (!option.second.is_flag)
        {
            std::cerr << "VALUE ";
        }
        std::cerr << "- ";
        if (option.second.is_mandatory)
        {
            std::cerr << "[MANDATORY] ";
        }
        if (!option.second.default_value.empty())
        {
            std::cerr << "[Default: " << option.second.default_value << "] ";
        }
        std::cerr << option.second.help << std::endl;
    }
}

void CommandLineParser::parse(char **argv, int argc, bool throw_on_invalid_argument)
{
    _app_name = argv[0];

    for (int i = 1; i < argc; i++)
    {
        auto option_it = _options.find(argv[i]);
        if (option_it == _options.end())
        {
            if (throw_on_invalid_argument)
            {
                throw std::runtime_error("Invalid option: " + std::string(argv[i]));
            }
            else
            {
                continue;
            }
        }

        if (option_it->second.is_flag)
        {
            _values.emplace(argv[i], "");
            continue;
        }

        if (++i == argc)
        {
            throw std::runtime_error("Option " + std::string(argv[i-1]) + " requires argument!");
        }

        _values[std::string(argv[i-1])] = argv[i];
    }

    _validate_mandatory_fields();
}

void CommandLineParser::_validate_mandatory_fields()
{
    for (const auto& option : _options)
    {
        if (option.second.is_mandatory && _values.find(option.first) == _values.end())
        {
            throw std::runtime_error("Option " + option.first + " is mandatory but has not been specified in the command line!");
        }
    }
}

int main(int argc, char **argv)
{
    CommandLineParser parser("-ip 127.0.0.1 -port 8765 -map my-map");

    ChromeTracing chrome_tracing;
    BufferHandler handler;

    parser.add_option("-ip", "IP address of the porfiled device", false, true);
    parser.add_option("-port", "profiling port", false, false, "8765");
    parser.add_option("-map", "comma-separated paths to files with mapping definitions", false, false);
    parser.add_option("-n", "maps labels to the first label in map that is not greater than actual label", true, false);
    parser.add_option("-output-file", "Output file for tracing (you can use specifiers from strftime like %d, %H etc.)",
                      false, false, "hawktracer-trace-%d-%m-%Y-%H_%M_%S.httrace");

    try
    {
        parser.parse(argv, argc, true);
    }
    catch (const std::runtime_error& ex)
    {
        std::cerr << ex.what() << std::endl;
        parser.print_help();
        return 1;
    }

    if (parser.option_specified("-n"))
    {
        mapper.enable_finding_nearest_label();
    }

    if (!parser.option_specified("-map"))
    {
        std::cerr << "map not specified... profiler will use numbers instead of human-readable names" << std::endl;
    }
    else
    {
        std::string maps = parser.get_value("-map");
        size_t start = 0;
        size_t len;
        do
        {
            size_t pos = maps.find(',', start);
            len = pos == std::string::npos ? pos : pos - start;
            std::string path = maps.substr(start, len);
            start += len + 1;

            if (!mapper.load_map(path))
            {
                std::cerr << "unable to load map file " << path << std::endl;
                std::cerr << "map file won't be used" << std::endl;
            }
        } while (len != std::string::npos);
    }

    time_t rawtime;
    time(&rawtime);
    struct tm* time_info = localtime (&rawtime);
    char file_name_buffer[128];
    strftime(file_name_buffer, 128, parser.get_value("-output-file").c_str(), time_info);

    if (!chrome_tracing.open(file_name_buffer))
    {
        std::cerr << "Unable to open file " << file_name_buffer << std::endl;
        return 1;
    }
    else
    {
        std::cerr << "Output will be written to a file: " << file_name_buffer << std::endl;
    }

    handler.add_action_handler(&ChromeTracing::action_handler, &chrome_tracing);

    hawktracer::TCPClient client;
    client.start(parser.get_value("-ip"), std::stoi(parser.get_value("-port")));

    std::thread th = std::thread([&client, &handler] {
        client.handle_responses(&BufferHandler::process_buffer, &handler);
    });

    std::cerr << "Hit [Enter] to finish the trace..." << std::endl;
    getchar();
    client.stop();
    th.join();
    chrome_tracing.finalize();
    std::cerr << "Trace file has been saved to: " << file_name_buffer << std::endl;

    return 0;
}
