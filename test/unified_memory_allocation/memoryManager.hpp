// Copyright (C) 2023 Intel Corporation
// SPDX-License-Identifier: MIT

#include "helpers.hpp"

class MemoryManager
{
private:
    size_t memory_size;
    uma_memory_pool_t *memory_pool;
    void *memory_pointer;

    void move(MemoryManager &&other)
    {
        memory_size = other.memory_size;
        if (memory_pointer)
             umaPoolFree(memory_pool, memory_pointer);
        memory_pointer = std::move(other.memory_pointer);
        other.memory_pointer = nullptr;
        memory_pool = std::move(other.memory_pool);
        other.memory_pool = nullptr;
    }

public:
    MemoryManager(size_t size, uma_memory_pool_t * pool)
        : memory_size(size),
          memory_pool(pool),
          memory_pointer(umaPoolMalloc(pool, size))
    {
        if (!memory_pointer) {
            throw std::bad_alloc();
        }
    }

    size_t size()
    {
        return memory_size;
    }

    MemoryManager(const MemoryManager &) = delete;

    MemoryManager(MemoryManager &&other)
    {
        move(std::move(other));
    }

    MemoryManager &operator=(MemoryManager &&other)
    {
        move(std::move(other));
        return *this;
    }

    ~MemoryManager()
    {
        if (memory_pointer)
            umaPoolFree(memory_pool, memory_pointer);
    }
};
