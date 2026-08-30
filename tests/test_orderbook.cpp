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

TEST(OrderBook, BestBidIsHighestPrice) {
    OrderBook book;
    book.add(buy(1, 100, 10));
    book.add(buy(2, 105, 4));

    ASSERT_TRUE(book.bestBid().has_value());
    EXPECT_EQ(*book.bestBid(), 105);
}

TEST(OrderBook, SamePriceFifo) {
    OrderBook book;
    book.add(sell(1, 101, 5));
    book.add(sell(2, 101, 5));

    Order* front = book.bestAskFront();
    ASSERT_NE(front, nullptr);
    EXPECT_EQ(front->id, 1);
}

TEST(OrderBook, CancelMovesFifoFront) {
    OrderBook book;
    book.add(sell(1, 101, 5));
    book.add(sell(2, 101, 5));

    EXPECT_TRUE(book.cancel(1));

    Order* front = book.bestAskFront();
    ASSERT_NE(front, nullptr);
    EXPECT_EQ(front->id, 2);
}

TEST(OrderBook, CancelUnknownId) {
    OrderBook book;
    EXPECT_FALSE(book.cancel(999));
}

TEST(OrderBook, OutOfRangeRestDoesNotBecomeBestBid) {
    OrderBook book;
    book.add(buy(1, OrderBook::kMinPrice + OrderBook::kPriceRange, 10));
    EXPECT_FALSE(book.bestBid().has_value());
}
