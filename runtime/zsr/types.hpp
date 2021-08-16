#pragma once

#include <zserio/BitStreamReader.h>
#include <zserio/BitStreamWriter.h>
#include <zserio/PreWriteAction.h>
#include <zserio/IService.h>

#include "zsr/variant.hpp"

#include <any>
#include <functional>
#include <memory>
#include <string>
#include <deque>
#include <optional>

#if _MSC_VER
/* Disable warnings about unexported template instantiations */
#pragma warning(push)
#pragma warning(disable: 4251)
#endif

/* Delete copy & move operators */
#define META_TYPE_ROOT(TYPE)                    \
    TYPE() {};                                  \
    TYPE(const TYPE &) = delete;                \
    TYPE(TYPE &&) = delete;                     \
    TYPE& operator=(const TYPE &) = delete;     \
    TYPE& operator=(TYPE &&) = delete;

#define META_TYPE(TYPE, PARENT)                 \
    PARENT& parent;                             \
    TYPE(PARENT* p) : parent(*p) {};            \
    TYPE(const TYPE &) = delete;                \
    TYPE(TYPE &&) = delete;                     \
    TYPE& operator=(const TYPE &) = delete;     \
    TYPE& operator=(TYPE &&) = delete;

namespace zsr {

struct Package;
struct Bitmask;
struct Enumeration;
struct Compound;
struct Service;

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
    enum Type
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
    enum Type
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
    META_TYPE(SubType, Package)

    std::string ident;
    std::optional<TypeRef> type;
};

/**
 * Package constant
 */
struct ZSR_EXPORT Constant
{
    META_TYPE(Constant, Package)

    std::string ident;
    Variant value;
    std::optional<TypeRef> type;
};

/**
 * Bitmask value
 */
struct ZSR_EXPORT BitmaskValue
{
    META_TYPE(BitmaskValue, Bitmask)

    std::string ident;
    Variant value;
};

/**
 * Bitmask
 */
struct ZSR_EXPORT Bitmask
{
    META_TYPE(Bitmask, Package)

    std::string ident;
    std::deque<BitmaskValue> values;
};

/**
 * Enumeration item
 */
struct ZSR_EXPORT EnumerationItem
{
    META_TYPE(EnumerationItem, Enumeration)

    std::string ident;
    Variant value;
};

/**
 * Enumeration
 */
struct ZSR_EXPORT Enumeration
{
    META_TYPE(Enumeration, Package)

    std::string ident;
    std::deque<EnumerationItem> items;
};

/**
 * Compound field/member
 */
struct ZSR_EXPORT Field
{
    META_TYPE(Field, Compound)

    std::string ident;
    std::optional<TypeRef> type;

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
    META_TYPE(Parameter, Compound)

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
    META_TYPE(Function, Compound)

    std::string ident;
    std::optional<TypeRef> type;

    std::function<Variant(const Introspectable&)> call = nullptr;
};

/**
 * Compound (Structure, Choice or Union)
 */
struct ZSR_EXPORT Compound
{
    META_TYPE(Compound, Package)

    std::string ident;
    enum class Type {
        Structure,
        Choice,
        Union,
    } type;

    std::deque<Parameter> parameters;
    std::deque<Field> fields;
    std::deque<Function> functions;

    /**
     * Returns a new instance of the compound.
     *
     * For compounds that have parameters, `initialize`
     * must be called.
     */
    std::function<Introspectable()> alloc = nullptr;

    /**
     * Returns a new instance of the compound read from the given
     * bitstream reader.
     *
     * @param BitStreamReader  Zserio bit stream reader.
     * @param Parameter list, see `Parameter::set`.
     *
     * Maps to zserio generated ctor with `BitStreamReader`.
     */
    std::function<Introspectable(::zserio::BitStreamReader&,
                                 ParameterList)> read = nullptr;

    /**
     * Optional initialization function.
     * Set for compounds that have parameters.
     *
     * @param Instance of the introspectable to initialize.
     * @param Parameter list, see `Parameter::set`.
     */
    std::function<void(Introspectable&, ParameterList)> initialize = nullptr;

    /**
     * Optional child initializator.
     *
     * Maps to zserio generated `initializeChildren` function.
     */
    std::function<void(Introspectable&)> initializeChildren = nullptr;

    /**
     * Comparison function.
     *
     * Maps to zserio generated `operator==` function.
     */
    std::function<bool(const Introspectable&, const Introspectable&)> compare =
        nullptr;

    /**
     * Hash function.
     *
     * Maps to zserio generated `hashCode` function.
     */
    std::function<int(const Introspectable&)> hash =
        nullptr;

    /**
     * Bitsize function.
     *
     * Maps to zserio generated `bitSizeOf` function.
     */
    std::function<size_t(const Introspectable&)> bitSize =
        nullptr;

    /**
     * Serialization function.
     *
     * Maps to zserio generated `write` function.
     */
    std::function<void(Introspectable&, ::zserio::BitStreamWriter&)> write =
        nullptr;
};

/**
 * Service method
 */
struct ZSR_EXPORT ServiceMethod
{
    META_TYPE(ServiceMethod, Service)

    std::string ident;

    TypeRef requestType;
    TypeRef responseType;

    std::function<Variant(::zserio::IService&, const Variant&)> call;

    /**
     * Object passed as user-data to zserio generated
     * service functions.
     *
     * Contains the original, non-serialized, request data plus
     * meta information about the service method being called.
     */
    struct Context
    {
        const struct Service& service;
        const struct ServiceMethod& method;

        const zsr::Variant& request;
    };
};

/**
 * Service
 */
struct ZSR_EXPORT Service
{
    META_TYPE(Service, Package)

    std::string ident;
    std::deque<ServiceMethod> methods;
};

/**
 * Package
 */
struct ZSR_EXPORT Package
{
    META_TYPE_ROOT(Package)

    std::string ident;

    std::deque<SubType> subTypes;
    std::deque<Constant> constants;
    std::deque<Enumeration> enumerations;
    std::deque<Bitmask> bitmasks;
    std::deque<Compound> compounds;
    std::deque<Service> services;
};

} // namespace zsr

#undef META_TYPE_ROOT
#undef META_TYPE

#if _MSC_VER
#pragma warning(pop)
#endif
