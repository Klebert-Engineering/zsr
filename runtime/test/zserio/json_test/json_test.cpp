#include "base.hpp"

#include <zsr/find.hpp>
#include <zsr/types-json.hpp>
#include <zsr/introspectable-json.hpp>


#include <zsr/speedy-j.hpp>

namespace {

PKG;

TEST(JsonTest, empty_object)
{
    speedyj::Stream ss;
    ss << speedyj::Object << speedyj::End;

    ASSERT_EQ(ss.str(), R"({})");
}

TEST(JsonTest, empty_array)
{
    speedyj::Stream ss;
    ss << speedyj::Array << speedyj::End;

    ASSERT_EQ(ss.str(), R"([])");
}

TEST(JsonTest, key_value_pair)
{
    speedyj::Stream ss;
    ss << speedyj::Object
       << "key" << "value"
       << speedyj::End;

    ASSERT_EQ(ss.str(), R"({"key":"value"})");
}

TEST(JsonTest, json_string_escaping)
{
    speedyj::Stream ss;
    ss << speedyj::Object
       << "\"hello\"" << "\\world\\"
       << speedyj::End;

    ASSERT_EQ(ss.str(), "{\"\\\"hello\\\"\":\"\\\\world\\\\\"}");
}

TEST(JsonTest, serialize_meta_subtype)
{
    speedyj::Stream ss;

    auto m = zsr::find<zsr::SubType>(pkg, "S1");
    auto s = (ss << *m).str();

    ASSERT_EQ(s, R"({"ident":"S1","type":{"ident":"","package":"","ctype":{"type":"int","size":8,"array":false},"ztype":{"type":"int","size":64,"array":false}}})");
}

}
