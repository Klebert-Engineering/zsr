/**
 * Helper functions for working with parameterlist tuples.
 */
#pragma once

#include <memory>
#include <type_traits>

#include "zsr/types.hpp"
#include "zsr/variant.hpp"
#include "zsr/error.hpp"

namespace zsr::parameterlist {

template <class _Tuple>
struct add_shared_ptr_if_compound;

template <class... _Types>
struct add_shared_ptr_if_compound<std::tuple<_Types...>>
{
    using input_type = std::tuple<_Types...>;
    using type = std::tuple<std::conditional_t<is_compound<_Types>::value,
                                               std::shared_ptr<_Types>,
                                               _Types>...>;
};

/**
 * Wrap tuple element in std::shared_ptr if it is a zserio
 * generated compound type.
 *
 * Example:
 *     std::tuple<int, nds::A, float>
 *   becomes
 *     std::tuple<int, std::shared_ptr<nds::A>, float>
 */
template <class _Tuple>
using add_shared_ptr_if_compound_t =
    typename add_shared_ptr_if_compound<_Tuple>::type;


template <class _Type>
struct deref_if_shared_impl
{
    static _Type& deref(_Type& v)
    {
        return v;
    }

    static const _Type& deref(const _Type& v)
    {
        return v;
    }
};

template <class _Type>
struct deref_if_shared_impl<std::shared_ptr<_Type>>
{
    static _Type& deref(const std::shared_ptr<_Type>& v)
    {
        return *v;
    }

    static const _Type& deref(const std::shared_ptr<const _Type>& v)
    {
        return *v;
    }
};

/**
 * Return a dereferenced input value if it is of type std::shared_ptr<T>.
 * Otherwise the input value will be returned unchanged.
 *
 * Example:
 *   Passing T:
 *     deref_if_shared(T) -> T&
 *   Passing std::shared_ptr<T>:
 *     deref_if_shared(std::shared_prt<T>&) -> T&
 */
template <class _Type>
auto& deref_if_shared(_Type&& v)
{
    return deref_if_shared_impl<std::decay_t<_Type>>::deref(
        std::forward<_Type>(v));
}


template <class _Type>
struct remove_shared_ptr
{
    using type = _Type;
};

template <class _Type>
struct remove_shared_ptr<std::shared_ptr<_Type>>
{
    using type = _Type;
};

/**
 * Remove std::shared_ptr container from type
 *
 * Example:
 *     remove_shared_ptr_t<std::shared_ptr<T>>
 *   becomes
 *     T
 */
template <class _Type>
using remove_shared_ptr_t = typename remove_shared_ptr<_Type>::type;


template <class _Type, bool _IsCompound = is_compound<_Type>::value>
struct unpack_variant
{
    static _Type unpack(const Variant& v,
                        const Type2Compound&)
    {
        if (auto unpacked = v.get<_Type>())
            return *unpacked;

        throw VariantCastError{};
    }
};

template <class _Type>
struct unpack_variant<_Type, true>
{
    static std::shared_ptr<_Type> unpack(const Variant& v,
                                         const Type2Compound& t2c)
    {
        if (auto unpacked = v.get<Introspectable>()) {
            return shared_introspectable_cast<_Type>(*unpacked,
                                                     t2c);
        }

        throw VariantCastError{};
    }
};

template <class _Type>
struct unpack_variant<std::vector<_Type>, true>
{
    static _Type unpack(const Variant& v,
                        const Type2Compound& t2c)
    {
        if (auto unpacked = v.get<std::vector<Introspectable>>()) {
            std::vector<_Type> casted;
            casted.reserve(unpacked->size());

            std::transform(unpacked->begin(),
                           unpacked->end(),
                           std::back_inserter(casted),
                           [t2c](const auto& i) {
                               introspectable_cast<_Type>(i, t2c);
                           });
            return casted;
        }

        throw VariantCastError{};
    }
};

/**
 * Function for storing a zsr::Variant as parameter into a
 * parameterlist tuple.
 */
template <size_t _Idx, class _Tuple>
auto set_parameter(_Tuple& list,
                   const Variant& value,
                   const Type2Compound& t2c)
{
    using element_type = std::tuple_element_t<_Idx, _Tuple>;

    std::get<_Idx>(list) =
        unpack_variant<remove_shared_ptr_t<element_type>>::unpack(value,
                                                                  t2c);
}


} // namespace zsr::parameterlist
