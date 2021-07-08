#include "base.hpp"

#include <zsr/find.hpp>
#include <zsr/types-json.hpp>
#include <zsr/introspectable-json.hpp>

#include "speedyj/speedyj.hpp"

namespace {

PKG;

TEST_CASE("empty_object", "[JsonTest::empty_object]")
{
    speedyj::Stream ss;
    ss << speedyj::Object << speedyj::End;

    REQUIRE(ss.str() == R"({})");
}

TEST_CASE("empty_array", "[JsonTest::empty_array]")
{
    speedyj::Stream ss;
    ss << speedyj::Array << speedyj::End;

    REQUIRE(ss.str() == R"([])");
}

TEST_CASE("key_value_pair", "[JsonTest::key_value_pair]")
{
    speedyj::Stream ss;
    ss << speedyj::Object
       << "key" << "value"
       << speedyj::End;

    REQUIRE(ss.str() == R"({"key":"value"})");
}

TEST_CASE("json_string_escaping", "[JsonTest::json_string_escaping]")
{
    speedyj::Stream ss;
    ss << speedyj::Object
       << "\"hello\"" << "\\world\\"
       << speedyj::End;

    REQUIRE(ss.str() == "{\"\\\"hello\\\"\":\"\\\\world\\\\\"}");
}

TEST_CASE("json_uchar_string_escaping", "[JsonTest::json_uchar_string_escaping]")
{
    speedyj::Stream ss;
    ss << speedyj::Object
       << "\"hello\x02\"" << "\\\xC2\xB9\\"
       << speedyj::End;

    REQUIRE(ss.str() == "{\"\\\"hello\\u0002\\\"\":\"\\\\\\u00C2\\u00B9\\\\\"}");
}

TEST_CASE("serialize_meta_subtype", "[JsonTest::serialize_meta_subtype]")
{
    speedyj::Stream ss;

    auto m = zsr::find<zsr::SubType>(pkg, "S1");
    auto s = (ss << *m).str();

    REQUIRE(s == R"({"ident":"S1","type":{"ident":"","package":"","ctype":{"type":"int","size":8,"array":false},"ztype":{"type":"int","size":64,"array":false}}})");
}

}
