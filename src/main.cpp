#include "Order.h"
#include <iostream>
#include <map>
#include <deque>
#include <unordered_map>

struct OrderLocation {
    Side  side;
    Price price;
};

void restAsk(std::map<Price, std::deque<Order>>& asks,
             std::unordered_map<OrderId, OrderLocation>& index,
             Order o) {
    asks[o.price].push_back(o);
    index[o.id] = OrderLocation{Side::Sell, o.price};
}

int main() {
    std::map<Price, std::deque<Order>> asks;  // begin() = best ask
    std::unordered_map<OrderId, OrderLocation> index;

    restAsk(asks, index, Order{1, Side::Sell, OrderType::Limit, 101, 5, 1});
    restAsk(asks, index, Order{2, Side::Sell, OrderType::Limit, 101, 5, 2});
    restAsk(asks, index, Order{3, Side::Sell, OrderType::Limit, 102, 9, 3});

    std::cout << "best ask " << asks.begin()->first << '\n';           // 101
    std::cout << "front qty " << asks[101].front().quantity << '\n';   // 5 (id=1)

    // FIFO: first at 101 should be id 1
    std::cout << "front id " << asks[101].front().id << '\n';

    // cancel id 1 using the index (do not scan price 102)
    OrderId kill = 1;
    OrderLocation loc = index.at(kill);
    std::deque<Order>& q = asks[loc.price];

    for (auto it = q.begin(); it != q.end(); ++it) {
        if (it->id == kill) {
            q.erase(it);
            break;
        }
    }
    index.erase(kill);
    if (q.empty()) {
        asks.erase(loc.price);
    }

    std::cout << "new front id " << asks[101].front().id << '\n';  // 2
    return 0;
}