#ifndef HT_TEST_CLIENT_TESTPATH_HPP
#define HT_TEST_CLIENT_TESTPATH_HPP

#include <string>

class TestPath
{
public:
    TestPath(TestPath&) = delete;
    TestPath& operator=(TestPath&) = delete;

    static TestPath& get();

    void set_application_path(const std::string& app_path);
    std::string get_input_file_path(const std::string& file_name);

private:
    TestPath();
    std::string _tests_dir;
};

#endif //HT_TEST_CLIENT_TESTPATH_HPP

