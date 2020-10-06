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

    StreamState(Type);
};

/**
 * Tiny wrapper over std::string stream for serializing
 * data directly to a json string.
 */
class Stream
{
public:
    Stream(Stream&&) = default;
    Stream& operator=(Stream&&) = default;

    /**
     * Create an empty json stream.
     *
     * Note: Push either Object or Array first, otherwise
     *       pushing values will throw.
     */
    Stream();

    /**
     * Returns the streams string value.
     */
    std::string str() const;

    /**
     * Push next value. Use operator << implementation.
     */
    Stream& push(const std::string&);
    Stream& push(uint64_t);
    Stream& push(int64_t);
    Stream& push(double);

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

Stream& operator<<(Stream&, const Null_&);
Stream& operator<<(Stream&, const True_&);
Stream& operator<<(Stream&, const False_&);
Stream& operator<<(Stream&, const Array_&);
Stream& operator<<(Stream&, const Object_&);
Stream& operator<<(Stream&, const End_&);

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
