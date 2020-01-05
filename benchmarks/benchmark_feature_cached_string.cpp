#include <hawktracer/timeline.h>
#include <hawktracer/feature_cached_string.h>

#include <benchmark/benchmark.h>
#include <vector>
#include <string>
#include <queue>

std::vector<std::string> generate_unique_strings(size_t str_length)
{
    std::vector<std::string> ret;
    std::queue<std::string> q;

    q.push("");

    while (!q.empty())
    {
        std::string cur_s = q.front();
        q.pop();
        for (char c = 'A'; c <= 'a'; c++)
        {
            std::string new_s = cur_s + c;
            if (new_s.length() == str_length)
            {
                ret.emplace_back(std::move(new_s));
            }
            else
            {
                q.emplace(std::move(new_s));
            }
        }
    }

    return ret;
}

static void FeatureCachedStringAddMapping(benchmark::State& state)
{
    HT_Timeline* timeline = ht_timeline_create(1024, HT_FALSE, HT_TRUE, NULL, NULL);
    ht_feature_cached_string_enable(timeline, HT_FALSE);
    auto unique_strings = generate_unique_strings(4);
    size_t ctr = 0;
    for (auto _ : state)
    {
        ht_feature_cached_string_add_mapping(timeline, unique_strings[ctr++ % unique_strings.size()].c_str());
    }

    ht_timeline_destroy(timeline);
}
BENCHMARK(FeatureCachedStringAddMapping);

static void FeatureCachedStringPushMapping_1185921_strings(benchmark::State& state)
{
    HT_Timeline* timeline = ht_timeline_create(1024, HT_FALSE, HT_TRUE, NULL, NULL);
    ht_feature_cached_string_enable(timeline, HT_FALSE);
    auto unique_strings = generate_unique_strings(3);
    for (size_t i = 0; i < unique_strings.size(); i++)
    {
        ht_feature_cached_string_add_mapping(timeline, unique_strings[i].c_str());
    }
    for (auto _ : state)
    {
        ht_feature_cached_string_push_map(timeline);
    }

    ht_timeline_destroy(timeline);
}
BENCHMARK(FeatureCachedStringPushMapping_1185921_strings);

static void FeatureCachedStringAddPushMappingDifferentLabels(benchmark::State& state)
{
    HT_Timeline* timeline = ht_timeline_create(1024, HT_FALSE, HT_TRUE, NULL, NULL);
    ht_feature_cached_string_enable(timeline, HT_FALSE);
    auto unique_strings = generate_unique_strings(4);
    size_t ctr = 0;
    for (auto _ : state)
    {
        ht_feature_cached_string_add_mapping(timeline, unique_strings[ctr++ % unique_strings.size()].c_str());
        ht_feature_cached_string_push_map(timeline);
    }

    ht_timeline_destroy(timeline);
}
BENCHMARK(FeatureCachedStringAddPushMappingDifferentLabels);

static void FeatureCachedStringAddPushMappingTheSameLabel(benchmark::State& state)
{
    HT_Timeline* timeline = ht_timeline_create(1024, HT_FALSE, HT_TRUE, NULL, NULL);
    ht_feature_cached_string_enable(timeline, HT_FALSE);
    for (auto _ : state)
    {
        ht_feature_cached_string_add_mapping(timeline, "label");
        ht_feature_cached_string_push_map(timeline);
    }

    ht_timeline_destroy(timeline);
}
BENCHMARK(FeatureCachedStringAddPushMappingTheSameLabel);

static void FeatureCachedStringAddMappingDynamic(benchmark::State& state)
{
    HT_Timeline* timeline = ht_timeline_create(1024, HT_FALSE, HT_TRUE, NULL, NULL);
    ht_feature_cached_string_enable(timeline, HT_FALSE);
    std::string s(1024*1024, 'a');
    char char_ctr = 'a';
    size_t pos_ctr = 0;

    for (auto _ : state)
    {
        //s[pos_ctr] = char_ctr;
        char_ctr++;
        if (char_ctr == 'z'+1)
        {
            char_ctr = 'a';
            pos_ctr++;
        }
        ht_feature_cached_string_add_mapping_dynamic(timeline, "lab");
    }

    ht_timeline_destroy(timeline);
}
BENCHMARK(FeatureCachedStringAddMappingDynamic);

