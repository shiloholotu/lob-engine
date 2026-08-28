#include <benchmark/benchmark.h>

static void BM_Add(benchmark::State& state) {
    for (auto _ : state) {
        int x = 1 + 1;
        benchmark::DoNotOptimize(x);
    }
}
BENCHMARK(BM_Add);

BENCHMARK_MAIN();