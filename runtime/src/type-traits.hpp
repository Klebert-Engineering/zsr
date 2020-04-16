#pragma once

#include <tuple>
#include <type_traits>
#include <vector>

namespace zsr {

/**
 * Returns if the type _Type is wrapped by std::vector<>.
 */
template <class _Type>
struct is_vector : std::false_type
{};

template <class _Type>
struct is_vector<std::vector<_Type>> : std::true_type
{};

/**
 * Remove std::vector container from type
 */
template <class _Type>
struct remove_vector
{
    using type = _Type;
};

template <class _Type>
struct remove_vector<std::vector<_Type>>
{
    using type = _Type;
};

template <class _Type>
using remove_vector_t = typename remove_vector<_Type>::type;

/**
 * Trait for getting type information about member functions.
 */
template <class _Function>
struct member_function_traits;

template <class _Result, class _Class, class... _Args>
struct member_function_traits<_Result (_Class::*)(_Args...)>
{
    using class_type = _Class;
    using return_type = _Result;
    using argument_tuple_type = std::tuple<std::decay_t<_Args>...>;
};

template <class _Function>
using argument_tuple_t =
    typename member_function_traits<_Function>::argument_tuple_type;

} // namespace zsr
