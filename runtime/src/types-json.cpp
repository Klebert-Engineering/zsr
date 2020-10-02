#include <zsr/types-json.hpp>
#include <zsr/introspectable-json.hpp>

#include <zserio/BitBuffer.h>

#include <vector>

namespace nlo = nlohmann;

namespace zsr
{

void to_json(nlo::json& j, const Introspectable& v)
{
    j = serialize(v);
}

void to_json(nlo::json& j, const Variant& v)
{
#define GEN(type)                               \
    if (auto vv = v.get< type >()) { j = *vv; return; }

    ZSR_VARIANT_TYPES(GEN);

#undef GEN
}

NLOHMANN_JSON_SERIALIZE_ENUM(ZType::Type, {
    {ZType::UInt, "uint"},
    {ZType::Int, "int"},
    {ZType::VarUInt, "varuint"},
    {ZType::VarInt, "varint"},
    {ZType::Float, "float"},
    {ZType::Bitmask, "bitmask"},
    {ZType::Bitfield, "bitfield"},
    {ZType::UBitfield, "ubitfield"},
    {ZType::VarBitfield, "varbitfield"},
    {ZType::VarUBitfield, "varubitfield"},
    {ZType::Bool, "bool"},
    {ZType::Enum, "enum"},
    {ZType::String, "string"},
    {ZType::Structure, "structure"},
})

void to_json(nlo::json& j, const ZType& m)
{
    j = {
        {"type", m.type},
        {"size", m.size},
        {"array", m.array}
    };
}

NLOHMANN_JSON_SERIALIZE_ENUM(CType::Type, {
    {CType::Bool, "bool"},
    {CType::UInt, "uint"},
    {CType::Int, "int"},
    {CType::Float, "float"},
    {CType::String, "string"},
    {CType::Structure, "structure"},
    {CType::BitBuffer, "bitbuffer"},
})

void to_json(nlo::json& j, const CType& m)
{
    j = {
        {"type", m.type},
        {"size", m.size},
        {"array", m.array}
    };
}

void to_json(nlo::json& j, const TypeRef& m)
{
    j = {
        {"ident", m.ident},
        {"package", m.package},
        {"ctype", m.ctype},
        {"ztype", m.ztype}
    };
}

void to_json(nlo::json& j, const SubType& m)
{
    j = {
        {"ident", m.ident},
    };
    if (m.type)
        j["type"] = *m.type;
}

void to_json(nlo::json& j, const Constant& m)
{
    j = {
        {"ident", m.ident},
        {"value", m.value}
    };
    if (m.type)
        j["type"] = *m.type;
}

void to_json(nlo::json& j, const BitmaskValue& m)
{
    j = {
        {"ident", m.ident},
        {"value", m.value}
    };
}

void to_json(nlo::json& j, const Bitmask& m)
{
    j = {
        {"ident", m.ident},
        {"values", m.values}
    };
}

void to_json(nlo::json& j, const EnumerationItem& m)
{
    j = {
        {"ident", m.ident},
        {"value", m.value}
    };
}

void to_json(nlo::json& j, const Enumeration& m)
{
    j = {
        {"ident", m.ident},
        {"items", m.items}
    };
}

void to_json(nlo::json& j, const Parameter& m)
{
    j = {
        {"ident", m.ident},
    };
    if (m.type)
        j["type"] = *m.type;
}

void to_json(nlo::json& j, const Field& m)
{
    j = {
        {"ident", m.ident},
    };
    if (m.type)
        j["type"] = *m.type;
}

void to_json(nlo::json& j, const Function& m)
{
    j = {
        {"ident", m.ident},
    };
    if (m.type)
        j["type"] = *m.type;
}

NLOHMANN_JSON_SERIALIZE_ENUM(Compound::Type, {
    {Compound::Type::Structure, "structure"},
    {Compound::Type::Union, "union"},
    {Compound::Type::Choice, "choice"},
})

void to_json(nlo::json& j, const Compound& m)
{
    j = {
        {"ident", m.ident},
        {"type", m.type},
        {"parameters", m.parameters},
        {"fields", m.fields},
        {"functions", m.functions}
    };
}

void to_json(nlo::json& j, const ServiceMethod& m)
{
    j = {
        {"ident", m.ident},
        {"request_type", m.requestType},
        {"response_type", m.responseType}
    };
}

void to_json(nlo::json& j, const Service& m)
{
    j = {
        {"ident", m.ident},
        {"methods", m.methods}
    };
}

void to_json(nlo::json& j, const Package& m)
{
    j = {
        {"ident", m.ident},
        {"sub_types", m.subTypes},
        {"constants", m.constants},
        {"enumerations", m.enumerations},
        {"bitmasks", m.bitmasks},
        {"compounds", m.compounds},
        {"services", m.services}
    };
}

}
