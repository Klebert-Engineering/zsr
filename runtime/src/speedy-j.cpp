#include <zsr/speedy-j.hpp>

namespace speedyj
{

static std::string jsonEncoded(const std::string& s)
{
    std::string res;
    res.reserve(s.size());

    for (auto c : s) {
        switch (c) {
        case '"':
            res += "\\\"";
            break;

        case '\\':
            res += "\\\\";
            break;

        default:
            if (c <= ' ') {
                char uliteral[7] = {0};
                snprintf(uliteral, sizeof(uliteral), "\\u%04d", (int)c);
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
            s.ss_ << (s.state().itemIdx % 2 == 0 ? ',' : ':');
            break;
        }
    }

    s.state().itemIdx++;
}

static void checkedNext(Stream& s)
{
    if (s.state_.empty())
        throw std::runtime_error("checkedNext: stack is empty");

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
        s.ss_ << '}';
        break;
    }
}

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
        throw std::runtime_error("pop: stack is empty");

    finish(*this);
    state_.pop();

    return *this;
}

StreamState& Stream::state()
{
    if (state_.empty())
        throw std::runtime_error("state: stack is empty");

    return state_.top();
}

const StreamState& Stream::state() const
{
    if (state_.empty())
        throw std::runtime_error("state: stack is empty");

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
