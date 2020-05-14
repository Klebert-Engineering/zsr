#pragma once

#include <zserio/BitStreamReader.h>
#include <zserio/BitStreamWriter.h>
#include <zserio/PreWriteAction.h>

#include "zsr/variant.hpp"

#include <any>
#include <functional>
#include <memory>
#include <string>
#include <vector>

#if _MSC_VER
/* Disable warnings about unexported template instantiations */
#pragma warning(push)
#pragma warning(disable: 4251)
#endif

namespace zsr {

/**
 * Generic parameter list.
 * See `Parameter::set`.
 */
struct ZSR_EXPORT ParameterList
{
    std::any list;
};

/**
 * ZSerio type information.
 *
 * TODO: Take ZType data with grain of salt. Especially array types
 *       might be incomplete.
 *
 * Use information from CType for type detection/conversion.
 * The ZType does not have to match the CType.
 */
struct ZSR_EXPORT ZType
{
    enum
    {
        UInt,
        Int,
        VarUInt,
        VarInt,
        Float,
        Bitmask,
        Bitfield,
        UBitfield,
        VarBitfield,
        VarUBitfield,
        Bool,
        Enum,
        String,
        Structure,
    } type;

    size_t size = 0;    /* Size in bits */
    bool array = false; /* Array of type */
};

/**
 * Generated C++ type information.
 */
struct ZSR_EXPORT CType
{
    enum
    {
        Bool,
        UInt,
        Int,
        Float,
        String,
        Structure,
        BitBuffer,
    } type;

    size_t size = 0;    /* Size in bytes */
    bool array = false; /* Array (vector) of type */
};

struct ZSR_EXPORT TypeRef
{
    std::string package;
    std::string ident;

    ZType ztype;
    CType ctype;
};

/**
 * Subtype
 */
struct ZSR_EXPORT SubType
{
    std::string ident;
    const TypeRef* type = nullptr;
};

/**
 * Package constant
 */
struct ZSR_EXPORT Constant
{
    std::string ident;
    Variant value;
    const TypeRef* type = nullptr;
};

/**
 * Bitmask value
 */
struct ZSR_EXPORT BitmaskValue
{
    std::string ident;
    Variant value;
};

/**
 * Bitmask
 */
struct ZSR_EXPORT Bitmask
{
    std::string ident;
    std::vector<const BitmaskValue*> values;
};

/**
 * Enumeration item
 */
struct ZSR_EXPORT EnumerationItem
{
    std::string ident;
    Variant value;
};

/**
 * Enumeration
 */
struct ZSR_EXPORT Enumeration
{
    std::string ident;
    std::vector<const EnumerationItem*> items;
};

/**
 * Compound field/member
 */
struct ZSR_EXPORT Field
{
    std::string ident;
    const TypeRef* type = nullptr;

    /**
     * Function that returns the fields value.
     */
    std::function<Variant(const Introspectable&)> get = nullptr;

    /**
     * Optional function to set the fields value.
     */
    std::function<void(Introspectable&, Variant)> set = nullptr;

    /**
     * Optional function to check if the field is set.
     */
    std::function<bool(const Introspectable&)> has = nullptr;

    /**
     * Optional function to reset the optional field.
     */
    std::function<void(Introspectable&)> reset = nullptr;
};

/**
 * Compound parameter
 */
struct ZSR_EXPORT Parameter
{
    std::string ident;
    const TypeRef* type = nullptr;
    const Field* field = nullptr; /* Matching field (read-only) */

    std::function<void(ParameterList&, Variant)> set = nullptr;
};

/**
 * Function
 */
struct ZSR_EXPORT Function
{
    std::string ident;
    const TypeRef* type = nullptr;

    std::function<Variant(const Introspectable&)> call = nullptr;
};

/**
 * Compound (Structure, Choice or Union)
 */
struct ZSR_EXPORT Compound
{
    std::string ident;
    enum class Type {
        Structure,
        Choice,
        Union,
    } type;

    std::vector<const Parameter*> parameters;
    std::vector<const Field*> fields;
    std::vector<const Function*> functions;

    /**
     * Returns a new instance of the compound.
     *
     * For compounds that have parameters, `initialize`
     * must be called.
     */
    std::function<Introspectable()> alloc = nullptr;

    /**
     * Optional initialization function.
     *
     * Valid for compounds that have parameters.
     *
     * @param Instance of the introspectable to initialize.
     * @param Parameter list, see `Parameter::set`.
     */
    std::function<void(Introspectable&, ParameterList)> initialize = nullptr;

    /**
     * Optional child initializator.
     */
    std::function<void(Introspectable&)> initializeChildren = nullptr;

    /**
     * Comparison function.
     */
    std::function<bool(const Introspectable&, const Introspectable&)> compare =
        nullptr;

    /**
     * Hash function.
     */
    std::function<int(const Introspectable&)> hash =
        nullptr;

    /**
     * Bitsize function.
     */
    std::function<size_t(const Introspectable&)> bitSize =
        nullptr;

    /**
     * Deserialization function.
     */
    std::function<void(Introspectable&, ::zserio::BitStreamReader&)> read =
        nullptr;

    /**
     * Serialization function.
     */
    std::function<void(Introspectable&, ::zserio::BitStreamWriter&)> write =
        nullptr;
};

/**
 * Package
 */
struct ZSR_EXPORT Package
{
    std::string ident;

    std::vector<const SubType*> subTypes;
    std::vector<const Constant*> constants;
    std::vector<const Enumeration*> enumerations;
    std::vector<const Bitmask*> bitmasks;
    std::vector<const Compound*> compounds;
};

} // namespace zsr

#if _MSC_VER
#pragma warning(pop)
#endif
