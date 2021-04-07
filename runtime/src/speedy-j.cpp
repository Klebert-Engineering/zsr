#include <zsr/speedy-j.hpp>

#include <limits>

namespace speedyj
{

#define IS_EVEN(x) (bool)((x) % 2 == 0)

static std::string jsonEncoded(const std::string& s)
{
    std::string res;
    res.reserve(s.size());

    for (char c : s) {
        switch (c) {
        case '"':
            res += "\\\"";
            break;

        case '\\':
            res += "\\\\";
            break;

        case '\n':
            res += "\\n";
            break;

        case '\r':
            res += "\\r";
            break;

        case 0x08: /* backspace */
            res += "\\b";
            break;

        case 0x0c: /* form-feed */
            res += "\\f";
            break;

        case '\t':
            res += "\\t";
            break;

        default:
            if (c < ' ' || c > std::numeric_limits<signed char>::max()) {
                /* Not optimal for multibyte codepoints, but
                 * it should work. */
                char uliteral[7] = {0};
                snprintf(uliteral, sizeof(uliteral), "\\u%04X",
                         (unsigned)(c < 0 ? std::numeric_limits<unsigned char>::max() + 1u + c : c));
                res += uliteral;
            } else {
                res.push_back(c);
            }
        }
    }

    return res;
}

static void next(Stream& s)
{
    if (s.state_.empty())
        return;
   
    if (s.state().itemIdx > 0) {
        switch (s.state().type) {
        case StreamState::Array:
            s.ss_ << ',';
            break;
        case StreamState::Object:
            s.ss_ << (IS_EVEN(s.state().itemIdx) ? ',' : ':');
            break;
        }
    }

    s.state().itemIdx++;
}

static void checkedNext(Stream& s)
{
    if (s.state_.empty())
        throw Error("checkedNext: Stack is empty");

    next(s);
}

static void begin(Stream& s)
{
    switch (s.state().type) {
    case StreamState::Array:
        s.ss_ << '[';
        break;
    case StreamState::Object:
        s.ss_ << '{';
        break;
    }
}

static void finish(Stream& s)
{
    switch (s.state().type) {
    case StreamState::Array:
        s.ss_ << ']';
        break;
    case StreamState::Object:
        if (!IS_EVEN(s.state().itemIdx))
            throw Error("finish: Key value count missmatch");
        s.ss_ << '}';
        break;
    }
}

Error::Error(const char* msg)
    : std::runtime_error(msg)
{}

StreamState::StreamState(StreamState::Type t)
    : type(t)
    , itemIdx(0)
{}

Stream::Stream()
{}

std::string Stream::str() const
{
    return ss_.str();
}

Stream& Stream::push(const std::string& s)
{
    checkedNext(*this);
    ss_ << '"' << jsonEncoded(s) << '"';
    return *this;
}

Stream& Stream::push(unsigned long long u)
{
    checkedNext(*this);
    ss_ << u;
    return *this;
}

Stream& Stream::push(unsigned long u)
{
    checkedNext(*this);
    ss_ << u;
    return *this;
}

Stream& Stream::push(unsigned int u)
{
    checkedNext(*this);
    ss_ << u;
    return *this;
}

Stream& Stream::push(long long s)
{
    checkedNext(*this);
    ss_ << s;
    return *this;
}

Stream& Stream::push(long s)
{
    checkedNext(*this);
    ss_ << s;
    return *this;
}

Stream& Stream::push(int s)
{
    checkedNext(*this);
    ss_ << s;
    return *this;
}

Stream& Stream::push(double f)
{
    checkedNext(*this);
    ss_ << f;
    return *this;
}

Stream& Stream::push(StreamState s)
{
    next(*this);
    state_.push(s);
    begin(*this);

    return *this;
}

Stream& Stream::pop()
{
    if (state_.empty())
        throw Error("pop: Stack is empty");

    finish(*this);
    state_.pop();

    return *this;
}

StreamState& Stream::state()
{
    if (state_.empty())
        throw Error("state: Stack is empty");

    return state_.top();
}

const StreamState& Stream::state() const
{
    if (state_.empty())
        throw Error("state: Stack is empty");

    return state_.top();
}

/* Stream Operators */

speedyj::Stream& operator<<(speedyj::Stream& s, const Null_&)
{
    next(s);
    s.ss_ << "null";
    return s;
}

speedyj::Stream& operator<<(speedyj::Stream& s, const True_&)
{
    next(s);
    s.ss_ << "true";
    return s;
}

speedyj::Stream& operator<<(speedyj::Stream& s, const False_&)
{
    next(s);
    s.ss_ << "false";
    return s;
}

speedyj::Stream& operator<<(speedyj::Stream& s, const Array_&)
{
    return s.push(StreamState(StreamState::Array));
}

speedyj::Stream& operator<<(speedyj::Stream& s, const Object_&)
{
    return s.push(StreamState(StreamState::Object));
}

speedyj::Stream& operator<<(speedyj::Stream& s, const End_&)
{
    return s.pop();
}

}
