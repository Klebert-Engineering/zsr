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

namespace zsr {

/**
 * Generic parameter list.
 * See `Parameter::set`.
 */
struct ParameterList
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
struct ZType
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
struct CType
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

struct TypeRef
{
    std::string package;
    std::string ident;

    ZType ztype;
    CType ctype;
};

/**
 * Subtype
 */
struct SubType
{
    std::string ident;
    const TypeRef* type = nullptr;
};

/**
 * Package constant
 */
struct Constant
{
    std::string ident;
    Variant value;
    const TypeRef* type = nullptr;
};

/**
 * Bitmask value
 */
struct BitmaskValue
{
    std::string ident;
    Variant value;
};

/**
 * Bitmask
 */
struct Bitmask
{
    std::string ident;
    std::vector<const BitmaskValue*> values;
};

/**
 * Enumeration item
 */
struct EnumerationItem
{
    std::string ident;
    Variant value;
};

/**
 * Enumeration
 */
struct Enumeration
{
    std::string ident;
    std::vector<const EnumerationItem*> items;
};

/**
 * Compound field/member
 */
struct Field
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
struct Parameter
{
    std::string ident;
    const TypeRef* type = nullptr;
    const Field* field = nullptr; /* Matching field (read-only) */

    std::function<void(ParameterList&, Variant)> set = nullptr;
};

/**
 * Function
 */
struct Function
{
    std::string ident;
    const TypeRef* type = nullptr;

    std::function<Variant(const Introspectable&)> call = nullptr;
};

/**
 * Choice case
 */
struct ChoiceCase
{
    /**
     * Pointer to field of parent compound `fields` or null.
     */
    const Field* field = nullptr;
};

/**
 * Compound (Structure, Choice or Union)
 */
struct Compound
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
    std::vector<const ChoiceCase*> cases;

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
struct Package
{
    std::string ident;

    std::vector<const SubType*> subTypes;
    std::vector<const Constant*> constants;
    std::vector<const Enumeration*> enumerations;
    std::vector<const Bitmask*> bitmasks;
    std::vector<const Compound*> compounds;
};

} // namespace zsr
