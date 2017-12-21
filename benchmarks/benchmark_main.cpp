#include <hawktracer/init.h>

#include <benchmark/benchmark.h>

int main(int argc, char** argv)
{
    ht_init(argc, argv);
    ::benchmark::Initialize(&argc, argv);
    if (::benchmark::ReportUnrecognizedArguments(argc, argv))
    {
        ht_deinit();
        return 1;
    }
    ::benchmark::RunSpecifiedBenchmarks();

    ht_deinit();

    return 0;
}
