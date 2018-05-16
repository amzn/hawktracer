#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <hawktracer/init.h>

int main(int argc, char** argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    ::testing::InitGoogleMock(&argc, argv);

    ht_init(argc, argv);

    auto ret = RUN_ALL_TESTS();

    ht_deinit();

    return ret;
}
