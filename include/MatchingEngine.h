#pragma once

#include "Order.h"
#include "OrderBook.h"

#include <vector>

struct Trade {
    OrderId  maker_id{};
    OrderId  taker_id{};
    Price    price{};
    Quantity quantity{};
};

class MatchingEngine {
public:
    std::vector<Trade> submit(Order order); // an incoming order can produce several fills. Takes and modifies a copy so caller's order is unchanged
    bool cancel(OrderId id);

    const OrderBook& book() const { return book_; } // peek at the book without modifying it

private:
    OrderBook book_;
};