#include "test_file_loader.hpp"
#include <sstream>

bool TestFileLoader::init(std::string file_name)
{
    _file.open(file_name);
    if (_file.is_open())
    {
        _parse_file();
    }
    return _file.is_open();
}

std::vector<CallGraph::NodeData> TestFileLoader::get_events()
{
    return _events;
}

void TestFileLoader::_read_events_data()
{
    std::string blank_line;
    size_t cnt_lines = stoi(_next_valid_line());

    // LABEL   START_TS   DURATION
    for (size_t i = 0; i < cnt_lines; ++i)
    { 
        std::stringstream line_stream(_next_valid_line());
        std::string label;
        HT_TimestampNs start_ts = 0;
        HT_DurationNs dur = 0;

        line_stream >> label >> start_ts >> dur;

        _events.emplace_back(label, start_ts, dur);
    }
}

void TestFileLoader::_parse_file()
{

    _read_tree_nodes();
    _read_tree_edges();
    _read_events_data();
}
