#ifndef HT_TEST_CLIENT_TESTFILELOADER_HPP
#define HT_TEST_CLIENT_TESTFILELOADER_HPP

#include <anomaly/tree_file_loader.hpp>

class TestFileLoader : public HawkTracer::anomaly::TreeFileLoader
{

public:
    bool init(std::string file_name);
    std::vector<CallGraph::NodeData> get_events();

private:
    void _read_events_data();
    void _parse_file();
    std::vector<CallGraph::NodeData> _events;
};

#endif //HT_TEST_CLIENT_TESTFILELOADER_HPP
