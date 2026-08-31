#include "MapMatchingEngine.h"
#include "MapOrderBook.h"

#include <benchmark/benchmark.h>

static Order limitBuy(OrderId id, Price price, Quantity qty) {
    return Order{id, Side::Buy, OrderType::Limit, price, qty, id};
}

static Order limitSell(OrderId id, Price price, Quantity qty) {
    return Order{id, Side::Sell, OrderType::Limit, price, qty, id};
}

static void BM_MapInsert(benchmark::State& state) {
    const int N = static_cast<int>(state.range(0));
    MapOrderBook book;
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

static void BM_MapCancel(benchmark::State& state) {
    const int N = static_cast<int>(state.range(0));
    MapOrderBook book;
    for (auto _ : state) {
        state.PauseTiming();
        for (int i = 0; i < N; ++i) {
            book.add(limitBuy(static_cast<OrderId>(i + 1), 100 + i, 1));
        }
        state.ResumeTiming();
        bool ok = true;
        for (int i = 0; i < N; ++i) {
            ok = book.cancel(static_cast<OrderId>(i + 1)) && ok;
        }
        benchmark::DoNotOptimize(ok);
    }
}

static void BM_MapMatch(benchmark::State& state) {
    const int N = static_cast<int>(state.range(0));
    for (auto _ : state) {
        state.PauseTiming();
        MapMatchingEngine engine;
        for (int i = 0; i < N; ++i) {
            engine.submit(limitSell(static_cast<OrderId>(i + 1), 100 + i, 1));
        }
        engine.submit(limitSell(static_cast<OrderId>(N + 1), 100 + N, 1));
        state.ResumeTiming();
        auto trades = engine.submit(
            limitBuy(static_cast<OrderId>(N + 2), 100 + N - 1, N));
        benchmark::DoNotOptimize(trades.size());
    }
}

BENCHMARK(BM_MapInsert)->Arg(1000);
BENCHMARK(BM_MapCancel)->Arg(1000);
BENCHMARK(BM_MapMatch)->Arg(1000);
