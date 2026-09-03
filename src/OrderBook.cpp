#include "OrderBook.h"

bool OrderBook::inRange(Price price) const {
    return price >= kMinPrice && price < kMinPrice + kPriceRange;
}

int OrderBook::indexOf(Price price) const {
    return static_cast<int>(price - kMinPrice);
}

Price OrderBook::priceOf(int index) const {
    return kMinPrice + index;
}

void OrderBook::add(const Order& order) {
    // return if the price is out of range
    if (!inRange(order.price))
        return;
    int i = indexOf(order.price);
    // get a slot for the order
    Order* slot = pool_.allocate();
    if (slot == nullptr)
        return;
    *slot = order; // copy the order into the slot
    if(order.side == Side::Buy){
        bidLevels_[i].orders.push_back(slot); // add the order to the price level
        if(!bestBidIndex_ || i > *bestBidIndex_) // update the best bid index if it is not set or the new price is higher
            bestBidIndex_ = i;
    }
    else{
        askLevels_[i].orders.push_back(slot);
        if(!bestAskIndex_ || i < *bestAskIndex_)
            bestAskIndex_ = i;
    }
    orderIndex_[order.id] = { order.side, order.price, slot }; // add the order to the index
}

bool OrderBook::cancel(OrderId id) {
    // find the order in the index
    auto idx = orderIndex_.find(id);
    if (idx == orderIndex_.end())
        return false;
    // save its location and then remove it from the index
    OrderLocation loc = idx->second;
    orderIndex_.erase(idx);

    int i = indexOf(loc.price);
    if (loc.side == Side::Buy) {
        auto& q = bidLevels_[i].orders; // find the deque of orders at the order's price level
        for (auto oit = q.begin(); oit != q.end(); ++oit) { // find the pointer to the order in the deque and remove it
            if (*oit == loc.ptr) {
                q.erase(oit);
                break;
            }
        }
        if (q.empty() && bestBidIndex_ == i) // if the deque is empty and the order was the best bid, recompute the best bid
            recomputeBestBidFrom(i);
    } else {
        auto& q = askLevels_[i].orders;
        for (auto oit = q.begin(); oit != q.end(); ++oit) {
            if (*oit == loc.ptr) {
                q.erase(oit);
                break;
            }
        }
        if (q.empty() && bestAskIndex_ == i)
            recomputeBestAskFrom(i);
    }
    pool_.deallocate(loc.ptr);
    return true;
}

void OrderBook::recomputeBestBidFrom(int i) {
    // level i just emptied; walk down to the next occupied bid (higher prices are better)
    bestBidIndex_.reset();
    for (int j = i - 1; j >= 0; --j) {
        if (!bidLevels_[j].orders.empty()) {
            bestBidIndex_ = j;
            break;
        }
    }
}

void OrderBook::recomputeBestAskFrom(int i) {
    // level i just emptied; walk up to the next occupied ask (lower prices are better)
    bestAskIndex_.reset();
    for (int j = i + 1; j < kPriceRange; ++j) {
        if (!askLevels_[j].orders.empty()) {
            bestAskIndex_ = j;
            break;
        }
    }
}

Order* OrderBook::bestBidFront() {
    if (!bestBidIndex_)
        return nullptr;
    // deque already stores Order* into the pool
    return bidLevels_[*bestBidIndex_].orders.front();
}

Order* OrderBook::bestAskFront() {
    if (!bestAskIndex_)
        return nullptr;
    return askLevels_[*bestAskIndex_].orders.front();
}

const Order* OrderBook::bestBidFront() const {
    if (!bestBidIndex_)
        return nullptr;
    return bidLevels_[*bestBidIndex_].orders.front();
}

const Order* OrderBook::bestAskFront() const {
    if (!bestAskIndex_)
        return nullptr;
    return askLevels_[*bestAskIndex_].orders.front();
}

void OrderBook::popBestBid() {
    auto& q = bidLevels_[*bestBidIndex_].orders;
    Order* p = q.front();
    q.pop_front();
    orderIndex_.erase(p->id);
    pool_.deallocate(p); // return the slot so later adds can reuse it
    if (q.empty())
        recomputeBestBidFrom(*bestBidIndex_); // last order at the touch: find the next best bid
}

void OrderBook::popBestAsk() {
    auto& q = askLevels_[*bestAskIndex_].orders;
    Order* p = q.front();
    q.pop_front();
    orderIndex_.erase(p->id);
    pool_.deallocate(p);
    if (q.empty())
        recomputeBestAskFrom(*bestAskIndex_);
}

std::optional<Price> OrderBook::bestBid() const {
    // cached index — do not scan the array
    if (bestBidIndex_)
        return priceOf(*bestBidIndex_);
    return std::nullopt;
}

std::optional<Price> OrderBook::bestAsk() const {
    if (bestAskIndex_)
        return priceOf(*bestAskIndex_);
    return std::nullopt;
}

void OrderBook::reset() {
    // get ids out of orderIndex_
    std::vector<OrderId> ids;
    for (auto& [id, loc] : orderIndex_) {
        ids.push_back(id);
    }
    // call cancel for each id
    for (auto id : ids) {
        cancel(id);
    }
}