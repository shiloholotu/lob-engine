#include "OrderBook.h"
#include <gtest/gtest.h>

Order buy(OrderId id, Price price, Quantity qty) {
    return Order{id, Side::Buy, OrderType::Limit, price, qty, id};
}

Order sell(OrderId id, Price price, Quantity qty) {
    return Order{id, Side::Sell, OrderType::Limit, price, qty, id};
}

TEST(OrderBook, AddBuySetsBestBid) {
    OrderBook book;
    book.add(buy(1, 100, 10));

    ASSERT_TRUE(book.bestBid().has_value());
    EXPECT_EQ(*book.bestBid(), 100);
    EXPECT_FALSE(book.bestAsk().has_value());
}

