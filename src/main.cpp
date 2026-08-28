#include "MatchingEngine.h"
#include <iostream>

int main() {
    MatchingEngine engine;
    Order o{1, Side::Buy, OrderType::Limit, 100, 10, 1};
    auto trades = engine.submit(o);
    std::cout << "trades=" << trades.size()
              << " bestBid? " << engine.book().bestBid().has_value() << '\n';
    return 0;
}