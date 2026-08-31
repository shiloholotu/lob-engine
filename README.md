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

