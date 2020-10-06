#pragma once

#include <optional>
#include <algorithm>
#include <deque>

namespace speedyj::impl
{

template <class _T>
struct TypeHelper
{
    static void push(Stream& s, const _T& v)
    {
        s.push(v);
    }
};

template <class _T>
struct TypeHelper<std::optional<_T>>
{
    static void push(Stream& s, const _T& v)
    {
        if (v)
            s.push(*v);
        else
            s << speedyj::Null;
    }
};

template <class _T, class _Alloc>
struct TypeHelper<std::vector<_T, _Alloc>>
{
    static void push(Stream& s, const std::vector<_T>& v)
    {
        try {
            s << speedyj::Array;
            std::for_each(v.begin(), v.end(), [&](const auto& vv) {
                s << vv;
            });
            s << speedyj::End;
        } catch (...) {
            s << speedyj::End;
            throw;
        }
    }
};

template <class _T, class _Alloc>
struct TypeHelper<std::deque<_T, _Alloc>>
{
    static void push(Stream& s, const std::deque<_T>& v)
    {
        try {
            s << speedyj::Array;
            std::for_each(v.begin(), v.end(), [&](const auto& vv) {
                s << vv;
            });
            s << speedyj::End;
        } catch (...) {
            s << speedyj::End;
            throw;
        }
    }
};

template <>
struct TypeHelper<bool>
{
    static void push(Stream& s, const bool& v)
    {
        if (v)
            s << speedyj::True;
        else
            s << speedyj::False;
    }
};

}
