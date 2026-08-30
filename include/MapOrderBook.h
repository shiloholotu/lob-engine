#pragma once

#include "Order.h"

#include <deque>
#include <functional>
#include <map>
#include <optional>
#include <unordered_map>

struct OrderLocation {
    Side  side{};
    Price price{};
};

class MapOrderBook {
public:
    void add(const Order& order); // pass by reference, unable to modify
    bool cancel(OrderId id); // pass by value, OrderId is just a uint64_t alias

    // pointer to the first order in the deque, or nullptr if that side is empty
    Order* bestBidFront();
    Order* bestAskFront();
    const Order* bestBidFront() const;
    const Order* bestAskFront() const;

    void popBestBid();
    void popBestAsk();

    std::optional<Price> bestBid() const; // const means the method does not modify the object
    std::optional<Price> bestAsk() const;

private:
    std::map<Price, std::deque<Order>, std::greater<Price>> bids_; // map of price and deque of orders, sorted from highest to lowest
    std::map<Price, std::deque<Order>>                      asks_; // map of price and deque of orders, sorted from lowest to highest by default
    std::unordered_map<OrderId, OrderLocation>              orderIndex_; // maps ids to locations, no sorting needed
};