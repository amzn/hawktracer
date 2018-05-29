#ifndef HT_TEST_CLIENT_TESTPATH_CPP
#define HT_TEST_CLIENT_TESTPATH_CPP

#include "test_path.hpp"

#ifdef _WIN32
#define PATH_SEPARATOR "\\"
#else
#define PATH_SEPARATOR "/"
#endif

TestPath::TestPath()
{
}

TestPath& TestPath::get()
{
    static TestPath test_path;
    return test_path;
}

void TestPath::set_application_path(const std::string& app_path)
{
    size_t pos = app_path.rfind(PATH_SEPARATOR);
    if (pos != std::string::npos)
    {
        _tests_dir= app_path.substr(0, pos + 1);
    }
}

std::string TestPath::get_input_file_path(const std::string& file_name)
{
    return _tests_dir + file_name;
}

#endif //HT_TEST_CLIENT_TESTPATH_CPP
