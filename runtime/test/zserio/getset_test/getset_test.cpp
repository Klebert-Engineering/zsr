#include "base.hpp"

#include <zsr/getset.hpp>

namespace {

PKG;

TEST_CASE("make_and_set_and_get", "[GetSetTest::make_and_set_and_get]")
{
    auto s = zsr::make(zsr::packages(), "getset_test.Compound", {
        {"x", 1},
        {"y", zsr::VariantMap{{"a", 2}}},
        {"y.b", 3}
    });

    REQUIRE(zsr::get(s, "x") == 1);
    REQUIRE(zsr::get(s, "y.a") == 2);
    REQUIRE(zsr::get(s, "y.b") == 3);

    zsr::set(s, "x", 4);
    zsr::set(s, {{
        "y", zsr::VariantMap{
            {"a", 5},
            {"b", 6}
        }
    }});

    REQUIRE(zsr::get(s, "x") == 4);
    REQUIRE(zsr::get(s, "y.a") == 5);
    REQUIRE(zsr::get(s, "y.b") == 6);

    REQUIRE_THROWS_AS(zsr::make(zsr::packages(), "unknown"), zsr::UnknownIdentifierError);
    REQUIRE_THROWS_AS(zsr::set(s, "y.c", 66), zsr::UnknownIdentifierError);
    REQUIRE_THROWS_AS(zsr::set(s, "y.b.c", 66), zsr::VariantCastError);
    REQUIRE_THROWS_AS(zsr::get(s, "y.c"), zsr::UnknownIdentifierError);
    REQUIRE_THROWS_AS(zsr::get(s, "y.b.c"), zsr::VariantCastError);
}

}
