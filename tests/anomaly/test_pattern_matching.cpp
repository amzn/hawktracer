#include <anomaly/pattern_matching.hpp>
#include <tests/client/test_path.hpp>
#include <gtest/gtest.h>

void test_pattern_matching(std::string pattern_file_name,
                           std::string config_file_name,
                           std::string source_file_name,
                           std::vector<std::vector<int>> results)
{
    auto patterns = std::make_shared<HawkTracer::anomaly::Graphs>();
    patterns->load_from_file(TestPath::get().get_input_file_path(pattern_file_name));

    auto config = std::make_shared<HawkTracer::anomaly::Config>();
    config->load_from_file(TestPath::get().get_input_file_path(config_file_name));

    HawkTracer::anomaly::PatternMatching pattern_matcher(config, patterns);
    auto source = std::make_shared<HawkTracer::anomaly::Graphs>();
    source->load_from_file(TestPath::get().get_input_file_path(source_file_name));
    
    const auto& sources = source->get_trees();
    for (size_t i = 0; i < sources.size(); ++i)
    {
        auto ans = pattern_matcher.get_matching_scores(sources[i].first);
        ASSERT_EQ(results[i].size(), ans.size());
        for (size_t j = 0; j < ans.size(); ++j)
        {
            ASSERT_EQ(results[i][j], ans[j]);
        }
    }
}

TEST(TestPatternMatching, TestOneRelabel)
{
    test_pattern_matching("test1_pattern.json",
                          "test1_config.json",
                          "test1_source.json",
                          {{2}});
}

TEST(TestPatternMatching, TestSameTree)
{
    test_pattern_matching("test2_pattern.json",
                          "test2_config.json",
                          "test2_source.json",
                          {{0}});
}

TEST(TestPatternMatching, TestOneDelete)
{
    test_pattern_matching("test3_pattern.json",
                          "test3_config.json",
                          "test3_source.json",
                          {{20}});
}

TEST(TestPatternMatching, TestOneDeleteAndOneInsert)
{
    test_pattern_matching("test4_pattern.json",
                          "test4_config.json",
                          "test4_source.json",
                          {{30}});
}

TEST(TestPatternMatching, TestReverseChildrenOrder)
{
    test_pattern_matching("test5_pattern.json",
                          "test5_config.json",
                          "test5_source.json",
                          {{24}});
}

