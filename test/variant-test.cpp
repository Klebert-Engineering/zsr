#include "gtest/gtest.h"

#include "zsr/variant.hpp"

using zsr::Variant;

namespace {

TEST(VariantTest, default_ctor) {
    Variant v;
}

TEST(VariantTest, bool_set_get) {
    Variant v;

    /* Supported conversions */

    v.set<bool>(false);
    ASSERT_EQ(false, *v.get<bool>());

    v.set<bool>(true);
    ASSERT_EQ(1u, *v.get<uint64_t>());

    /* Unsupported conversions */

    v.set<bool>(true);
    ASSERT_FALSE(v.get<int64_t>());
    ASSERT_FALSE(v.get<double>());
    ASSERT_FALSE(v.get<std::string>());
}

TEST(VariantTest, signed_set_get) {
    Variant v;

    /* Supported conversions */

    v.set<int8_t>(1);
    ASSERT_EQ(1, *v.get<int64_t>());

    v.set<int16_t>(2);
    ASSERT_EQ(2, *v.get<int64_t>());

    v.set<int32_t>(3);
    ASSERT_EQ(3, *v.get<int64_t>());

    v.set<int64_t>(4);
    ASSERT_EQ(4, *v.get<int64_t>());

    /* Unsupported conversions */

    v.set<int64_t>(5);
    ASSERT_FALSE(v.get<bool>());
    ASSERT_FALSE(v.get<uint64_t>());
    ASSERT_FALSE(v.get<double>());
    ASSERT_FALSE(v.get<std::string>());
}

TEST(VariantTest, unsigned_set_get) {
    Variant v;

    /* Supported conversions */

    v.set<uint8_t>(1u);
    ASSERT_EQ(1u, *v.get<uint64_t>());

    v.set<uint16_t>(2u);
    ASSERT_EQ(2u, *v.get<uint64_t>());

    v.set<uint32_t>(3u);
    ASSERT_EQ(3u, *v.get<uint64_t>());

    v.set<uint64_t>(4u);
    ASSERT_EQ(4u, *v.get<uint64_t>());

    v.set<uint64_t>(5u);
    ASSERT_EQ(true, *v.get<bool>());

    /* Unsupported conversions */

    v.set<uint64_t>(6u);
    ASSERT_FALSE(v.get<int64_t>());
    ASSERT_FALSE(v.get<double>());
    ASSERT_FALSE(v.get<std::string>());
}

TEST(VariantTest, float_set_get) {
    Variant v;

    /* Supported conversions */

    v.set<float>(0.5f);
    ASSERT_EQ(0.5f, *v.get<double>());

    v.set<double>(1.5);
    ASSERT_EQ(1.5, *v.get<double>());

    /* Unsupported conversions */

    v.set<double>(2.5);
    ASSERT_FALSE(v.get<bool>());
    ASSERT_FALSE(v.get<int64_t>());
    ASSERT_FALSE(v.get<uint64_t>());
    ASSERT_FALSE(v.get<std::string>());
}

TEST(VariantTest, string_set_get) {
    Variant v;

    /* Supported conversions */

    v.set<const char*>("const char");
    ASSERT_EQ("const char", *v.get<std::string>());

    v.set<std::string>("std string");
    ASSERT_EQ("std string", *v.get<std::string>());

    /* Unsupported conversions */

    v.set<std::string>("unsupported");
    ASSERT_FALSE(v.get<bool>());
    ASSERT_FALSE(v.get<int64_t>());
    ASSERT_FALSE(v.get<uint64_t>());
    ASSERT_FALSE(v.get<double>());
}

}
