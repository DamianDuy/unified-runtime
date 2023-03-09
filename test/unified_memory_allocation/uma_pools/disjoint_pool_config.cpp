// Copyright (C) 2023 Intel Corporation
// SPDX-License-Identifier: MIT

#include "helpers.hpp"
#include "usm_pool_config.hpp"

using PoolDescriptor = usm_pool::PoolDescriptor;

const PoolDescriptor DESCRIPTORS[4] = {PoolDescriptor(UR_USM_TYPE_HOST),
                                       PoolDescriptor(UR_USM_TYPE_DEVICE),
                                       PoolDescriptor(UR_USM_TYPE_SHARED),
                                       PoolDescriptor(UR_USM_TYPE_SHARED, true)};

TEST(disjointPoolConfigTests, disjointPoolConfigTest) {
    // test if the returned unordered map contains only the valid configs
    usm_pool::PoolConfigurations<typename DisjointPool::Config> configsMap =
        usm_pool::getPoolConfigurationsFor<DisjointPool>();

    // check if all the valid configs are present
    for (const auto &descriptor : DESCRIPTORS) {
        ASSERT_EQ(configsMap.count(descriptor), 1);
    }

    // check if there are no more configs in the unordered map
    ASSERT_EQ(configsMap.size(), 4);
}

TEST(disjointPoolConfigTests, disjointPoolConfigStringEnabledBuffersTest) {
    // test for valid string with enabled buffers-- (values to configs should be
    // parsed from string)
    std::string config = "1;32M;host:1M,4,64k;device:1m,4,64K;shared:0,3,1M;"
                         "read_only_shared:0,0,3M";
    usm_pool::PoolConfigurations<typename DisjointPool::Config> configsMap =
        usm_pool::getPoolConfigurationsFor<DisjointPool>(config);

    // test for host
    ASSERT_EQ(configsMap.at(DESCRIPTORS[0]).MaxPoolableSize, 1 * 1024 * 1024);
    ASSERT_EQ(configsMap.at(DESCRIPTORS[0]).Capacity, 4);
    ASSERT_EQ(configsMap.at(DESCRIPTORS[0]).SlabMinSize, 64 * 1024);
    ASSERT_EQ(configsMap.at(DESCRIPTORS[0]).limits->MaxSize, 32 * 1024 * 1024);

    // test for device
    ASSERT_EQ(configsMap.at(DESCRIPTORS[1]).MaxPoolableSize, 1 * 1024 * 1024);
    ASSERT_EQ(configsMap.at(DESCRIPTORS[1]).Capacity, 4);
    ASSERT_EQ(configsMap.at(DESCRIPTORS[1]).SlabMinSize, 64 * 1024);
    ASSERT_EQ(configsMap.at(DESCRIPTORS[1]).limits->MaxSize, 32 * 1024 * 1024);

    // test for shared
    ASSERT_EQ(configsMap.at(DESCRIPTORS[2]).MaxPoolableSize, 0);
    ASSERT_EQ(configsMap.at(DESCRIPTORS[2]).Capacity, 3);
    ASSERT_EQ(configsMap.at(DESCRIPTORS[2]).SlabMinSize, 1 * 1024 * 1024);
    ASSERT_EQ(configsMap.at(DESCRIPTORS[2]).limits->MaxSize, 32 * 1024 * 1024);

    // test for read-only shared
    ASSERT_EQ(configsMap.at(DESCRIPTORS[3]).MaxPoolableSize, 0);
    ASSERT_EQ(configsMap.at(DESCRIPTORS[3]).Capacity, 0);
    ASSERT_EQ(configsMap.at(DESCRIPTORS[3]).SlabMinSize, 3 * 1024 * 1024);
    ASSERT_EQ(configsMap.at(DESCRIPTORS[3]).limits->MaxSize, 32 * 1024 * 1024);
}

TEST(disjointPoolConfigTests, disjointPoolConfigStringImpartialEnabledBuffersTest) {
    // test for valid impartial string with enabled buffers-- (values to configs should be
    // parsed from string if present, the rest should be default
    // set by getConfigurationsFor)
    std::string config = "1;32M;host:1M,4,64k;shared:0,3,1M;";
    usm_pool::PoolConfigurations<typename DisjointPool::Config> configsMap =
        usm_pool::getPoolConfigurationsFor<DisjointPool>(config);

    // test for host
    ASSERT_EQ(configsMap.at(DESCRIPTORS[0]).MaxPoolableSize, 1 * 1024 * 1024);
    ASSERT_EQ(configsMap.at(DESCRIPTORS[0]).Capacity, 4);
    ASSERT_EQ(configsMap.at(DESCRIPTORS[0]).SlabMinSize, 64 * 1024);
    ASSERT_EQ(configsMap.at(DESCRIPTORS[0]).limits->MaxSize, 32 * 1024 * 1024);

    // test for device
    ASSERT_EQ(configsMap.at(DESCRIPTORS[1]).MaxPoolableSize, 4 * 1024 * 1024);
    ASSERT_EQ(configsMap.at(DESCRIPTORS[1]).Capacity, 4);
    ASSERT_EQ(configsMap.at(DESCRIPTORS[1]).SlabMinSize, 64 * 1024);
    ASSERT_EQ(configsMap.at(DESCRIPTORS[1]).limits->MaxSize, 32 * 1024 * 1024);

    // test for shared
    ASSERT_EQ(configsMap.at(DESCRIPTORS[2]).MaxPoolableSize, 0);
    ASSERT_EQ(configsMap.at(DESCRIPTORS[2]).Capacity, 3);
    ASSERT_EQ(configsMap.at(DESCRIPTORS[2]).SlabMinSize, 1 * 1024 * 1024);
    ASSERT_EQ(configsMap.at(DESCRIPTORS[2]).limits->MaxSize, 32 * 1024 * 1024);

    // test for read-only shared
    ASSERT_EQ(configsMap.at(DESCRIPTORS[3]).MaxPoolableSize, 4 * 1024 * 1024);
    ASSERT_EQ(configsMap.at(DESCRIPTORS[3]).Capacity, 4);
    ASSERT_EQ(configsMap.at(DESCRIPTORS[3]).SlabMinSize, 2 * 1024 * 1024);
    ASSERT_EQ(configsMap.at(DESCRIPTORS[3]).limits->MaxSize, 32 * 1024 * 1024);
}

TEST(disjointPoolConfigTests, disjointPoolConfigStringDisabledBuffersTest) {
    // test for valid string with disabled buffers-- (the map should be empty)
    std::string config = "0;32M;host:1M,4,64k;device:1m,4,64K;shared:0,3,2M";
    usm_pool::PoolConfigurations<typename DisjointPool::Config> configsMap =
        usm_pool::getPoolConfigurationsFor<DisjointPool>(config);

    ASSERT_TRUE(configsMap.empty());
}

TEST(disjointPoolConfigTests, disjointPoolConfigStringEmpty) {
    // test for empty string-- (all the values should be default
    // set by getConfigurationsFor)
    std::string config = "";
    usm_pool::PoolConfigurations<typename DisjointPool::Config> configsMap =
        usm_pool::getPoolConfigurationsFor<DisjointPool>(config);

    // test for host
    ASSERT_EQ(configsMap.at(DESCRIPTORS[0]).MaxPoolableSize, 2 * 1024 * 1024);
    ASSERT_EQ(configsMap.at(DESCRIPTORS[0]).Capacity, 4);
    ASSERT_EQ(configsMap.at(DESCRIPTORS[0]).SlabMinSize, 64 * 1024);
    ASSERT_EQ(configsMap.at(DESCRIPTORS[0]).limits->MaxSize, 16 * 1024 * 1024);

    // test for device
    ASSERT_EQ(configsMap.at(DESCRIPTORS[1]).MaxPoolableSize, 4 * 1024 * 1024);
    ASSERT_EQ(configsMap.at(DESCRIPTORS[1]).Capacity, 4);
    ASSERT_EQ(configsMap.at(DESCRIPTORS[1]).SlabMinSize, 64 * 1024);
    ASSERT_EQ(configsMap.at(DESCRIPTORS[1]).limits->MaxSize, 16 * 1024 * 1024);

    // test for shared
    ASSERT_EQ(configsMap.at(DESCRIPTORS[2]).MaxPoolableSize, 0);
    ASSERT_EQ(configsMap.at(DESCRIPTORS[2]).Capacity, 0);
    ASSERT_EQ(configsMap.at(DESCRIPTORS[2]).SlabMinSize, 2 * 1024 * 1024);
    ASSERT_EQ(configsMap.at(DESCRIPTORS[2]).limits->MaxSize, 16 * 1024 * 1024);

    // test for read-only shared
    ASSERT_EQ(configsMap.at(DESCRIPTORS[3]).MaxPoolableSize, 4 * 1024 * 1024);
    ASSERT_EQ(configsMap.at(DESCRIPTORS[3]).Capacity, 4);
    ASSERT_EQ(configsMap.at(DESCRIPTORS[3]).SlabMinSize, 2 * 1024 * 1024);
    ASSERT_EQ(configsMap.at(DESCRIPTORS[3]).limits->MaxSize, 16 * 1024 * 1024);
}

TEST(disjointPoolConfigTests, disjointPoolConfigStringInvalid) {
    // test for invalid string-- (all the values should be default
    // set by getConfigurationsFor)
    std::string config = "ab12cdefghi34jk56lmn78opr910";
    usm_pool::PoolConfigurations<typename DisjointPool::Config> configsMap =
        usm_pool::getPoolConfigurationsFor<DisjointPool>(config);

    // test for host
    ASSERT_EQ(configsMap.at(DESCRIPTORS[0]).MaxPoolableSize, 2 * 1024 * 1024);
    ASSERT_EQ(configsMap.at(DESCRIPTORS[0]).Capacity, 4);
    ASSERT_EQ(configsMap.at(DESCRIPTORS[0]).SlabMinSize, 64 * 1024);
    ASSERT_EQ(configsMap.at(DESCRIPTORS[0]).limits->MaxSize, 16 * 1024 * 1024);

    // test for device
    ASSERT_EQ(configsMap.at(DESCRIPTORS[1]).MaxPoolableSize, 4 * 1024 * 1024);
    ASSERT_EQ(configsMap.at(DESCRIPTORS[1]).Capacity, 4);
    ASSERT_EQ(configsMap.at(DESCRIPTORS[1]).SlabMinSize, 64 * 1024);
    ASSERT_EQ(configsMap.at(DESCRIPTORS[1]).limits->MaxSize, 16 * 1024 * 1024);

    // test for shared
    ASSERT_EQ(configsMap.at(DESCRIPTORS[2]).MaxPoolableSize, 0);
    ASSERT_EQ(configsMap.at(DESCRIPTORS[2]).Capacity, 0);
    ASSERT_EQ(configsMap.at(DESCRIPTORS[2]).SlabMinSize, 2 * 1024 * 1024);
    ASSERT_EQ(configsMap.at(DESCRIPTORS[2]).limits->MaxSize, 16 * 1024 * 1024);

    // test for read-only shared
    ASSERT_EQ(configsMap.at(DESCRIPTORS[3]).MaxPoolableSize, 4 * 1024 * 1024);
    ASSERT_EQ(configsMap.at(DESCRIPTORS[3]).Capacity, 4);
    ASSERT_EQ(configsMap.at(DESCRIPTORS[3]).SlabMinSize, 2 * 1024 * 1024);
    ASSERT_EQ(configsMap.at(DESCRIPTORS[3]).limits->MaxSize, 16 * 1024 * 1024);
}

TEST(disjointPoolConfigTests, disjointPoolConfigStringInvalidMemType) {
    // test for a string with invalid memory type-- (since the string is
    // invalid, values should be set to default but this test doesn't pass yet)
    std::string config = "1;32M;foo:0,3,2m;device:1M,4,64k";
    usm_pool::PoolConfigurations<typename DisjointPool::Config> configsMap =
        usm_pool::getPoolConfigurationsFor<DisjointPool>(config);

    // test for host
    ASSERT_EQ(configsMap.at(DESCRIPTORS[0]).MaxPoolableSize, 2 * 1024 * 1024);
    ASSERT_EQ(configsMap.at(DESCRIPTORS[0]).Capacity, 4);
    ASSERT_EQ(configsMap.at(DESCRIPTORS[0]).SlabMinSize, 64 * 1024);
    ASSERT_EQ(configsMap.at(DESCRIPTORS[0]).limits->MaxSize, 16 * 1024 * 1024);

    // test for device
    ASSERT_EQ(configsMap.at(DESCRIPTORS[1]).MaxPoolableSize, 4 * 1024 * 1024);
    ASSERT_EQ(configsMap.at(DESCRIPTORS[1]).Capacity, 4);
    ASSERT_EQ(configsMap.at(DESCRIPTORS[1]).SlabMinSize, 64 * 1024);
    ASSERT_EQ(configsMap.at(DESCRIPTORS[1]).limits->MaxSize, 16 * 1024 * 1024);

    // test for shared
    ASSERT_EQ(configsMap.at(DESCRIPTORS[2]).MaxPoolableSize, 0);
    ASSERT_EQ(configsMap.at(DESCRIPTORS[2]).Capacity, 0);
    ASSERT_EQ(configsMap.at(DESCRIPTORS[2]).SlabMinSize, 2 * 1024 * 1024);
    ASSERT_EQ(configsMap.at(DESCRIPTORS[2]).limits->MaxSize, 16 * 1024 * 1024);

    // test for read-only shared
    ASSERT_EQ(configsMap.at(DESCRIPTORS[3]).MaxPoolableSize, 4 * 1024 * 1024);
    ASSERT_EQ(configsMap.at(DESCRIPTORS[3]).Capacity, 4);
    ASSERT_EQ(configsMap.at(DESCRIPTORS[3]).SlabMinSize, 2 * 1024 * 1024);
    ASSERT_EQ(configsMap.at(DESCRIPTORS[3]).limits->MaxSize, 16 * 1024 * 1024);
}

TEST(disjointPoolConfigTests, disjointPoolConfigStringInvalidBegin) {
    // test for a string with invalid beginning-- (since the string is
    // invalid, values should be set to default but this test doesn't pass yet)
    std::string config = "132M;host:4m,3,2m,4m;device:1M,4,64k";
    usm_pool::PoolConfigurations<typename DisjointPool::Config> configsMap =
        usm_pool::getPoolConfigurationsFor<DisjointPool>(config);

    // test for host
    ASSERT_EQ(configsMap.at(DESCRIPTORS[0]).MaxPoolableSize, 2 * 1024 * 1024);
    ASSERT_EQ(configsMap.at(DESCRIPTORS[0]).Capacity, 4);
    ASSERT_EQ(configsMap.at(DESCRIPTORS[0]).SlabMinSize, 64 * 1024);
    ASSERT_EQ(configsMap.at(DESCRIPTORS[0]).limits->MaxSize, 16 * 1024 * 1024);

    // test for device
    ASSERT_EQ(configsMap.at(DESCRIPTORS[1]).MaxPoolableSize, 4 * 1024 * 1024);
    ASSERT_EQ(configsMap.at(DESCRIPTORS[1]).Capacity, 4);
    ASSERT_EQ(configsMap.at(DESCRIPTORS[1]).SlabMinSize, 64 * 1024);
    ASSERT_EQ(configsMap.at(DESCRIPTORS[1]).limits->MaxSize, 16 * 1024 * 1024);

    // test for shared
    ASSERT_EQ(configsMap.at(DESCRIPTORS[2]).MaxPoolableSize, 0);
    ASSERT_EQ(configsMap.at(DESCRIPTORS[2]).Capacity, 0);
    ASSERT_EQ(configsMap.at(DESCRIPTORS[2]).SlabMinSize, 2 * 1024 * 1024);
    ASSERT_EQ(configsMap.at(DESCRIPTORS[2]).limits->MaxSize, 16 * 1024 * 1024);

    // test for read-only shared
    ASSERT_EQ(configsMap.at(DESCRIPTORS[3]).MaxPoolableSize, 4 * 1024 * 1024);
    ASSERT_EQ(configsMap.at(DESCRIPTORS[3]).Capacity, 4);
    ASSERT_EQ(configsMap.at(DESCRIPTORS[3]).SlabMinSize, 2 * 1024 * 1024);
    ASSERT_EQ(configsMap.at(DESCRIPTORS[3]).limits->MaxSize, 16 * 1024 * 1024);
}

TEST(disjointPoolConfigTests, disjointPoolConfigStringPartiallyInvalid) {
    // test for a partially valid and partially invalid string-- (since the string is
    // invalid, values should be set to default but this test doesn't pass yet)
    std::string config = "1;32M;abdc123;;;device:1M,4,64k";
    usm_pool::PoolConfigurations<typename DisjointPool::Config> configsMap =
        usm_pool::getPoolConfigurationsFor<DisjointPool>(config);

    // test for host
    ASSERT_EQ(configsMap.at(DESCRIPTORS[0]).MaxPoolableSize, 2 * 1024 * 1024);
    ASSERT_EQ(configsMap.at(DESCRIPTORS[0]).Capacity, 4);
    ASSERT_EQ(configsMap.at(DESCRIPTORS[0]).SlabMinSize, 64 * 1024);
    ASSERT_EQ(configsMap.at(DESCRIPTORS[0]).limits->MaxSize, 16 * 1024 * 1024);

    // test for device
    ASSERT_EQ(configsMap.at(DESCRIPTORS[1]).MaxPoolableSize, 4 * 1024 * 1024);
    ASSERT_EQ(configsMap.at(DESCRIPTORS[1]).Capacity, 4);
    ASSERT_EQ(configsMap.at(DESCRIPTORS[1]).SlabMinSize, 64 * 1024);
    ASSERT_EQ(configsMap.at(DESCRIPTORS[1]).limits->MaxSize, 16 * 1024 * 1024);

    // test for shared
    ASSERT_EQ(configsMap.at(DESCRIPTORS[2]).MaxPoolableSize, 0);
    ASSERT_EQ(configsMap.at(DESCRIPTORS[2]).Capacity, 0);
    ASSERT_EQ(configsMap.at(DESCRIPTORS[2]).SlabMinSize, 2 * 1024 * 1024);
    ASSERT_EQ(configsMap.at(DESCRIPTORS[2]).limits->MaxSize, 16 * 1024 * 1024);

    // test for read-only shared
    ASSERT_EQ(configsMap.at(DESCRIPTORS[3]).MaxPoolableSize, 4 * 1024 * 1024);
    ASSERT_EQ(configsMap.at(DESCRIPTORS[3]).Capacity, 4);
    ASSERT_EQ(configsMap.at(DESCRIPTORS[3]).SlabMinSize, 2 * 1024 * 1024);
    ASSERT_EQ(configsMap.at(DESCRIPTORS[3]).limits->MaxSize, 16 * 1024 * 1024);
}

TEST(disjointPoolConfigTests, disjointPoolConfigStringInvalidEnabledBuffers) {
    // test for a string with invalid parameter for EnabledBuffers--
    // (it should be set to a default)
    std::string config = "-5;32M;host:0,3,2m,4m;device:1M,4,64k";
    usm_pool::PoolConfigurations<typename DisjointPool::Config> configsMap =
        usm_pool::getPoolConfigurationsFor<DisjointPool>(config);

    // test for host
    ASSERT_EQ(configsMap.at(DESCRIPTORS[0]).MaxPoolableSize, 0);
    ASSERT_EQ(configsMap.at(DESCRIPTORS[0]).Capacity, 3);
    ASSERT_EQ(configsMap.at(DESCRIPTORS[0]).SlabMinSize, 2 * 1024 * 1024);
    ASSERT_EQ(configsMap.at(DESCRIPTORS[0]).limits->MaxSize, 32 * 1024 * 1024);

    // test for device
    ASSERT_EQ(configsMap.at(DESCRIPTORS[1]).MaxPoolableSize, 1 * 1024 * 1024);
    ASSERT_EQ(configsMap.at(DESCRIPTORS[1]).Capacity, 4);
    ASSERT_EQ(configsMap.at(DESCRIPTORS[1]).SlabMinSize, 64 * 1024);
    ASSERT_EQ(configsMap.at(DESCRIPTORS[1]).limits->MaxSize, 32 * 1024 * 1024);

    // test for shared
    ASSERT_EQ(configsMap.at(DESCRIPTORS[2]).MaxPoolableSize, 0);
    ASSERT_EQ(configsMap.at(DESCRIPTORS[2]).Capacity, 0);
    ASSERT_EQ(configsMap.at(DESCRIPTORS[2]).SlabMinSize, 2 * 1024 * 1024);
    ASSERT_EQ(configsMap.at(DESCRIPTORS[2]).limits->MaxSize, 32 * 1024 * 1024);

    // test for read-only shared
    ASSERT_EQ(configsMap.at(DESCRIPTORS[3]).MaxPoolableSize, 4 * 1024 * 1024);
    ASSERT_EQ(configsMap.at(DESCRIPTORS[3]).Capacity, 4);
    ASSERT_EQ(configsMap.at(DESCRIPTORS[3]).SlabMinSize, 2 * 1024 * 1024);
    ASSERT_EQ(configsMap.at(DESCRIPTORS[3]).limits->MaxSize, 32 * 1024 * 1024);
}

TEST(disjointPoolConfigTests, disjointPoolConfigStringTooManyParameters) {
    // test for when too many parameters are passed-- (the extra parameters
    // should be ignored)
    std::string config = "1;32M;host:0,3,2m,4;device:1M,4,64k,5";
    usm_pool::PoolConfigurations<typename DisjointPool::Config> configsMap =
        usm_pool::getPoolConfigurationsFor<DisjointPool>(config);

    // test for host
    ASSERT_EQ(configsMap.at(DESCRIPTORS[0]).MaxPoolableSize, 0);
    ASSERT_EQ(configsMap.at(DESCRIPTORS[0]).Capacity, 3);
    ASSERT_EQ(configsMap.at(DESCRIPTORS[0]).SlabMinSize, 2 * 1024 * 1024);
    ASSERT_EQ(configsMap.at(DESCRIPTORS[0]).limits->MaxSize, 32 * 1024 * 1024);

    // test for device
    ASSERT_EQ(configsMap.at(DESCRIPTORS[1]).MaxPoolableSize, 1 * 1024 * 1024);
    ASSERT_EQ(configsMap.at(DESCRIPTORS[1]).Capacity, 4);
    ASSERT_EQ(configsMap.at(DESCRIPTORS[1]).SlabMinSize, 64 * 1024);
    ASSERT_EQ(configsMap.at(DESCRIPTORS[1]).limits->MaxSize, 32 * 1024 * 1024);

    // test for shared
    ASSERT_EQ(configsMap.at(DESCRIPTORS[2]).MaxPoolableSize, 0);
    ASSERT_EQ(configsMap.at(DESCRIPTORS[2]).Capacity, 0);
    ASSERT_EQ(configsMap.at(DESCRIPTORS[2]).SlabMinSize, 2 * 1024 * 1024);
    ASSERT_EQ(configsMap.at(DESCRIPTORS[2]).limits->MaxSize, 32 * 1024 * 1024);

    // test for read-only shared
    ASSERT_EQ(configsMap.at(DESCRIPTORS[3]).MaxPoolableSize, 4 * 1024 * 1024);
    ASSERT_EQ(configsMap.at(DESCRIPTORS[3]).Capacity, 4);
    ASSERT_EQ(configsMap.at(DESCRIPTORS[3]).SlabMinSize, 2 * 1024 * 1024);
    ASSERT_EQ(configsMap.at(DESCRIPTORS[3]).limits->MaxSize, 32 * 1024 * 1024);
}
