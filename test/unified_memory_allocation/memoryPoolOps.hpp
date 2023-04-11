// Copyright (C) 2023 Intel Corporation
// SPDX-License-Identifier: MIT

#include "helpers.hpp"
#include "randomSizesAllocator.hpp"
#include <cstring>
#include <thread>
#include <vector>

#define NTHREADS 5

#ifndef UR_UMA_MEMORY_POOL_OPS_H
#define UR_UMA_MEMORY_POOL_OPS_H

using poolTest = uma::poolTest;

TEST_P(poolTest, allocFree) {
    static constexpr size_t allocSize = 64;
    auto *ptr = umaPoolMalloc(pool.get(), allocSize);
    ASSERT_NE(ptr, nullptr);
    std::memset(ptr, 0, allocSize);
    umaPoolFree(pool.get(), ptr);
}

TEST_P(poolTest, pow2AlignedAlloc) {
    static constexpr size_t allocSize = 64;
    static constexpr size_t maxAlignment = (1u << 22);

    for (size_t alignment = 1; alignment <= maxAlignment; alignment <<= 1) {
        std::cout << alignment << std::endl;
        auto *ptr = umaPoolAlignedMalloc(pool.get(), allocSize, alignment);
        ASSERT_NE(ptr, nullptr);
        std::memset(ptr, 0, allocSize);
        umaPoolFree(pool.get(), ptr);
    }
}

TEST_P(poolTest, allocZeroSize) {
    static constexpr size_t allocSize = 0;
    auto *ptr = umaPoolMalloc(pool.get(), allocSize);
    ASSERT_EQ(ptr, nullptr);
}

TEST_P(poolTest, allocLargestSize) {
    // alloc size > 2GB (allocations with size > 2GB bypass the pool and
    // go directly to the provider)
    static constexpr size_t allocSize = (size_t)1 << 32;
    auto *ptr = umaPoolMalloc(pool.get(), allocSize);
    ASSERT_NE(ptr, nullptr);
    std::memset(ptr, 0, allocSize);
    umaPoolFree(pool.get(), ptr);
}

TEST_P(poolTest, freeNullptr) {
    void *ptr = nullptr;
    umaPoolFree(pool.get(), ptr);
    ASSERT_EQ(ptr, nullptr);
}

TEST_P(poolTest, allocOutOfMem) {
    // test whether memory is kept in a pool accordingly to MaxSize
    static constexpr size_t allocSize = 16;
    // MaxSize equals 16 * 1024 * 1024;
    static constexpr size_t maxAllocSizeMultiplier = 1024 * 1024;

    // allocate until oom
    auto *ptr = umaPoolMalloc(pool.get(), allocSize);
    ASSERT_NE(ptr, nullptr);

    for(size_t i = 0; i <= maxAllocSizeMultiplier; ++i) {
        ptr = umaPoolMalloc(pool.get(), allocSize);
        ASSERT_NE(ptr, nullptr);
    }

    umaPoolFree(pool.get(), ptr);

    ptr = umaPoolMalloc(pool.get(), allocSize);
    ASSERT_NE(ptr, nullptr);
}

TEST_P(poolTest, allocRandomMemory) {
    static constexpr size_t min_size = 16;
    static constexpr size_t max_size = 64;
    static constexpr int max_allocations_number = 10;
    std::shared_ptr<RandomSizesAllocator> rand_alloc =
        std::make_shared<RandomSizesAllocator>(min_size, max_size, max_allocations_number);
    
    size_t allocated_size = rand_alloc->malloc_random_memory(pool.get());
    ASSERT_NE(allocated_size, 0);
}

TEST_P(poolTest, freeRandomMemory) {
    static constexpr size_t min_size = 16;
    static constexpr size_t max_size = 64;
    static constexpr int max_allocations_number = 10;
    std::shared_ptr<RandomSizesAllocator> rand_alloc =
        std::make_shared<RandomSizesAllocator>(min_size, max_size, max_allocations_number);

    size_t allocated_size = 0;
    for(size_t i = 0; i < max_allocations_number; ++i) {
        allocated_size = rand_alloc->malloc_random_memory(pool.get());
        ASSERT_NE(allocated_size, 0);
    }

    allocated_size = rand_alloc->free_random_memory();
    ASSERT_NE(allocated_size, 0);
}

TEST_P(poolTest, multiThreadedMallocFree) {
    auto poolMalloc=[](size_t allocSize, uma_memory_pool_t * pool) {
        auto *ptr = umaPoolMalloc(pool, allocSize);
        ASSERT_NE(ptr, nullptr);
        umaPoolFree(pool, ptr);
    };

    std::vector<std::thread> threads;
    for (int i = 0; i < NTHREADS; i++) {
        threads.push_back(std::thread(poolMalloc, 64, pool.get()));
    }

    for (auto &thread : threads) {
        thread.join();
    }

}

#endif /* UR_UMA_MEMORY_POOL_OPS_H */
