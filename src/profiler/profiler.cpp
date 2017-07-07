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

void csv_action_handler(const hawktracer::Action& action, void* userData)
{
    static bool initialized = false;

    if (!initialized)
    {
        std::cout << "Action label,Action ID,Start time,Stop time,Thread ID" << std::endl;
        initialized = true;
    }

    std::cout << mapper.get_label_info(action.label).label << ","
              << action.actionId << ","
              << action.startTime << ","
              << action.stopTime << ","
              << +action.threadId << std::endl;
}

class ChromeTracing
{
public:
    static void action_handler(const hawktracer::Action &action, void* userData);

    void finalize();

private:
    void _action_handler(const hawktracer::Action &action);
    static double to_microseconds(hawktracer::NanoTime_t time);

    bool initialized = false;
};

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
    if (initialized)
    {
        std::cout << ",";
    }

    if (!initialized)
    {
        std::cout << "{\"traceEvents\":[" << std::endl;
        initialized = true;
    }

    TracepointMapper::MapInfo info = mapper.get_label_info(action.label);

    std::cout << std::fixed
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
    if (initialized)
    {
        std::cout << "]}" << std::flush;
    }
}

class CommandLineParser
{
public:
    struct OptionInfo
    {
        std::string help;
        bool is_flag;
        bool is_mandatory;
    };

    CommandLineParser(const std::string& example_usage);

    void parse(char** argv, int argc, bool throw_on_invalid_argument);

    void add_option(const std::string& identifier, const std::string& help, bool is_flag, bool is_mandatory);

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

void CommandLineParser::add_option(const std::string& option, const std::string& help, bool is_flag, bool is_mandatory)
{
    _options[option] = { help, is_flag, is_mandatory };
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

        _values.emplace(argv[i-1], argv[i]);
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

ChromeTracing chrome_tracing;
BufferHandler handler;

void signal_callback_handler(int signum)
{
    chrome_tracing.finalize();

    exit(signum);
}

int main(int argc, char **argv)
{
    CommandLineParser parser("-ip 127.0.0.1 -port 8765 -format chrome-tracing -map my-map");

    parser.add_option("-ip", "IP address of the porfiled device", false, true);
    parser.add_option("-port", "profiling port", false, true);
    parser.add_option("-format", "output format (csv or chrome-tracing)", false, true);
    parser.add_option("-map", "path to a file with mapping definitions", false, false);
    parser.add_option("-n", "maps labels to the first label in map that is not greater than actual label", true, false);

    try
    {
        parser.parse(argv, argc, true);
    }
    catch (const std::runtime_error& ex)
    {
        std::cerr << ex.what();
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
    else if (!mapper.load_map(parser.get_value("-map")))
    {
        std::cerr << "unable to load map file " << parser.get_value("-map") << std::endl;
        std::cerr << "map file won't be used" << std::endl;
    }

    if (parser.get_value("-format") == "chrome-tracing")
    {
        handler.add_action_handler(&ChromeTracing::action_handler, &chrome_tracing);
    }
    else if (parser.get_value("-format") == "csv")
    {
        handler.add_action_handler(csv_action_handler, nullptr);
    }
    else
    {
        std::cerr << "invalid mode " << parser.get_value("-format") << std::endl;
        return 1;
    }

    signal(SIGINT, signal_callback_handler);

    hawktracer::TCPClient client;
    client.start(parser.get_value("-ip"), std::stoi(parser.get_value("-port")));

    client.handle_responses(&BufferHandler::process_buffer, &handler);

    return 0;
}
