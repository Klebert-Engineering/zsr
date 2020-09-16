#include "base.hpp"

namespace {

PKG;

TEST(ServiceTest, check_service_meta) {
    auto* a_service = zsr::find<zsr::Service>(pkg, "A_service");
    ASSERT_TRUE(a_service);
    ASSERT_EQ(&a_service->parent, &pkg);
   
    auto* a_service_method_a = zsr::find<zsr::ServiceMethod>(*a_service, "method_a");
    ASSERT_TRUE(a_service_method_a);
    ASSERT_EQ(&a_service_method_a->parent, a_service);
}

}
