#include <gtest/gtest.h>

#include <hawktracer/init.h>

int main(int argc, char** argv)
{
    ::testing::InitGoogleTest(&argc, argv);

    ht_init(argc, argv);

    auto ret = RUN_ALL_TESTS();

    ht_deinit();

    return ret;
}
