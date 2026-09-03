#pragma once

#include "Order.h"
#include "MapOrderBook.h"

#include <vector>

struct Trade {
    OrderId  maker_id{};
    OrderId  taker_id{};
    Price    price{};
    Quantity quantity{};
};

class MapMatchingEngine {
public:
    std::vector<Trade> submit(Order order); // an incoming order can produce several fills. Takes and modifies a copy so caller's order is unchanged
    bool cancel(OrderId id);

    const MapOrderBook& book() const { return book_; } // peek at the book without modifying it
    void reset();

private:
    MapOrderBook book_;
};