#pragma once

#include "Order.h"

#include <cstddef>
#include <vector>

class OrderPool{
public:
    static constexpr std::size_t kMaxOrders = 100'000; // constant is known at compile time that belongs to class, not object
    OrderPool(); // constructor
    Order* allocate();
    void deallocate(Order* o);

private:
    std::vector<Order> slots_; // heap buffer so OrderBook can live on the stack
    std::vector<std::size_t> free_; // list of unused slots
};
