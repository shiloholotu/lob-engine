#include "OrderPool.h"
#include <gtest/gtest.h>

TEST(OrderPool, AllocateTwiceGivesDifferentSlots) {
    OrderPool pool;
    Order* a = pool.allocate();
    Order* b = pool.allocate();

    ASSERT_NE(a, nullptr);
    ASSERT_NE(b, nullptr);
    EXPECT_NE(a, b);
}

TEST(OrderPool, DeallocateThenAllocateReusesASlot) {
    OrderPool pool;
    Order* a = pool.allocate();
    ASSERT_NE(a, nullptr);

    pool.deallocate(a);

    Order* b = pool.allocate();
    ASSERT_NE(b, nullptr);
    EXPECT_EQ(b, a);  // you pop_back, so the last freed index is reused first
}