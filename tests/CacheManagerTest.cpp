#include <gtest/gtest.h>

#include "ForgeSim/execution/CacheManager.hpp"

TEST(CacheManagerTest, EmptyCacheDoesNotContainNode)
{
    CacheManager cache;

    EXPECT_FALSE(
        cache.contains(1)
    );

    EXPECT_EQ(
        cache.size(),
        0
    );
}

TEST(CacheManagerTest, StoresAndRetrievesResult)
{
    CacheManager cache;

    Result result(123.45);

    cache.store(1, result);

    ASSERT_TRUE(
        cache.contains(1)
    );

    EXPECT_DOUBLE_EQ(
        cache.get(1).getValue(),
        123.45
    );

    EXPECT_EQ(
        cache.size(),
        1
    );
}

TEST(CacheManagerTest, InvalidateRemovesValidity)
{
    CacheManager cache;

    cache.store(
        1,
        Result(100.0)
    );

    ASSERT_TRUE(
        cache.contains(1)
    );

    cache.invalidate(1);

    EXPECT_FALSE(
        cache.contains(1)
    );
}

TEST(CacheManagerTest, ClearRemovesAllEntries)
{
    CacheManager cache;

    cache.store(1, Result(10.0));
    cache.store(2, Result(20.0));
    cache.store(3, Result(30.0));

    EXPECT_EQ(
        cache.size(),
        3
    );

    cache.clear();

    EXPECT_EQ(
        cache.size(),
        0
    );

    EXPECT_FALSE(cache.contains(1));
    EXPECT_FALSE(cache.contains(2));
    EXPECT_FALSE(cache.contains(3));
}
