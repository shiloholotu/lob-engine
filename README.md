# Limit Order Book & Matching Engine

A C++20 matching engine with price-time priority for limit, market, and cancel orders. Resting orders live in a price-indexed array and a preallocated pool. A `std::map` book is kept only so both layouts can be benchmarked on the same workloads.

## Matching

- **Limit** orders fill against the opposite side at the maker's price, then rest if any quantity remains.
- **Market** orders fill against whatever is there and never rest. Leftover quantity is dropped.
- **Cancel** removes a resting order by id.
- At a given price, earlier orders fill first (FIFO). Better prices fill before worse ones.
- A single incoming order can produce several trades if it walks more than one level.

Prices are integers (ticks), not floats.

## Design

The live book (`OrderBook`) stores bids and asks as two arrays of price levels, indexed by `price - minPrice`. Best bid and best ask are cached indices, updated on insert and walked to the next occupied level when the touch empties — the book does not scan the whole array on every lookup.

Each resting order is a slot in an `OrderPool` (100,000 preallocated `Order`s). Allocate/free is an index into that buffer, not `new`/`delete` on the hot path. Levels hold `Order*` into the pool and stay FIFO via `std::deque`.

Cancel still uses an `unordered_map` from id to `{side, price, pointer}`, then a linear scan of that price's deque. That hash map allocates; the pool only covers the `Order` objects themselves.

The original book (`MapOrderBook`) is a `std::map` of price → `deque<Order>`. It is compiled into the benchmark binary only, so insert/cancel/match can be compared against the array layout without changing matching rules.

### Price range

Legal resting prices are `[0, 10000)`. `add` ignores an order whose price is outside that band; it does not become best bid or best ask. Crossing an out-of-range limit against the live book is a matching-policy question that this engine does not special-case: a limit that never rests simply will not sit on the book.

## Benchmarks

Google Benchmark, **Release**, `--benchmark_min_time=0.5s`, 12 × 2592 MHz CPUs. Each row is one batch of **N = 1000** operations (not a single order). Times are wall time.

| Version | Insert | Cancel | Match |
|---|---|---|---|
| Array + pool | 61,590 ns | 47,714 ns | 678,357 ns |
| `std::map` + deque | 265,293 ns | 159,334 ns | 145,341 ns |

On this machine the array book is about **4× faster on insert** and **3× faster on cancel**. Matching a buy that walks 1,000 consecutive ask levels is **slower** on the array book.

Insert and cancel benefit from contiguous levels and pool slots instead of tree nodes. The match bench builds a fresh engine every iteration (10,000 deques plus a 100k-slot pool) and then pops a thousand levels, so construction and best-index walks dominate. The map book is a small tree; that workload does not favor the array. Those numbers are included on purpose — the layout change is not a blanket speedup.

Run them:

```powershell
cmake --build build --config Release --target lob_bench
.\build\Release\lob_bench.exe --benchmark_min_time=0.5s
```

## Tests

Google Test covers book invariants (best bid/ask, FIFO, cancel, out-of-range rest), pool allocate/free, and matching (full fill, partial fill, multi-level crosses, market orders, cancel).

```powershell
cmake --build build --config Debug --target lob_tests
.\build\Debug\lob_tests.exe
```

## Build

C++20, CMake 3.20+, and a compiler that can build Google Test and Google Benchmark (MSVC 2022, GCC 11+, or Clang 14+). Dependencies are pulled with FetchContent; no extra install step.

```powershell
cmake -S . -B build
cmake --build build --config Debug
```
