#include "base.hpp"

#include <zsr/find.hpp>
#include <zsr/types-json.hpp>
#include <zsr/introspectable-json.hpp>

#include "speedyj/speedyj.hpp"

namespace {

PKG;

TEST_CASE("serialize_meta_subtype", "[JsonTest::serialize_meta_subtype]")
{
    speedyj::Stream ss;

    auto m = zsr::find<zsr::SubType>(pkg, "S1");
    auto s = (ss << *m).str();

    REQUIRE(s == R"({"ident":"S1","type":{"ident":"","package":"","ctype":{"type":"int","size":8,"array":false},"ztype":{"type":"int","size":64,"array":false}}})");
}

}
