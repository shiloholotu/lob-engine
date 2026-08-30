#include "OrderPool.h"

OrderPool::OrderPool(){
    for(std::size_t i = 0; i < kMaxOrders; ++i){
        free_.push_back(i);
    }
}

Order* OrderPool::allocate(){
    // return nullptr if no more free slots
    if (free_.empty()) {
        return nullptr;
    }
    std::size_t i = free_.back();
    free_.pop_back(); // mark the back slot as no longer free
    return &slots_[i]; // return the address of the Order in the slot


}

void OrderPool::deallocate(Order* o) {
    std::size_t i = static_cast<std::size_t>(o - slots_.data()); // o - slots.data() means "how many Orders from the start of the array"
    free_.push_back(i);
}