#pragma once

#include <cstdint>

using OrderId  = std::uint64_t;
using Price    = std::int64_t;
using Quantity = std::int64_t;

// typed set of named values
enum class Side { Buy, Sell };
enum class OrderType { Limit, Market };

struct Order {
    OrderId    id{}; // {} means default to zero or the first enum value
    Side       side{};
    OrderType  type{};
    Price      price{};
    Quantity   quantity{};
    std::uint64_t timestamp{};
};