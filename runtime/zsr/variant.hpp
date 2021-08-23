#pragma once

#include <algorithm>
#include <bitset>
#include <cstdint>
#include <optional>
#include <string>
#include <type_traits>
#include <utility>
#include <variant>
#include <vector>

#include "zsr/introspectable.hpp"
#include "zserio/BitBuffer.h"
#include "zserio/StringView.h"

#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable:4251)
#endif

/**
 * List of types the variant stores (+ vectors of them).
 */
/* clang-format off */
#define ZSR_VARIANT_TYPES(_)                    \
    _(int64_t)                                  \
    _(uint64_t)                                 \
    _(double)                                   \
    _(std::string)                              \
    _(zserio::BitBuffer)                        \
    _(zsr::Introspectable)
/* clang-format on */

namespace zsr {
namespace impl {

/**
 * List of types and their internal representation.
 */
/* clang-format off */
#define TYPELIST(_)                                     \
    /* external                      internal */        \
    _(bool,                          uint64_t)          \
    _(uint8_t,                       uint64_t)          \
    _(uint16_t,                      uint64_t)          \
    _(uint32_t,                      uint64_t)          \
    _(uint64_t,                      uint64_t)          \
    _(int8_t,                        int64_t)           \
    _(int16_t,                       int64_t)           \
    _(int32_t,                       int64_t)           \
    _(int64_t,                       int64_t)           \
    _(float,                         double)            \
    _(double,                        double)            \
    _(const char*,                   std::string)       \
    _(zserio::BasicStringView<char>, std::string)       \
    _(std::string,                   std::string)       \
    _(zserio::BitBuffer,             zserio::BitBuffer) \
    _(Introspectable,                Introspectable)
/* clang-format on */

#define GEN_EXT2INT(from, to)                                                  \
    template <>                                                                \
    struct Ext2Int<from>                                                       \
    {                                                                          \
        using Type = to;                                                       \
    };

/**
 * Trait class for mapping external types to internal ones,
 * e.g. uint8_t -> uint64_t.
 */
template <typename _T, class _Enable = void>
struct Ext2Int;

/* Generate `Ext2Int` for `TYPELIST` */
TYPELIST(GEN_EXT2INT)

#undef GEN_EXT2INT
#undef TYPELIST

template <typename _T>
using Ext2IntT = typename Ext2Int<_T>::Type;

/* Specialization for std::vector<T> */
template <class _T>
struct Ext2Int<std::vector<_T>>
{
    using Type = std::vector<Ext2IntT<_T>>;
};

/* Specialization for std::bitset<N> */
template <std::size_t _N>
struct Ext2Int<std::bitset<_N>>
{
    using Type = std::conditional_t<_N <= 64u, uint64_t, void>;
};

/* Specialization for enum/enum class */
template <class _T>
struct Ext2Int<_T, std::enable_if_t<std::is_enum<_T>::value>>
{
    using Type = Ext2IntT<std::underlying_type_t<_T>>;
};

template <class _T>
struct Ext2Int<_T, std::enable_if_t<std::is_enum<typename _T::Values>::value>>
{
    using Type = Ext2IntT<std::underlying_type_t<typename _T::Values>>;
};

/**
 * Type casting between external and internal types.
 * See TYPELIST.
 */
template <class _Source, class _Target, class _Enable = void>
struct VariantCast;

/**
 * A -> A
 */
template <class _Target>
struct VariantCast<_Target, _Target, void>
{
    template <class _Input>
    static _Target cast(_Input&& s)
    {
        return std::forward<_Input>(s);
    }
};

/**
 * BasicStringView<A> -> B
 */
template <class _Char, class _Target>
struct VariantCast<zserio::BasicStringView<_Char>, _Target, void>
{
    template <class _Input>
    static _Target cast(_Input&& s)
    {
        return _Target(s.begin(), s.end());
    }
};


/**
 * Bitmask<A> -> B
 */
template <class _Source, class _Target>
struct VariantCast<_Source,
                   _Target,
                   std::enable_if_t<std::is_enum<typename _Source::Values>::value>>
{
    template <class _Input>
    static _Target cast(_Input&& s)
    {
        return static_cast<_Target>(s.getValue());
    }
};

/**
 * A -> B
 */
template <class _Source, class _Target, class _Enable>
struct VariantCast
{
    static _Target cast(const _Source& s)
    {
        return static_cast<_Target>(s);
    }
};

/**
 * A -> B (Bitmask)
 */
template <class _Source, class _Target>
struct VariantCast<_Source,
                   _Target,
                   std::enable_if_t<std::is_enum<typename _Target::Values>::value>>
{
    static _Target cast(const _Source& s)
    {
        return static_cast<_Target>(std::underlying_type_t<typename _Target::Values>(s));
    }
};

/**
 * vector<A> -> vector<B>
 */
template <class _Source, class _Target>
struct VariantCast<std::vector<_Source>,
                   std::vector<_Target>,
                   std::enable_if_t<!std::is_same<_Source, _Target>()>>
{
    static std::vector<_Target> cast(const std::vector<_Source>& s)
    {
        std::vector<_Target> t;
        t.resize(s.size());
        std::transform(s.begin(), s.end(), t.begin(), [](const auto& v) {
            return VariantCast<_Source, _Target>::cast(v);
        });

        return t;
    }
};

/**
 * bitset<N> -> T
 */
template <std::size_t _N, class _Target>
struct VariantCast<std::bitset<_N>, _Target>
{
    static _Target cast(const std::bitset<_N>& s)
    {
        return s.to_ullong();
    }
};

} // namespace impl

/**
 * Zsr value container.
 */
class ZSR_EXPORT Variant final
{
public:
    Variant()
    {}

    template <class _T>
    Variant(_T&& v)
    {
        set(std::forward<_T>(v));
    }

    Variant(const Variant&) = default;
    Variant& operator=(const Variant&) = default;

    Variant(Variant&& o)
        : val_(std::move(o.val_))
    {}

    Variant& operator=(Variant&& o)
    {
        val_ = std::move(o.val_);
        return *this;
    }

    template <typename _T>
    std::optional<_T> get() const&
    {
        if (auto p = std::get_if<impl::Ext2IntT<_T>>(&val_)) {
            return impl::VariantCast<impl::Ext2IntT<_T>, _T>::cast(*p);
        } else {
            return std::nullopt;
        }
    }

    template <typename _T>
    std::optional<_T> get() &&
    {
        if (auto p = std::get_if<impl::Ext2IntT<_T>>(&val_)) {
            return impl::VariantCast<impl::Ext2IntT<_T>, _T>::cast(
                std::move(*p));
        } else {
            return std::nullopt;
        }
    }

    template <typename _T>
    void set(_T&& v)
    {
        val_ = impl::VariantCast<
            std::decay_t<_T>,
            impl::Ext2IntT<std::decay_t<_T>>>::cast(std::forward<_T>(v));
    }

    bool empty() const
    {
        return val_.index() == 0;
    }

    bool operator==(const Variant& v) const
    {
        return val_ == v.val_;
    }

    bool operator!=(const Variant& v) const
    {
        return !(*this == v);
    }

private:
#define GEN(type)                               \
        , type, std::vector<type>

    std::variant<std::monostate
                 ZSR_VARIANT_TYPES(GEN)
                 >
        val_;

#undef GEN
};



/**
 * Function for visiting variant values.
 *
 * _Fun must implement ()(T) and ()(std::vector<T>) and ()(void).
 */
template <class _Fun>
auto visit(const zsr::Variant& v, _Fun& f)
{
#define GEN(type)                                   \
    if (auto val = v.get< type >()) {               \
        return f(*val);                             \
    }                                               \
    if (auto val = v.get<std::vector< type >>()) {  \
        return f(*val);                             \
    }

    ZSR_VARIANT_TYPES(GEN)

    return f(); /* Fallback: empty variant */
#undef GEN
}

} // namespace zsr

#ifdef _MSC_VER
#pragma warning(pop)
#endif
