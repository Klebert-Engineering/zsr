#pragma once

#include <type_traits>
#include <variant>
#include <utility>
#include <optional>
#include <algorithm>
#include <string>
#include <vector>
#include <bitset>
#include <cstdint>

#include "zsr/introspectable.hpp"

namespace zsr {
namespace impl {

/**
 * List of types and their internal representation.
 */
/* clang-format off */
#define TYPELIST(_)                             \
    /* external       internal */               \
    _(bool,           uint64_t)                 \
    _(uint8_t,        uint64_t)                 \
    _(uint16_t,       uint64_t)                 \
    _(uint32_t,       uint64_t)                 \
    _(uint64_t,       uint64_t)                 \
    _(int8_t,         int64_t)                  \
    _(int16_t,        int64_t)                  \
    _(int32_t,        int64_t)                  \
    _(int64_t,        int64_t)                  \
    _(float,          double)                   \
    _(double,         double)                   \
    _(const char*,    std::string)              \
    _(std::string,    std::string)              \
    _(Introspectable, Introspectable)
/* clang-format on */

#define GEN_EXT2INT(from, to)                   \
    template <>                                 \
    struct Ext2Int<from> {                      \
        using Type = to;                        \
    };

/**
 * Trait class for mapping external types to internal ones,
 * e.g. uint8_t -> uint64_t.
 */
template <typename _T, class _Enable = void>
struct Ext2Int;

template <typename _T>
using Ext2IntT = typename Ext2Int<_T>::Type;

/* Generate `Ext2Int` for `TYPELIST` */
TYPELIST(GEN_EXT2INT)

#undef GEN_EXT2INT
#undef TYPELIST

/* Specialization for std::vector<T> */
template <class _T>
struct Ext2Int<std::vector<_T>> {
    using Type = std::vector<Ext2IntT<_T>>;
};

/* Specialization for std::bitset<N> */
template <std::size_t _N>
struct Ext2Int<std::bitset<_N>> {
    using Type = std::conditional_t<_N <= 64u, uint64_t, void>;
};

/* Specialization for enum/enum class */
template <class _T>
struct Ext2Int<_T, std::enable_if_t<std::is_enum<_T>::value>> {
    using Type = Ext2IntT<std::underlying_type_t<_T>>;
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
struct VariantCast<_Target, _Target, void> {
    template <class _Input>
    static _Target cast(_Input&& s) {
        return std::forward<_Input>(s);
    }
};

/**
 * A -> B
 */
template <class _Source, class _Target, class _Enable>
struct VariantCast {
    static _Target cast(const _Source& s) {
        return static_cast<_Target>(s);
    }
};

/**
 * vector<A> -> vector<B>
 */
template <class _Source, class _Target>
struct VariantCast<std::vector<_Source>, std::vector<_Target>, std::enable_if_t<!std::is_same<_Source, _Target>()>> {
    static std::vector<_Target> cast(const std::vector<_Source>& s) {
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
struct VariantCast<std::bitset<_N>, _Target> {
    static _Target cast(const std::bitset<_N>& s) {
        return s.to_ullong();
    }
};

}

/**
 * Zsr value container.
 */
class Variant final {
public:
    Variant()
    {}

    template <class _T>
    Variant(_T&& v) {
        set<_T>(std::forward<_T>(v));
    }

    Variant(const Variant&) = delete;
    Variant& operator=(const Variant&) = delete;

    Variant(Variant&& o)
        : val_(std::move(o.val_))
    {}

    Variant& operator=(Variant&& o) {
        val_ = std::move(o.val_);
        return *this;
    }

    template <typename _T>
    std::optional<_T> get() const& {
        if (auto p = std::get_if<impl::Ext2IntT<_T>>(&val_)) {
            return impl::VariantCast<impl::Ext2IntT<_T>, _T>::cast(*p);
        } else {
            return std::nullopt;
        }
    }

    template <typename _T>
    std::optional<_T> get() && {
        if (auto p = std::get_if<impl::Ext2IntT<_T>>(&val_)) {
            return impl::VariantCast<impl::Ext2IntT<_T>, _T>::cast(std::move(*p));
        } else {
            return std::nullopt;
        }
    }

    template <typename _T>
    void set(_T&& v) {
        val_ = impl::VariantCast<std::decay_t<_T>,
                                 impl::Ext2IntT<
                                     std::decay_t<_T>>>::cast(std::forward<_T>(v));
    }

private:
    std::variant<
        std::monostate,
        /* non-array */
        uint64_t,
        int64_t,
        double,
        std::string,
        Introspectable,
        /* array */
        std::vector<uint64_t>,
        std::vector<int64_t>,
        std::vector<double>,
        std::vector<std::string>,
        std::vector<Introspectable>
    > val_;
};

}
