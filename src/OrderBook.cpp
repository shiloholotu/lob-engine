#include "OrderBook.h"

void OrderBook::add(const Order& /*order*/) {
    // Week 2: push into bids_ or asks_ and orderIndex_
}

bool OrderBook::cancel(OrderId /*id*/) {
    return false;
}

std::optional<Price> OrderBook::bestBid() const {
    return std::nullopt;
}

std::optional<Price> OrderBook::bestAsk() const {
    return std::nullopt;
}