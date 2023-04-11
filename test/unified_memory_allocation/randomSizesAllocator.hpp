// Copyright (C) 2023 Intel Corporation
// SPDX-License-Identifier: MIT

#include "memoryManager.hpp"
#include "helpers.hpp"
#include <random>
#include <vector>

class RandomSizesAllocator
{
private:
    std::vector<MemoryManager> allocated_memory;
    std::default_random_engine generator;
    std::uniform_int_distribution<int> memory_distribution;

    size_t get_random_size()
    {
        return memory_distribution(generator);
    }

public:
    RandomSizesAllocator(size_t min_size, size_t max_size,
                         int max_allocations_number)
        : memory_distribution(min_size, max_size)
    {
        allocated_memory.reserve(max_allocations_number);
    }

    size_t malloc_random_memory(uma_memory_pool_t * pool)
    {
        size_t size = get_random_size();
        allocated_memory.emplace_back(size, pool);
        return size;
    }

    size_t free_random_memory()
    {
        if (empty())
            return 0;
        std::uniform_int_distribution<int> distribution(
            0, allocated_memory.size() - 1);
        int random_index = distribution(generator);
        auto it = std::begin(allocated_memory) + random_index;
        size_t size = it->size();
        allocated_memory.erase(it);
        return size;
    }

    bool empty()
    {
        return allocated_memory.empty();
    }
};