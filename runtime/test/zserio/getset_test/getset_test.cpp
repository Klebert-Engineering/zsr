#include "base.hpp"

#include <zsr/getset.hpp>

namespace {

    PKG;

    TEST(GetSetTest, make_and_set_and_get)
    {
        auto s = zsr::make(zsr::packages(), "getset_test.Compound", {
            {"x", 1},
            {"y", zsr::VariantMap{{"a", 2}}},
            {"y.b", 3}
        });

        ASSERT_EQ(zsr::get(s, "x"), 1);
        ASSERT_EQ(zsr::get(s, "y.a"), 2);
        ASSERT_EQ(zsr::get(s, "y.b"), 3);

        zsr::set(s, "x", 4);
        zsr::set(s, {{
            "y", zsr::VariantMap{
                {"a", 5},
                {"b", 6}
            }
        }});

        ASSERT_EQ(zsr::get(s, "x"), 4);
        ASSERT_EQ(zsr::get(s, "y.a"), 5);
        ASSERT_EQ(zsr::get(s, "y.b"), 6);

        ASSERT_THROW(zsr::make(zsr::packages(), "unknown"), zsr::UnknownIdentifierError);
        ASSERT_THROW(zsr::set(s, "y.c", 66), zsr::UnknownIdentifierError);
        ASSERT_THROW(zsr::set(s, "y.b.c", 66), zsr::VariantCastError);
        ASSERT_THROW(zsr::get(s, "y.c"), zsr::UnknownIdentifierError);
        ASSERT_THROW(zsr::get(s, "y.b.c"), zsr::VariantCastError);
    }
}
