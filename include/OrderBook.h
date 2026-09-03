#pragma once

#include "Order.h"
#include "OrderPool.h"

#include <deque>
#include <optional>
#include <unordered_map>
#include <vector>

struct OrderLocation {
    Side  side{};
    Price price{};
    Order* ptr{};
};

struct PriceLevel {
    std::deque<Order*> orders;  // FIFO: push_back, pop_front; pointers into the pool
};


class OrderBook {
public:
    static constexpr Price kMinPrice = 0;
    static constexpr int kPriceRange = 10'000;

    bool inRange(Price price) const;
    int indexOf(Price price) const;
    Price priceOf(int index) const;

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

    void reset();

private:
    OrderPool pool_;
    std::vector<PriceLevel> bidLevels_ = std::vector<PriceLevel>(kPriceRange);
    std::vector<PriceLevel> askLevels_ = std::vector<PriceLevel>(kPriceRange);
    std::optional<int> bestBidIndex_;
    std::optional<int> bestAskIndex_;
    std::unordered_map<OrderId, OrderLocation> orderIndex_;

    void recomputeBestBidFrom(int i);
    void recomputeBestAskFrom(int i);
};