#pragma once

#include "Order.h"

#include <array>
#include <cstddef>
#include <vector>

class OrderPool{
public:
    static constexpr std::size_t kMaxOrders = 100'000; // constant is known at compile time that belongs to class, not object
    OrderPool(); // constructor
    Order* allocate();
    void deallocate(Order* o);

private:
    std::array<Order, kMaxOrders> slots_{}; // list of orders
    std::vector<std::size_t> free_; // list of unused slots
};