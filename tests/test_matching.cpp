#include "MatchingEngine.h"
#include <gtest/gtest.h>

Order limitBuy(OrderId id, Price price, Quantity qty) {
    return Order{id, Side::Buy, OrderType::Limit, price, qty, id};
}

Order limitSell(OrderId id, Price price, Quantity qty) {
    return Order{id, Side::Sell, OrderType::Limit, price, qty, id};
}

Order marketBuy(OrderId id, Quantity qty) {
    return Order{id, Side::Buy, OrderType::Market, 0, qty, id};
}

Order marketSell(OrderId id, Quantity qty) {
    return Order{id, Side::Sell, OrderType::Market, 0, qty, id};
}

TEST(Matching, FullFill) {
    MatchingEngine engine;
    engine.submit(limitSell(1, 100, 10));
    auto trades = engine.submit(limitBuy(2, 100, 10));

    ASSERT_EQ(trades.size(), 1u);
    EXPECT_EQ(trades[0].maker_id, 1);
    EXPECT_EQ(trades[0].taker_id, 2);
    EXPECT_EQ(trades[0].price, 100);
    EXPECT_EQ(trades[0].quantity, 10);
    EXPECT_FALSE(engine.book().bestBid().has_value());
    EXPECT_FALSE(engine.book().bestAsk().has_value());
}

TEST(Matching, PartialFillRests) {
    MatchingEngine engine;
    engine.submit(limitSell(1, 101, 5));
    auto trades = engine.submit(limitBuy(2, 101, 12));

    ASSERT_EQ(trades.size(), 1u);
    EXPECT_EQ(trades[0].maker_id, 1);
    EXPECT_EQ(trades[0].taker_id, 2);
    EXPECT_EQ(trades[0].price, 101);
    EXPECT_EQ(trades[0].quantity, 5);

    ASSERT_TRUE(engine.book().bestBid().has_value());
    EXPECT_EQ(*engine.book().bestBid(), 101);
    const Order* bid = engine.book().bestBidFront();
    ASSERT_NE(bid, nullptr);
    EXPECT_EQ(bid->id, 2);
    EXPECT_EQ(bid->quantity, 7);
    EXPECT_FALSE(engine.book().bestAsk().has_value());
}

