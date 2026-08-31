# Limit Order Book & Matching Engine

A C++20 matching engine with price-time priority for limit, market, and cancel orders. Resting orders live in a price-indexed array and a preallocated pool. A `std::map` book is kept only so both layouts can be benchmarked on the same workloads.

## Matching

- **Limit** orders fill against the opposite side at the maker's price, then rest if any quantity remains.
- **Market** orders fill against whatever is there and never rest. Leftover quantity is dropped.
- **Cancel** removes a resting order by id.
- At a given price, earlier orders fill first (FIFO). Better prices fill before worse ones.
- A single incoming order can produce several trades if it walks more than one level.

Prices are integers (ticks), not floats.

