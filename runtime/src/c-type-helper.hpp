#pragma once

namespace zsr {

/**
 * Function for filling a zsr::CType instance for a given C type.
 */
template <class _Type, class _Enable = void>
struct CTypeTraits;

template <class _Type>
struct CTypeTraits<std::vector<_Type>>
{
    static auto set(CType& type)
    {
        CTypeTraits<_Type>::set(type);
        type.array = true;
    }
};

template <>
struct CTypeTraits<bool>
{
    static auto set(CType& type)
    {
        type.type = CType::Bool;
        type.size = 1u;
        type.array = false;
    }
};

template <class _Type>
struct CTypeTraits<_Type, std::enable_if_t<std::is_unsigned<_Type>::value>>
{
    static auto set(CType& type)
    {
        type.type = CType::UInt;
        type.size = sizeof(_Type);
        type.array = false;
    }
};

template <class _Type>
struct CTypeTraits<_Type,
                   std::enable_if_t<std::is_signed<_Type>::value &&
                                    !std::is_floating_point<_Type>::value>>
{
    static auto set(CType& type)
    {
        type.type = CType::Int;
        type.size = sizeof(_Type);
        type.array = false;
    }
};

template <class _Type>
struct CTypeTraits<_Type,
                   std::enable_if_t<std::is_floating_point<_Type>::value>>
{
    static auto set(CType& type)
    {
        type.type = CType::Float;
        type.size = sizeof(_Type);
        type.array = false;
    }
};

template <class _Type>
struct CTypeTraits<_Type, std::enable_if_t<std::is_enum<_Type>::value>>
{
    static auto set(CType& type)
    {
        type.type = CType::Enum;
        type.size = sizeof(std::underlying_type_t<_Type>);
        type.array = false;
    }
};

template <>
struct CTypeTraits<std::string>
{
    static auto set(CType& type)
    {
        type.type = CType::String;
        type.size = 0u;
        type.array = false;
    }
};

template <class _Type>
struct CTypeTraits<_Type, std::enable_if_t<is_compound<_Type>::value>>
{
    static auto set(CType& type)
    {
        type.type = CType::Structure;
        type.size = 0u;
        type.array = false;
    }
};

} // namespace zsr
