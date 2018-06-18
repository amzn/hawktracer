#include <anomaly/pattern_matching.hpp>
#include <tests/client/test_path.hpp>
#include <gtest/gtest.h>

TEST(TestPatternMatching, TestOneRelabel)
{
    auto patterns = std::make_shared<HawkTracer::anomaly::Graphs>();
    patterns->load_from_file(TestPath::get().get_input_file_path("test1_pattern.json"));

    auto config = std::make_shared<HawkTracer::anomaly::Config>();
    config->load_from_file(TestPath::get().get_input_file_path("test1_config.json"));

    HawkTracer::anomaly::PatternMatching pattern_matcher(config, patterns);
    auto source = std::make_shared<HawkTracer::anomaly::Graphs>();
    source->load_from_file(TestPath::get().get_input_file_path("test1_source.json"));
    
    const auto& sources = source->get_trees();
    ASSERT_TRUE(patterns->get_trees().size() == 1);
    ASSERT_TRUE(sources.size() == 1);
    for (const auto& source : sources)
    {
        auto ans = pattern_matcher.get_matching_scores(source.first);
        ASSERT_TRUE(ans.size() == 1);
        ASSERT_TRUE(ans[0] == 2);
    }
}

TEST(TestPatternMatching, TestSameTree)
{
    auto patterns = std::make_shared<HawkTracer::anomaly::Graphs>();
    patterns->load_from_file(TestPath::get().get_input_file_path("test2_pattern.json"));

    auto config = std::make_shared<HawkTracer::anomaly::Config>();
    config->load_from_file(TestPath::get().get_input_file_path("test2_config.json"));

    HawkTracer::anomaly::PatternMatching pattern_matcher(config, patterns);
    auto source = std::make_shared<HawkTracer::anomaly::Graphs>();
    source->load_from_file(TestPath::get().get_input_file_path("test2_source.json"));
    
    const auto& sources = source->get_trees();
    ASSERT_TRUE(patterns->get_trees().size() == 1);
    ASSERT_TRUE(sources.size() == 1);
    for (const auto& source : sources)
    {
        auto ans = pattern_matcher.get_matching_scores(source.first);
        ASSERT_TRUE(ans.size() == 1);
        ASSERT_TRUE(ans[0] == 0);
    }
}

TEST(TestPatternMatching, TestOneDelete)
{
    auto patterns = std::make_shared<HawkTracer::anomaly::Graphs>();
    patterns->load_from_file(TestPath::get().get_input_file_path("test3_pattern.json"));

    auto config = std::make_shared<HawkTracer::anomaly::Config>();
    config->load_from_file(TestPath::get().get_input_file_path("test3_config.json"));

    HawkTracer::anomaly::PatternMatching pattern_matcher(config, patterns);
    auto source = std::make_shared<HawkTracer::anomaly::Graphs>();
    source->load_from_file(TestPath::get().get_input_file_path("test3_source.json"));
    
    const auto& sources = source->get_trees();
    ASSERT_TRUE(patterns->get_trees().size() == 1);
    ASSERT_TRUE(sources.size() == 1);
    for (const auto& source : sources)
    {
        auto ans = pattern_matcher.get_matching_scores(source.first);
        ASSERT_TRUE(ans.size() == 1);
        ASSERT_TRUE(ans[0] == 20);
    }
}

TEST(TestPatternMatching, TestOneDeleteAndOneInsert)
{
    auto patterns = std::make_shared<HawkTracer::anomaly::Graphs>();
    patterns->load_from_file(TestPath::get().get_input_file_path("test4_pattern.json"));
    ASSERT_TRUE(patterns->get_trees().size() == 1);

    auto config = std::make_shared<HawkTracer::anomaly::Config>();
    config->load_from_file(TestPath::get().get_input_file_path("test4_config.json"));

    HawkTracer::anomaly::PatternMatching pattern_matcher(config, patterns);
    auto source = std::make_shared<HawkTracer::anomaly::Graphs>();
    source->load_from_file(TestPath::get().get_input_file_path("test4_source.json"));
    
    const auto& sources = source->get_trees();
    ASSERT_TRUE(patterns->get_trees().size() == 1);
    ASSERT_TRUE(sources.size() == 1);
    for (const auto& source : sources)
    {
        auto ans = pattern_matcher.get_matching_scores(source.first);
        ASSERT_TRUE(ans.size() == 1);
        ASSERT_TRUE(ans[0] == 30);
    }
}

TEST(TestPatternMatching, TestReverseChildrenOrder)
{
    auto patterns = std::make_shared<HawkTracer::anomaly::Graphs>();
    patterns->load_from_file(TestPath::get().get_input_file_path("test5_pattern.json"));

    auto config = std::make_shared<HawkTracer::anomaly::Config>();
    config->load_from_file(TestPath::get().get_input_file_path("test5_config.json"));

    HawkTracer::anomaly::PatternMatching pattern_matcher(config, patterns);
    auto source = std::make_shared<HawkTracer::anomaly::Graphs>();
    source->load_from_file(TestPath::get().get_input_file_path("test5_source.json"));
    
    const auto& sources = source->get_trees();
    ASSERT_TRUE(patterns->get_trees().size() == 1);
    ASSERT_TRUE(sources.size() == 1);
    for (const auto& source : sources)
    {
        auto ans = pattern_matcher.get_matching_scores(source.first);
        printf("%d\n", ans[0]);
        ASSERT_TRUE(ans.size() == 1);
        ASSERT_TRUE(ans[0] == 24);
    }
}

