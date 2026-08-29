#include "MatchingEngine.h"

std::vector<Trade> MatchingEngine::submit(Order order) {
    if (order.quantity <= 0) return {};
    std::vector<Trade> trades;

    while (order.quantity > 0) {
        // get the best order from the other side, break if no valid match
        Order* maker = order.side == Side::Buy ? book_.bestAskFront() : book_.bestBidFront();
        if (maker == nullptr) break;
        if (order.type == OrderType::Limit){
            if (order.side == Side::Buy && order.price < maker->price) break;
            if (order.side == Side::Sell && order.price > maker->price) break;
        }

        // fill the order and add the trade to the vector
        Quantity fill = std::min(order.quantity, maker->quantity);
        trades.push_back({ maker->id,order.id, maker->price, fill });
        order.quantity -= fill;
        maker->quantity -= fill;
        // if the maker's order is fully filled, remove it from the book
        if (maker->quantity == 0) {
            if (maker->side == Side::Buy) book_.popBestBid();
            else book_.popBestAsk();
        }
    }

    // if the order is not fully filled, add it to the book
    if (order.quantity > 0 && order.type == OrderType::Limit) {
        book_.add(order);
    }

    return trades;
}

bool MatchingEngine::cancel(OrderId id) {
    return book_.cancel(id);
}