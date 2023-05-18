// Copyright (C) 2023 Intel Corporation
// SPDX-License-Identifier: MIT

#include "helpers.hpp"
#include "disjoint_pool.hpp"

static DisjointPool::Config poolConfig() {
    DisjointPool::Config config{};
    config.SlabMinSize = 4096;
    config.MaxPoolableSize = 4096;
    config.Capacity = 4;
    config.MinBucketSize = 64;
    return config;
}

TEST(disjointPoolLimits, disjointPoolSlabMinSizeLimit) {
    //auto pool = uma::poolMakeUnique<DisjointPool>(uma::wrapProviderUnique(mockProviderCreate()), poolConfig());
    // I need to get config parameters from provider functions but how
}