#include "base.hpp"

namespace {

PKG;

TEST_CASE("check_service_meta", "[ServiceTest::check_service_meta]") {
    auto* a_service = zsr::find<zsr::Service>(pkg, "A_service");
    REQUIRE(a_service);
    REQUIRE(&a_service->parent == &pkg);
   
    auto* a_service_method_a = zsr::find<zsr::ServiceMethod>(*a_service, "method_a");
    REQUIRE(a_service_method_a);
    REQUIRE(&a_service_method_a->parent == a_service);
}

}
