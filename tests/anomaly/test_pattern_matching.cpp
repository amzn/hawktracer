#include <anomaly/pattern_matching.hpp>
#include <anomaly/source.hpp>
#include "test_path.hpp"
#include <gtest/gtest.h>

TEST(TestPatternMatching, TestOneRelabel)
{
    auto patterns = std::make_shared<HawkTracer::anomaly::Patterns>();
    patterns->init(TestPath::get().get_input_file_path("test1_pattern.txt"));

    auto config = std::make_shared<HawkTracer::anomaly::Config>();
    config->init(TestPath::get().get_input_file_path("test1_config.json"));

    HawkTracer::anomaly::PatternMatching pattern_matcher(config, patterns);
    auto source = std::make_shared<HawkTracer::anomaly::Source>();
    source->init(TestPath::get().get_input_file_path("test1_source.txt"));
    
    const auto& sources = source->get_trees();
    for (const auto& source : sources)
    {
        auto ans = pattern_matcher.get_matching_scores(source.first);
        ASSERT_TRUE(ans.size() == 1);
        ASSERT_TRUE(ans[0] == 2);
    }
}

TEST(TestPatternMatching, TestSameTree)
{
    auto patterns = std::make_shared<HawkTracer::anomaly::Patterns>();
    patterns->init(TestPath::get().get_input_file_path("test2_pattern.txt"));

    auto config = std::make_shared<HawkTracer::anomaly::Config>();
    config->init(TestPath::get().get_input_file_path("test2_config.json"));

    HawkTracer::anomaly::PatternMatching pattern_matcher(config, patterns);
    auto source = std::make_shared<HawkTracer::anomaly::Source>();
    source->init(TestPath::get().get_input_file_path("test2_source.txt"));
    
    const auto& sources = source->get_trees();
    for (const auto& source : sources)
    {
        auto ans = pattern_matcher.get_matching_scores(source.first);
        ASSERT_TRUE(ans.size() == 1);
        ASSERT_TRUE(ans[0] == 0);
    }
}

TEST(TestPatternMatching, TestOneDelete)
{
    auto patterns = std::make_shared<HawkTracer::anomaly::Patterns>();
    patterns->init(TestPath::get().get_input_file_path("test3_pattern.txt"));

    auto config = std::make_shared<HawkTracer::anomaly::Config>();
    config->init(TestPath::get().get_input_file_path("test3_config.json"));

    HawkTracer::anomaly::PatternMatching pattern_matcher(config, patterns);
    auto source = std::make_shared<HawkTracer::anomaly::Source>();
    source->init(TestPath::get().get_input_file_path("test3_source.txt"));
    
    const auto& sources = source->get_trees();
    for (const auto& source : sources)
    {
        auto ans = pattern_matcher.get_matching_scores(source.first);
        ASSERT_TRUE(ans.size() == 1);
        ASSERT_TRUE(ans[0] == 20);
    }
}

TEST(TestPatternMatching, TestOneDeleteAndOneInsert)
{
    auto patterns = std::make_shared<HawkTracer::anomaly::Patterns>();
    patterns->init(TestPath::get().get_input_file_path("test4_pattern.txt"));

    auto config = std::make_shared<HawkTracer::anomaly::Config>();
    config->init(TestPath::get().get_input_file_path("test4_config.json"));

    HawkTracer::anomaly::PatternMatching pattern_matcher(config, patterns);
    auto source = std::make_shared<HawkTracer::anomaly::Source>();
    source->init(TestPath::get().get_input_file_path("test4_source.txt"));
    
    const auto& sources = source->get_trees();
    for (const auto& source : sources)
    {
        auto ans = pattern_matcher.get_matching_scores(source.first);
        ASSERT_TRUE(ans.size() == 1);
        ASSERT_TRUE(ans[0] == 30);
    }
}

TEST(TestPatternMatching, TestReverseChildrenOrder)
{
    auto patterns = std::make_shared<HawkTracer::anomaly::Patterns>();
    patterns->init(TestPath::get().get_input_file_path("test5_pattern.txt"));

    auto config = std::make_shared<HawkTracer::anomaly::Config>();
    config->init(TestPath::get().get_input_file_path("test5_config.json"));

    HawkTracer::anomaly::PatternMatching pattern_matcher(config, patterns);
    auto source = std::make_shared<HawkTracer::anomaly::Source>();
    source->init(TestPath::get().get_input_file_path("test5_source.txt"));
    
    const auto& sources = source->get_trees();
    for (const auto& source : sources)
    {
        auto ans = pattern_matcher.get_matching_scores(source.first);
        ASSERT_TRUE(ans.size() == 1);
        ASSERT_TRUE(ans[0] == 24);
    }
}

