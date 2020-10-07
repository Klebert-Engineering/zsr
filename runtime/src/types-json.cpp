#include <zsr/types-json.hpp>
#include <zsr/introspectable-json.hpp>

#include <zserio/BitBuffer.h>

#include <vector>

namespace zsr
{

speedyj::Stream& operator<<(speedyj::Stream& s, const Introspectable& i)
{
    return s << serialize(i).str();
}

speedyj::Stream& operator<<(speedyj::Stream& s, const Variant& v)
{
#define GEN(TYPE)                                                       \
    if (auto&& vv = v.get< TYPE >()) { return s << (TYPE)*vv; }         \
    if (auto&& vv = v.get<std::vector< TYPE >>()) { return s << *vv; }  \

    ZSR_VARIANT_TYPES(GEN)

#undef GEN

    return s << speedyj::Null;
}

speedyj::Stream& operator<<(speedyj::Stream& s, const ZType::Type& m)
{
    switch (m) {
    case ZType::UInt:
        return s << "uint";
    case ZType::Int:
        return s << "int";
    case ZType::VarUInt:
        return s << "varuint";
    case ZType::VarInt:
        return s << "varint";
    case ZType::Float:
        return s << "float";
    case ZType::Bitmask:
        return s << "bitmask";
    case ZType::Bitfield:
        return s << "bitfield";
    case ZType::UBitfield:
        return s << "ubitfield";
    case ZType::VarBitfield:
        return s << "varbitfield";
    case ZType::VarUBitfield:
        return s << "varubitfield";
    case ZType::Bool:
        return s << "bool";
    case ZType::Enum:
        return s << "enum";
    case ZType::String:
        return s << "string";
    case ZType::Structure:
        return s << "structure";
    }

    return s << speedyj::Null;
}

speedyj::Stream& operator<<(speedyj::Stream& s, const ZType& m)
{
    return s << speedyj::Object
             << "type" << m.type
             << "size" << m.size
             << "array" << m.array
             << speedyj::End;
}

speedyj::Stream& operator<<(speedyj::Stream& s, const CType::Type& m)
{
    switch (m) {
    case CType::Bool:
        return s << "bool";
    case CType::UInt:
        return s << "uint";
    case CType::Int:
        return s << "int";
    case CType::Float:
        return s << "float";
    case CType::String:
        return s << "string";
    case CType::Structure:
        return s << "structure";
    case CType::BitBuffer:
        return s << "bitbuffer";
    }

    return s << speedyj::Null;
}

speedyj::Stream& operator<<(speedyj::Stream& s, const CType& m)
{
    return s << speedyj::Object
             << "type" << m.type
             << "size" << m.size
             << "array" << m.array
             << speedyj::End;
}

speedyj::Stream& operator<<(speedyj::Stream& s, const TypeRef& m)
{
    return s << speedyj::Object
             << "ident" << m.ident
             << "package" << m.package
             << "ctype" << m.ctype
             << "ztype" << m.ztype
             << speedyj::End;
}

speedyj::Stream& operator<<(speedyj::Stream& s, const SubType& m)
{
    s << speedyj::Object
      << "ident" << m.ident;

    if (m.type)
        s << "type" << *m.type;

    return s << speedyj::End;
}

speedyj::Stream& operator<<(speedyj::Stream& s, const Constant& m)
{
    s << speedyj::Object
      << "ident" << m.ident;

    if (m.type)
        s << "type" << *m.type;

    return s << speedyj::End;
}

speedyj::Stream& operator<<(speedyj::Stream& s, const BitmaskValue& m)
{
    return s << speedyj::Object
             << "ident" << m.ident
             << "value" << m.value
             << speedyj::End;
}

speedyj::Stream& operator<<(speedyj::Stream& s, const Bitmask& m)
{
    return s << speedyj::Object
             << "ident" << m.ident
             << "values" << m.values
             << speedyj::End;
}

speedyj::Stream& operator<<(speedyj::Stream& s, const EnumerationItem& m)
{
    return s << speedyj::Object
             << "ident" << m.ident
             << "value" << m.value
             << speedyj::End;
}

speedyj::Stream& operator<<(speedyj::Stream& s, const Enumeration& m)
{
    return s << speedyj::Object
             << "ident" << m.ident
             << "items" << m.items
             << speedyj::End;
}

speedyj::Stream& operator<<(speedyj::Stream& s, const Parameter& m)
{
    s << speedyj::Object
      << "ident" << m.ident;

    if (m.type)
        s << "type" << *m.type;

    return s << speedyj::End;
}

speedyj::Stream& operator<<(speedyj::Stream& s, const Field& m)
{
    s << speedyj::Object
      << "ident" << m.ident;

    if (m.type)
        s << "type" << *m.type;

    return s << speedyj::End;
}

speedyj::Stream& operator<<(speedyj::Stream& s, const Function& m)
{
    s << speedyj::Object
      << "ident" << m.ident;

    if (m.type)
        s << "type" << *m.type;

    return s << speedyj::End;
}

speedyj::Stream& operator<<(speedyj::Stream& s, const Compound::Type& m)
{
    switch (m) {
    case Compound::Type::Structure:
        return s << "structure";
    case Compound::Type::Union:
        return s << "union";
    case Compound::Type::Choice:
        return s << "choice";
    }

    return s << speedyj::Null;
}

speedyj::Stream& operator<<(speedyj::Stream& s, const Compound& m)
{
    return s << speedyj::Object
             << "ident" << m.ident
             << "type" << m.type
             << "parameters" << m.parameters
             << "fields" << m.fields
             << "functions" << m.functions
             << speedyj::End;
}

speedyj::Stream& operator<<(speedyj::Stream& s, const ServiceMethod& m)
{
    return s << speedyj::Object
             << "ident" << m.ident
             << "request_type" << m.requestType
             << "response_type" << m.responseType
             << speedyj::End;
}

speedyj::Stream& operator<<(speedyj::Stream& s, const Service& m)
{
    return s << speedyj::Object
             << "ident" << m.ident
             << "methods" << m.methods
             << speedyj::End;
}

speedyj::Stream& operator<<(speedyj::Stream& s, const Package& m)
{
    return s << speedyj::Object
             << "ident" << m.ident
             << "sub_types" << m.subTypes
             << "constants" << m.constants
             << "enumerations" << m.enumerations
             << "bitmasks" << m.bitmasks
             << "compounds" << m.compounds
             << "services" << m.services
             << speedyj::End;
}

}

namespace zserio
{
speedyj::Stream& operator<<(speedyj::Stream& s, const zserio::BitBuffer& b)
{
    s << speedyj::Array;
    std::for_each(b.getBuffer(), b.getBuffer() + b.getByteSize(),
                  [&](auto v) {
                      s << (uint64_t)v;
                  });

    return s << speedyj::End;
}
}
