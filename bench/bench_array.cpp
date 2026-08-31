#include "MatchingEngine.h"
#include "OrderBook.h"

#include <benchmark/benchmark.h>

static Order limitBuy(OrderId id, Price price, Quantity qty) {
    return Order{id, Side::Buy, OrderType::Limit, price, qty, id};
}

static Order limitSell(OrderId id, Price price, Quantity qty) {
    return Order{id, Side::Sell, OrderType::Limit, price, qty, id};
}

// One book for the whole run so the 100k-slot pool stays warm.
// Cleanup is PauseTiming so only the measured op is in the Time column.
// Insert/cancel use N distinct prices (100 .. 100+N-1) so the map actually
// walks a tree, not 50 tiny nodes.

static void BM_ArrayInsert(benchmark::State& state) {
    const int N = static_cast<int>(state.range(0));
    OrderBook book;
    for (auto _ : state) {
        for (int i = 0; i < N; ++i) {
            book.add(limitBuy(static_cast<OrderId>(i + 1), 100 + i, 1));
        }
        benchmark::DoNotOptimize(book.bestBid());
        state.PauseTiming();
        for (int i = 0; i < N; ++i) {
            book.cancel(static_cast<OrderId>(i + 1));
        }
        state.ResumeTiming();
    }
}

BENCHMARK(BM_ArrayInsert)->Arg(1000);

BENCHMARK_MAIN();
