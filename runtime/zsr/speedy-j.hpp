#pragma once

#include "export.hpp"

#include <string>
#include <stack>
#include <vector>
#include <sstream>

namespace speedyj
{

/**
 * Internal stream state.
 */
struct StreamState
{
    enum Type {
        Object,
        Array
    } type;
    int itemIdx;

    ZSR_EXPORT StreamState(Type);
};

/**
 * Tiny wrapper over std::string stream for serializing
 * data directly to a json string.
 */
class Stream
{
public:
    ZSR_EXPORT Stream(Stream&&) = default;
    ZSR_EXPORT Stream& operator=(Stream&&) = default;

    /**
     * Create an empty json stream.
     *
     * Note: Push either Object or Array first, otherwise
     *       pushing values will throw.
     */
    ZSR_EXPORT Stream();

    /**
     * Returns the streams string value.
     */
    ZSR_EXPORT std::string str() const;

    /**
     * Push next value. Use operator << implementation.
     */
    ZSR_EXPORT Stream& push(const std::string&);
    ZSR_EXPORT Stream& push(unsigned long long);
    ZSR_EXPORT Stream& push(unsigned long);
    ZSR_EXPORT Stream& push(unsigned int);
    ZSR_EXPORT Stream& push(long long);
    ZSR_EXPORT Stream& push(long);
    ZSR_EXPORT Stream& push(int);
    ZSR_EXPORT Stream& push(double);

/* private */
    std::stringstream ss_;
    std::stack<StreamState, std::vector<StreamState>> state_;

    Stream& push(StreamState);
    Stream& pop();

    StreamState& state();
    const StreamState& state() const;
};

namespace impl {
template <class> struct TypeHelper;
}

template <class _Type>
Stream& operator<<(Stream& s, const _Type& v)
{
    impl::TypeHelper<_Type>::push(s, v);
    return s;
}

static constexpr struct Null_ {} Null;
static constexpr struct True_ {} True;
static constexpr struct False_ {} False;
static constexpr struct Array_ {} Array;
static constexpr struct Object_ {} Object;
static constexpr struct End_ {} End;

ZSR_EXPORT Stream& operator<<(Stream&, const Null_&);
ZSR_EXPORT Stream& operator<<(Stream&, const True_&);
ZSR_EXPORT Stream& operator<<(Stream&, const False_&);
ZSR_EXPORT Stream& operator<<(Stream&, const Array_&);
ZSR_EXPORT Stream& operator<<(Stream&, const Object_&);
ZSR_EXPORT Stream& operator<<(Stream&, const End_&);

struct ScopedObject
{
    Stream& s_;

    ScopedObject(Stream& s) : s_(s) { s_ << Object; }
    ~ScopedObject() { s_ << End; }
};

struct ScopedArray
{
    Stream& s_;

    ScopedArray(Stream& s) : s_(s) { s_ << Array; }
    ~ScopedArray() { s_ << End; }
};

}

#include "speedy-j-impl.hpp"
