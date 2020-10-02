#include "base.hpp"

#include <zsr/find.hpp>
#include <zsr/types-json.hpp>
#include <zsr/introspectable-json.hpp>

#include <nlohmann/json.hpp>

namespace nlo = nlohmann;

namespace {

PKG;

TEST(JsonTest, serialize_meta_subtype)
{
    auto s1 = zsr::find<zsr::SubType>(pkg, "S1");
    auto s1j = nlo::json(*s1);

    ASSERT_EQ(s1j["ident"], "S1");
    ASSERT_EQ(s1j["type"]["ztype"]["type"], "int");
}

/* TODO: Add more useful tests */

TEST(JsonTest, serialize_introspectable_default)
{
    auto meta_struct_a = zsr::find<zsr::Compound>(pkg, "A_struct");
    ASSERT_TRUE(meta_struct_a);

    auto meta_field_a = zsr::find<zsr::Field>(*meta_struct_a, "a");
    ASSERT_TRUE(meta_field_a);

    auto meta_field_b = zsr::find<zsr::Field>(*meta_struct_a, "b");
    ASSERT_TRUE(meta_field_b);

    auto obj = meta_struct_a->alloc();
    meta_field_a->set(obj, static_cast<int64_t>(123));
    meta_field_b->set(obj, static_cast<std::string>("Hello"));

    ASSERT_EQ(zsr::serialize(obj), R"({
        "a": 123,
        "b": "Hello"
    })"_json);
}

}
