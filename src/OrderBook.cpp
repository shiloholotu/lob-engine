#include "OrderBook.h"

void OrderBook::add(const Order& order) {
    // order is a read only alias of the caller's order
    if (order.side == Side::Buy)
        bids_[order.price].push_back(order); // copy the order into the deque
    else
        asks_[order.price].push_back(order); // copy the order into the deque
    orderIndex_[order.id] = { order.side, order.price }; // save the location of the order in the book
}

bool OrderBook::cancel(OrderId id) {
    // find the order in the index
    auto idx = orderIndex_.find(id);
    if (idx == orderIndex_.end())
        return false;

    // save its location and then remove it from the index
    OrderLocation loc = idx->second;
    orderIndex_.erase(idx);

    if (loc.side == Side::Buy) {
        // find the deque of orders at the order's price
        auto pit = bids_.find(loc.price);
        if (pit == bids_.end())
            return false;
        auto& q = pit->second;
        // find the order in the deque and remove it
        for (auto oit = q.begin(); oit != q.end(); ++oit) {
            if (oit->id == id) {
                q.erase(oit);
                break;
            }
        }
        // if the queue is empty, remove the price from the map
        if (q.empty())
            bids_.erase(pit);
    } else {
        auto pit = asks_.find(loc.price);
        if (pit == asks_.end())
            return false;
        auto& q = pit->second;
        for (auto oit = q.begin(); oit != q.end(); ++oit) {
            if (oit->id == id) {
                q.erase(oit);
                break;
            }
        }
        if (q.empty())
            asks_.erase(pit);
    }
    return true;
}

Order* OrderBook::bestBidFront() {
    if (!bids_.empty()){
        return &(bids_.begin()->second.front());
         // -> second.front() is the first order in the deque,
         // & is so we return a pointer, so the engine can modify the order
    }
    return nullptr;
}

Order* OrderBook::bestAskFront() {
    if (!asks_.empty())
        return &(asks_.begin()->second.front());
    return nullptr;
}


void OrderBook::popBestBid() {
    auto& q = bids_.begin() -> second; // alias the queue of orders at best bid price
    OrderId id = q.front().id; // get the id of the first order
    q.pop_front(); // remove it
    orderIndex_.erase(id); // remove the order from the index
    if (q.empty())
        bids_.erase(bids_.begin()); // if it was the only order at that price, remove the price from the map
}

void OrderBook::popBestAsk() {
    auto& q = asks_.begin() -> second;
    OrderId id = q.front().id;
    q.pop_front();
    orderIndex_.erase(id);
    if (q.empty())
        asks_.erase(asks_.begin());
}

std::optional<Price> OrderBook::bestBid() const {
    if (!bids_.empty())
        return bids_.begin()->first;
    return std::nullopt;
}

std::optional<Price> OrderBook::bestAsk() const {
    if (!asks_.empty())
        return asks_.begin()->first;
    return std::nullopt;
}