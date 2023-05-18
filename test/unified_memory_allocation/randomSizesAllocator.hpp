// Copyright (C) 2023 Intel Corporation
// SPDX-License-Identifier: MIT

#include "helpers.hpp"
#include <random>
#include <vector>

class RandomSizesAllocator
{
private:
    std::vector<std::unique_ptr<void, std::function<void(void*)>>> allocated_memory;
    std::vector<size_t> mem_sizes;
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

    size_t malloc_random_memory(uma_memory_pool_handle_t pool)
    {
        size_t size = get_random_size();
        // create deleted function - capture pool and use it to call umaPoolFree.
        auto dtor = [pool = pool](void* ptr) {umaPoolFree(pool, ptr); }; 
        allocated_memory.emplace_back(umaPoolMalloc(pool, size), dtor);
        mem_sizes.emplace_back(size);
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
        auto mem_it = std::begin(mem_sizes) + random_index;
        size_t size = *mem_it;
        allocated_memory.erase(it);
        return size;
    }

    bool empty()
    {
        return allocated_memory.empty();
    }
};
