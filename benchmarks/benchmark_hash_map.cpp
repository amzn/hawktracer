#include <internal/hash_map.h>

#include <benchmark/benchmark.h>

inline uint64_t randomize_integer(uint64_t x)
{
    x = ((x >> 16) ^ x) * 0x45d9f3b;
    x = ((x >> 16) ^ x) * 0x45d9f3b;
    x = (x >> 16) ^ x;
    return x;
}

static void HashMapGetValueEmptyMap(benchmark::State& state)
{
    HT_HashMap map;
    ht_hash_map_init(&map);

    for (auto _ : state)
    {
        ht_hash_map_get_value(&map, 1234);
    }
    ht_hash_map_deinit(&map);
}
BENCHMARK(HashMapGetValueEmptyMap);

static void HashMapInsertElements(benchmark::State& state)
{
    HT_HashMap map;
    ht_hash_map_init(&map);

    for (auto _ : state)
    {
        for (int i = 0; i < state.range(0); i++)
        {
            ht_hash_map_insert(&map, randomize_integer(i), "test", NULL);
        }
    }
    ht_hash_map_deinit(&map);
}
BENCHMARK(HashMapInsertElements)->Range(1, 1<<15);

#include <unordered_map>
static void HashMapInsertElementsCpp(benchmark::State& state)
{
    std::unordered_map<uint64_t, const char*> map;

    for (auto _ : state)
    {
        for (int i = 0; i < state.range(0); i++)
        {
            map[randomize_integer(i)] = "test";
        }
    }
}
BENCHMARK(HashMapInsertElementsCpp)->Range(1, 1<<15);

static void HashMapGetValue(benchmark::State& state)
{
    HT_HashMap map;
    ht_hash_map_init(&map);

    for (int i = 0; i < state.range(0); i++)
    {
        ht_hash_map_insert(&map, randomize_integer(i), "test", NULL);
    }

    for (auto _ : state)
    {
        ht_hash_map_get_value(&map, state.range(1));
    }
    ht_hash_map_deinit(&map);
}
BENCHMARK(HashMapGetValue)
    ->Args({1<<12, 0})
    ->Args({1<<12, (1<<12)/2})
    ->Args({1<<12, (1<<12)-1});

static void HashMapGetValueCpp(benchmark::State& state)
{
    std::unordered_map<uint64_t, const char*> map;

    for (int i = 0; i < state.range(0); i++)
    {
        map[randomize_integer(i)] = "test";
    }

    for (auto _ : state)
    {
        map.find(state.range(1));
    }
}
BENCHMARK(HashMapGetValueCpp)
    ->Args({1<<12, 0})
    ->Args({1<<12, (1<<12)/2})
    ->Args({1<<12, (1<<12)-1});
