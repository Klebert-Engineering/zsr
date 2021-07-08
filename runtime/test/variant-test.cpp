#include <catch2/catch.hpp>

#include "zsr/variant.hpp"

using zsr::Variant;

namespace {

TEST_CASE("default_ctor", "[VariantTest::default_ctor]") {
    Variant v;
}

TEST_CASE("bool_set_get", "[VariantTest::bool_set_get]") {
    Variant v;

    /* Supported conversions */

    v.set<bool>(false);
    REQUIRE(false == *v.get<bool>());

    v.set<bool>(true);
    REQUIRE(1u == *v.get<uint64_t>());

    /* Unsupported conversions */

    v.set<bool>(true);
    REQUIRE(!v.get<int64_t>());
    REQUIRE(!v.get<double>());
    REQUIRE(!v.get<std::string>());
}

TEST_CASE("signed_set_get", "[VariantTest::signed_set_get]") {
    Variant v;

    /* Supported conversions */

    v.set<int8_t>(1);
    REQUIRE(1 == *v.get<int64_t>());

    v.set<int16_t>(2);
    REQUIRE(2 == *v.get<int64_t>());

    v.set<int32_t>(3);
    REQUIRE(3 == *v.get<int64_t>());

    v.set<int64_t>(4);
    REQUIRE(4 == *v.get<int64_t>());

    /* Unsupported conversions */

    v.set<int64_t>(5);
    REQUIRE(!v.get<bool>());
    REQUIRE(!v.get<uint64_t>());
    REQUIRE(!v.get<double>());
    REQUIRE(!v.get<std::string>());
}

TEST_CASE("unsigned_set_get", "[VariantTest::unsigned_set_get]") {
    Variant v;

    /* Supported conversions */

    v.set<uint8_t>(1u);
    REQUIRE(1u == *v.get<uint64_t>());

    v.set<uint16_t>(2u);
    REQUIRE(2u == *v.get<uint64_t>());

    v.set<uint32_t>(3u);
    REQUIRE(3u == *v.get<uint64_t>());

    v.set<uint64_t>(4u);
    REQUIRE(4u == *v.get<uint64_t>());

    v.set<uint64_t>(5u);
    REQUIRE(true == *v.get<bool>());

    /* Unsupported conversions */

    v.set<uint64_t>(6u);
    REQUIRE(!v.get<int64_t>());
    REQUIRE(!v.get<double>());
    REQUIRE(!v.get<std::string>());
}

TEST_CASE("float_set_get", "[VariantTest::float_set_get]") {
    Variant v;

    /* Supported conversions */

    v.set<float>(0.5f);
    REQUIRE(0.5f == *v.get<double>());

    v.set<double>(1.5);
    REQUIRE(1.5 == *v.get<double>());

    /* Unsupported conversions */

    v.set<double>(2.5);
    REQUIRE(!v.get<bool>());
    REQUIRE(!v.get<int64_t>());
    REQUIRE(!v.get<uint64_t>());
    REQUIRE(!v.get<std::string>());
}

TEST_CASE("string_set_get", "[VariantTest::string_set_get]") {
    Variant v;

    /* Supported conversions */

    v.set("const char");
    REQUIRE("const char" == *v.get<std::string>());

    v.set<std::string>("std string");
    REQUIRE("std string" == *v.get<std::string>());

    /* Unsupported conversions */

    v.set<std::string>("unsupported");
    REQUIRE(!v.get<bool>());
    REQUIRE(!v.get<int64_t>());
    REQUIRE(!v.get<uint64_t>());
    REQUIRE(!v.get<double>());
}

}
