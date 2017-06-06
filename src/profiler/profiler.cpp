#include <hawktracer/TCPClient.h>
#include <hawktracer/Timeline.h>

#include <signal.h>

#include <fstream>
#include <cstring>
#include <iostream>

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

    TracepointMapper::MapInfo get_label_info(uint32_t label);

    bool load_map(const std::string& map_file);

private:
    std::unordered_map<uint32_t, MapInfo> _map;

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

    std::string label;
    uint32_t number;
    uint32_t category;
    while (file >> number >> category >> label)
    {
        _map[number] = { label, category_to_string((Category)category) };
    }

    return true;
}

TracepointMapper::MapInfo TracepointMapper::get_label_info(uint32_t label)
{
    auto it = _map.find(label);
    if (it == _map.end())
    {
        std::cerr << "Cannot find mapping for label " << label << std::endl;
        return { std::to_string(label), category_to_string(Unknown) };
    }

    return it->second;
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

ChromeTracing chrome_tracing;
BufferHandler handler;

void signal_callback_handler(int signum)
{
    chrome_tracing.finalize();

    exit(signum);
}

int main(int argc, char **argv)
{
    if (argc != 5 && argc != 4)
    {
        std::cerr << "usage: " << argv[0] << " ip port chrome-tracing|csv [map_file]" << std::endl;
        return 1;
    }

    if (argc < 5)
    {
        std::cerr << "map not specified... profiler will use numbers instead of human-readable names" << std::endl;
    }
    else if (!mapper.load_map(argv[4]))
    {
        std::cerr << "unable to load map file " << argv[4] << std::endl;
	std::cerr << "map file won't be used" << std::endl;
    }

    if (strcmp(argv[3], "chrome-tracing") == 0)
    {
        handler.add_action_handler(&ChromeTracing::action_handler, &chrome_tracing);
    }
    else if (strcmp(argv[3], "csv") == 0)
    {
        handler.add_action_handler(csv_action_handler, nullptr);
    }
    else
    {
        std::cerr << "invalid mode " << argv[3] << std::endl;
        return 1;
    }

    signal(SIGINT, signal_callback_handler);

    hawktracer::TCPClient client;
    client.start(argv[1], atoi(argv[2]));

    client.handle_responses(&BufferHandler::process_buffer, &handler);

    return 0;
}
