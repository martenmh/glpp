#include <benchmark/benchmark.h>

static void BM_(benchmark::State& state) {
	for(auto _: state) {
	}
	state.SetBytesProcessed(int64_t(state.iterations()) *
							int64_t(state.range(0)));
}

BENCHMARK(BM_);

BENCHMARK_MAIN();
