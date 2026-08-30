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

