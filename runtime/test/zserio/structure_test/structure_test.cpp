#include "base.hpp"

namespace {

PKG;

TEST_CASE("a_alloc_empty", "[StructureTest::a_alloc_empty]")
{
    auto* meta_struct = zsr::find<zsr::Compound>(pkg, "A_struct");

    REQUIRE(meta_struct);
    REQUIRE(&meta_struct->parent == &pkg);

    REQUIRE(meta_struct->alloc);
    REQUIRE(!meta_struct->initialize);
    REQUIRE(meta_struct->type == zsr::Compound::Type::Structure);

    auto introspectable = meta_struct->alloc();
    REQUIRE(introspectable.meta() == meta_struct);
}

TEST_CASE("b_alloc_init_parameterized", "[StructureTest::b_alloc_init_parameterized]")
{
    auto* meta_struct = zsr::find<zsr::Compound>(pkg, "B_struct");

    REQUIRE(meta_struct);
    REQUIRE(meta_struct->alloc);
    REQUIRE(meta_struct->initialize);

    auto introspectable = meta_struct->alloc();

    /*  Initialize with parameter list */
    auto* meta_parameter_a = zsr::find<zsr::Parameter>(*meta_struct, "a");

    REQUIRE(meta_parameter_a);

    zsr::ParameterList l;
    meta_parameter_a->set(l, uint8_t{123});

    meta_struct->initialize(introspectable, l);

    /* Read out matching field */
    auto* meta_field =
        zsr::find<zsr::Field>(*meta_struct, meta_parameter_a->ident);

    REQUIRE(meta_field);
    REQUIRE(meta_field->get);

    REQUIRE(meta_field->get(introspectable) == uint8_t{123});
}

TEST_CASE("c_alloc_init_parameterized_instance", "[StructureTest::c_alloc_init_parameterized_instance]")
{
    auto* s_parent = zsr::find<zsr::Compound>(pkg, "C_struct");
    auto* s_parameter = zsr::find<zsr::Compound>(pkg, "C_parameter_struct");

    REQUIRE(s_parent);
    REQUIRE(s_parameter);

    auto* p_a = zsr::find<zsr::Parameter>(*s_parent, "a");

    REQUIRE(p_a);

    /* Set parameter instance */
    zsr::ParameterList l;
    auto i_parameter = s_parameter->alloc();

    auto* m_a = zsr::find<zsr::Field>(*s_parameter, "a");

    m_a->set(i_parameter, "Hola");
    p_a->set(l, i_parameter);

    auto i_parent = s_parent->alloc();
    s_parent->initialize(i_parent, std::move(l));

    /* Read out (read-only) field */

    auto* m_parameter = zsr::find<zsr::Parameter>(*s_parent, "a");

    REQUIRE(m_parameter);
    REQUIRE(m_parameter->field);

    auto value = m_parameter->field->get(i_parent);
    auto instance = value.get<zsr::Introspectable>();

    REQUIRE(instance);
    REQUIRE(instance->meta());
    REQUIRE("C_parameter_struct" == instance->meta()->ident);

    auto* m_ca_a = zsr::find<zsr::Field>(*s_parameter, "a");
    REQUIRE(m_ca_a->get(*instance) == std::string{"Hola"});
}

TEST_CASE("d_call_function", "[StructureTest::d_call_function]")
{
    auto* s_struct = zsr::find<zsr::Compound>(pkg, "D_struct");
    REQUIRE(s_struct);
   
    auto* f_struct_a = zsr::find<zsr::Field>(*s_struct, "a");
    auto* f_fun_1 = zsr::find<zsr::Function>(*s_struct, "fun");
    auto* f_fun_2 = zsr::find<zsr::Function>(*s_struct, "fun2");

    auto* s_res = zsr::find<zsr::Compound>(pkg, "D_res");
    REQUIRE(s_res);

    auto* f_res_a = zsr::find<zsr::Field>(*s_res, "a");

    /* Alloc instance */
    auto instance = s_struct->alloc();

    /* Call function 1 (Integer) */ {
        REQUIRE(f_fun_1);

        auto value = f_fun_1->call(instance);
        REQUIRE(value == uint8_t{10});
    }

    /* Call function 2 (Compound) */ {
        REQUIRE(f_fun_2);

        /* Set-up child struct */ {
            auto res_instance = s_res->alloc();
            f_res_a->set(res_instance, int32_t{123});
            f_struct_a->set(instance, std::move(res_instance));
        }

        auto value = f_fun_2->call(instance);
        auto introspectable = value.get<zsr::Introspectable>();

        REQUIRE(introspectable);
        REQUIRE(f_res_a->get(*introspectable) == int32_t(123));
    }
}

TEST_CASE("e_set_get_builtin_field", "[StructureTest::e_set_get_builtin_field]")
{
    auto* s_struct = zsr::find<zsr::Compound>(pkg, "E_struct");
    auto* m_value = zsr::find<zsr::Field>(*s_struct, "a");

    /* Alloc instance */
    auto instance = s_struct->alloc();

    /* Write & read parameter */
    m_value->set(instance, std::string{"Hello"});
    REQUIRE(m_value->get(instance) == std::string{"Hello"});

    /* Overwrite & read parameter */
    m_value->set(instance, std::string{"World"});
    REQUIRE(m_value->get(instance) == std::string{"World"});
}

TEST_CASE("f_set_get_compound_field", "[StructureTest::f_set_get_compound_field]")
{
    auto* s_parent = zsr::find<zsr::Compound>(pkg, "F_parent");
    auto* m_parent = zsr::find<zsr::Field>(*s_parent, "a");
    auto* f_parent = zsr::find<zsr::Function>(*s_parent, "child_value");

    auto* s_child = zsr::find<zsr::Compound>(pkg, "F_child");
    auto* m_child = zsr::find<zsr::Field>(*s_child, "a");

    /* Alloc parameter */
    auto child = s_child->alloc();
    m_child->set(child, int32_t{123});

    /* Alloc parent & set child */
    auto parent = s_parent->alloc();
    m_parent->set(parent, child);

    /* Access child */
    auto child_ref = m_parent->get(parent);
    REQUIRE(child_ref.get<zsr::Introspectable>());

    /* Call function */
    auto value = f_parent->call(parent);
    REQUIRE(value == int32_t{123});
}

TEST_CASE("g_set_get_builtin_array_field", "[StructureTest::g_set_get_builtin_array_field]")
{
    auto* s_struct = zsr::find<zsr::Compound>(pkg, "G_struct");
    auto* m_value = zsr::find<zsr::Field>(*s_struct, "a");

    /* Alloc instance */
    auto instance = s_struct->alloc();

    /* Write & read parameter */
    m_value->set(instance, std::vector<std::string>{"Hello", "World", "!"});

    if (auto result = m_value->get(instance).get<std::vector<std::string>>()) {
        auto value = *result;
        REQUIRE(value[0] == "Hello");
        REQUIRE(value[1] == "World");
        REQUIRE(value[2] == "!");
    } else {
        FAIL("Not a vector of string!");
    }
}

TEST_CASE("h_invalid_field_access", "[StructureTest::h_invalid_field_access]")
{
    auto* meta_struct_a = zsr::find<zsr::Compound>(pkg, "H_struct_a");
    auto* meta_struct_b = zsr::find<zsr::Compound>(pkg, "H_struct_b");
    auto* meta_value = zsr::find<zsr::Field>(*meta_struct_a, "a");

    /* Alloc instance b */
    auto instance = meta_struct_b->alloc();

    /* Try write a::a with (wrong) instance of b */
    REQUIRE_THROWS_AS(meta_value->set(instance, std::string{"Hello"}),
                      zsr::IntrospectableCastError);
}

TEST_CASE("i_check_has_initialize_children", "[StructureTest::i_check_has_initialize_children]")
{
    auto* meta_struct_a = zsr::find<zsr::Compound>(pkg, "I_struct_a");
    REQUIRE(!meta_struct_a->initializeChildren);

    auto* meta_struct_b = zsr::find<zsr::Compound>(pkg, "I_struct_b");
    REQUIRE(meta_struct_b->initializeChildren);
}

TEST_CASE("j_conditional_field", "[StructureTest::j_conditional_field]")
{
    auto* meta_struct_a = zsr::find<zsr::Compound>(pkg, "J_struct");
    auto* meta_field_a = zsr::find<zsr::Field>(*meta_struct_a, "a");

    REQUIRE(!meta_field_a->has);
    REQUIRE(!meta_field_a->reset);

    auto* meta_field_b = zsr::find<zsr::Field>(*meta_struct_a, "b");

    REQUIRE(meta_field_b->has);
    REQUIRE(meta_field_b->reset);
}

TEST_CASE("k_nested_compounds", "[StructureTest::k_nested_compounds]")
{
    auto* meta_struct_a = zsr::find<zsr::Compound>(pkg, "K_struct_a");
    auto* meta_field_a_a = zsr::find<zsr::Field>(*meta_struct_a, "a");

    auto* meta_struct_b = zsr::find<zsr::Compound>(pkg, "K_struct_b");
    auto* meta_field_b_a = zsr::find<zsr::Field>(*meta_struct_b, "a");

    /* Alloc root object b */
    auto instance_b = meta_struct_b->alloc();

    /* Add children of type a */
    std::vector<zsr::Introspectable> children;
    for (auto i = 0; i < 10; ++i) {
        auto child = meta_struct_a->alloc();
        meta_field_a_a->set(child, int32_t{i});

        children.push_back(child);
    }

    meta_field_b_a->set(instance_b, children);

    /* Get children, expecting non-owning objects */
    auto opt_children_a =
        meta_field_b_a->get(instance_b).get<std::vector<zsr::Introspectable>>();

    REQUIRE(opt_children_a);
    REQUIRE(opt_children_a->size() == 10);

    {
        int32_t idx{0};
        for (const auto& child : *opt_children_a) {
            REQUIRE(!child.isOwning());
            REQUIRE(meta_field_a_a->get(child) == idx);

            ++idx;
        }
    }

    /* Overwrite/clear the vector of instance b */
    meta_field_b_a->set(instance_b, std::vector<zsr::Introspectable>{});

    /* Expect all (prev) children to own their referenced object */
    {
        int32_t idx{0};
        for (const auto& child : *opt_children_a) {
            REQUIRE(child.isOwning());
            REQUIRE(meta_field_a_a->get(child) == idx);

            ++idx;
        }
    }
}

TEST_CASE("l_later_registered_member_type", "[StructureTest::l_later_registered_member_type]")
{
    auto* meta_parent = zsr::find<zsr::Compound>(pkg, "L_parent");
    auto* meta_field = zsr::find<zsr::Field>(*meta_parent, "a");

    /* Alloc parent */
    auto instance = meta_parent->alloc();

    /* Access child instance */
    auto member = meta_field->get(instance).get<zsr::Introspectable>();

    REQUIRE(member);
    REQUIRE(member->obj);
    REQUIRE(member->meta());
}

TEST_CASE("m_field_type_info", "[StructureTest::m_field_type_info]")
{
    auto* meta_parent = zsr::find<zsr::Compound>(pkg, "M_parent");
    auto* meta_field_a = zsr::find<zsr::Field>(*meta_parent, "a");
    auto* meta_field_b = zsr::find<zsr::Field>(*meta_parent, "b");
    auto* meta_field_c = zsr::find<zsr::Field>(*meta_parent, "c");
    auto* meta_field_d = zsr::find<zsr::Field>(*meta_parent, "d");
    auto* meta_field_e = zsr::find<zsr::Field>(*meta_parent, "e");

    REQUIRE(meta_field_a);
    REQUIRE(zsr::ZType::Int == meta_field_a->type->ztype.type);
    REQUIRE(32 == meta_field_a->type->ztype.size);

    REQUIRE(meta_field_b);
    REQUIRE(zsr::ZType::UInt == meta_field_b->type->ztype.type);
    REQUIRE(16 == meta_field_b->type->ztype.size);

    REQUIRE(meta_field_c);
    REQUIRE(zsr::ZType::Float == meta_field_c->type->ztype.type);
    REQUIRE(32 == meta_field_c->type->ztype.size);

    REQUIRE(meta_field_d);
    REQUIRE(zsr::ZType::String == meta_field_d->type->ztype.type);
    REQUIRE(0 == meta_field_d->type->ztype.size);

    REQUIRE(meta_field_e);
    REQUIRE(zsr::ZType::Structure == meta_field_e->type->ztype.type);
    REQUIRE("M_child" == meta_field_e->type->ident);
}

TEST_CASE("n_set_incompatible_type", "[StructureTest::n_set_incompatible_type]")
{
    auto* meta_parent = zsr::find<zsr::Compound>(pkg, "N_parent");
    auto* meta_field = zsr::find<zsr::Field>(*meta_parent, "a");
    auto* meta_wrong = zsr::find<zsr::Compound>(pkg, "N_wrong");

    /* Alloc parent */
    auto instance = meta_parent->alloc();

    /* Alloc wrong member type */
    auto wrong_instance = meta_wrong->alloc();

    /* Set incompatible type. Expected N_expected, got N_wrong */
    REQUIRE_THROWS_AS(meta_field->set(instance, wrong_instance),
                      zsr::IntrospectableCastError);
}

TEST_CASE("o_array_field", "[StructureTest::o_array_field]")
{
    auto* meta_parent = zsr::find<zsr::Compound>(pkg, "O_parent");
    auto* meta_field = zsr::find<zsr::Field>(*meta_parent, "a");

    REQUIRE(meta_field);
    REQUIRE(meta_field->type);

    REQUIRE(meta_field->type->ztype.array);
    REQUIRE(meta_field->type->ztype.type == zsr::ZType::String);

    REQUIRE(meta_field->type->ctype.array);
    REQUIRE(meta_field->type->ctype.type == zsr::CType::String);
}

} // namespace
