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

TEST(Matching, MultiLevelCross) {
    MatchingEngine engine;
    engine.submit(limitSell(10, 101, 4));
    engine.submit(limitSell(11, 102, 4));
    auto trades = engine.submit(limitBuy(20, 102, 6));

    ASSERT_EQ(trades.size(), 2u);
    EXPECT_EQ(trades[0].maker_id, 10);
    EXPECT_EQ(trades[0].taker_id, 20);
    EXPECT_EQ(trades[0].price, 101);
    EXPECT_EQ(trades[0].quantity, 4);
    EXPECT_EQ(trades[1].maker_id, 11);
    EXPECT_EQ(trades[1].taker_id, 20);
    EXPECT_EQ(trades[1].price, 102);
    EXPECT_EQ(trades[1].quantity, 2);

    EXPECT_FALSE(engine.book().bestBid().has_value());
    ASSERT_TRUE(engine.book().bestAsk().has_value());
    EXPECT_EQ(*engine.book().bestAsk(), 102);
    const Order* ask = engine.book().bestAskFront();
    ASSERT_NE(ask, nullptr);
    EXPECT_EQ(ask->id, 11);
    EXPECT_EQ(ask->quantity, 2);
}

TEST(Matching, MultiLevelStopsAtLimit) {
    MatchingEngine engine;
    engine.submit(limitSell(10, 101, 4));
    engine.submit(limitSell(11, 102, 4));
    auto trades = engine.submit(limitBuy(20, 101, 6));

    ASSERT_EQ(trades.size(), 1u);
    EXPECT_EQ(trades[0].maker_id, 10);
    EXPECT_EQ(trades[0].price, 101);
    EXPECT_EQ(trades[0].quantity, 4);

    ASSERT_TRUE(engine.book().bestBid().has_value());
    EXPECT_EQ(*engine.book().bestBid(), 101);
    const Order* bid = engine.book().bestBidFront();
    ASSERT_NE(bid, nullptr);
    EXPECT_EQ(bid->id, 20);
    EXPECT_EQ(bid->quantity, 2);

    ASSERT_TRUE(engine.book().bestAsk().has_value());
    EXPECT_EQ(*engine.book().bestAsk(), 102);
    const Order* ask = engine.book().bestAskFront();
    ASSERT_NE(ask, nullptr);
    EXPECT_EQ(ask->id, 11);
    EXPECT_EQ(ask->quantity, 4);
}

TEST(Matching, CancelRestingThenIncomingRests) {
    MatchingEngine engine;
    engine.submit(limitSell(1, 100, 10));
    EXPECT_TRUE(engine.cancel(1));
    EXPECT_FALSE(engine.book().bestAsk().has_value());

    auto trades = engine.submit(limitBuy(2, 100, 10));
    EXPECT_TRUE(trades.empty());
    ASSERT_TRUE(engine.book().bestBid().has_value());
    EXPECT_EQ(*engine.book().bestBid(), 100);
    EXPECT_FALSE(engine.book().bestAsk().has_value());
}

TEST(Matching, CancelAlreadyFilledOrUnknown) {
    MatchingEngine engine;
    engine.submit(limitSell(1, 100, 10));
    engine.submit(limitBuy(2, 100, 10));

    EXPECT_FALSE(engine.cancel(1));
    EXPECT_FALSE(engine.cancel(999));
}

TEST(Matching, MarketAgainstEmptyBook) {
    MatchingEngine engine;
    auto trades = engine.submit(marketBuy(1, 10));

    EXPECT_TRUE(trades.empty());
    EXPECT_FALSE(engine.book().bestBid().has_value());
    EXPECT_FALSE(engine.book().bestAsk().has_value());
}

TEST(Matching, PriceTimePriorityFifo) {
    MatchingEngine engine;
    engine.submit(limitSell(1, 50, 3));
    engine.submit(limitSell(2, 50, 3));
    auto trades = engine.submit(limitBuy(3, 50, 4));

    ASSERT_EQ(trades.size(), 2u);
    EXPECT_EQ(trades[0].maker_id, 1);
    EXPECT_EQ(trades[0].quantity, 3);
    EXPECT_EQ(trades[1].maker_id, 2);
    EXPECT_EQ(trades[1].quantity, 1);

    const Order* ask = engine.book().bestAskFront();
    ASSERT_NE(ask, nullptr);
    EXPECT_EQ(ask->id, 2);
    EXPECT_EQ(ask->quantity, 2);
}

TEST(Matching, BestBidAskAfterInsertsAndCancels) {
    MatchingEngine engine;
    engine.submit(limitBuy(1, 100, 10));
    engine.submit(limitBuy(2, 105, 10));
    engine.submit(limitSell(3, 110, 10));

    ASSERT_TRUE(engine.book().bestBid().has_value());
    EXPECT_EQ(*engine.book().bestBid(), 105);
    ASSERT_TRUE(engine.book().bestAsk().has_value());
    EXPECT_EQ(*engine.book().bestAsk(), 110);

    EXPECT_TRUE(engine.cancel(2));
    ASSERT_TRUE(engine.book().bestBid().has_value());
    EXPECT_EQ(*engine.book().bestBid(), 100);

    EXPECT_TRUE(engine.cancel(3));
    EXPECT_FALSE(engine.book().bestAsk().has_value());
}

TEST(Matching, MarketLeftoverDropped) {
    MatchingEngine engine;
    engine.submit(limitSell(1, 100, 4));
    auto trades = engine.submit(marketBuy(2, 10));

    ASSERT_EQ(trades.size(), 1u);
    EXPECT_EQ(trades[0].maker_id, 1);
    EXPECT_EQ(trades[0].quantity, 4);
    EXPECT_FALSE(engine.book().bestBid().has_value());
    EXPECT_FALSE(engine.book().bestAsk().has_value());
}

TEST(Matching, NoCrossBothRest) {
    MatchingEngine engine;
    auto buyTrades = engine.submit(limitBuy(1, 100, 10));
    auto sellTrades = engine.submit(limitSell(2, 101, 10));

    EXPECT_TRUE(buyTrades.empty());
    EXPECT_TRUE(sellTrades.empty());
    ASSERT_TRUE(engine.book().bestBid().has_value());
    EXPECT_EQ(*engine.book().bestBid(), 100);
    ASSERT_TRUE(engine.book().bestAsk().has_value());
    EXPECT_EQ(*engine.book().bestAsk(), 101);
}

TEST(Matching, PriceImprovementUsesMakerPrice) {
    MatchingEngine engine;
    engine.submit(limitSell(1, 101, 10));
    auto trades = engine.submit(limitBuy(2, 105, 10));

    ASSERT_EQ(trades.size(), 1u);
    EXPECT_EQ(trades[0].price, 101);
    EXPECT_FALSE(engine.book().bestBid().has_value());
    EXPECT_FALSE(engine.book().bestAsk().has_value());
}

TEST(Matching, SellWalksBidsBestFirst) {
    MatchingEngine engine;
    engine.submit(limitBuy(1, 102, 3));
    engine.submit(limitBuy(2, 100, 10));
    auto trades = engine.submit(limitSell(3, 100, 5));

    ASSERT_EQ(trades.size(), 2u);
    EXPECT_EQ(trades[0].maker_id, 1);
    EXPECT_EQ(trades[0].price, 102);
    EXPECT_EQ(trades[0].quantity, 3);
    EXPECT_EQ(trades[1].maker_id, 2);
    EXPECT_EQ(trades[1].price, 100);
    EXPECT_EQ(trades[1].quantity, 2);

    const Order* bid = engine.book().bestBidFront();
    ASSERT_NE(bid, nullptr);
    EXPECT_EQ(bid->id, 2);
    EXPECT_EQ(bid->quantity, 8);
    EXPECT_FALSE(engine.book().bestAsk().has_value());
}
