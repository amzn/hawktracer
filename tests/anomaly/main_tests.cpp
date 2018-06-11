#include <gtest/gtest.h>

#include <hawktracer/init.h>

#include "test_path.hpp"

int main(int argc, char** argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    TestPath::get().set_application_path(argv[0]);

    ht_init(argc, argv);

    auto ret = RUN_ALL_TESTS();

    ht_deinit();

    return ret;
}
