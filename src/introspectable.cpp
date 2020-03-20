#include "zsr/introspectable.hpp"

namespace zsr {

Introspectable::Introspectable(const Compound* meta,
                         std::shared_ptr<impl::InstanceBase> obj)
    : meta(meta)
    , obj(std::move(obj))
{}

Introspectable::Introspectable(const Introspectable& o)
    : meta(o.meta)
    , obj(o.obj)
{}

Introspectable& Introspectable::operator=(const Introspectable& o)
{
    obj = o.obj;
    meta = o.meta;
    return *this;
}

Introspectable::Introspectable(Introspectable&& o)
    : meta(o.meta)
    , obj(std::move(o.obj))
{}

Introspectable& Introspectable::operator=(Introspectable&& o)
{
    obj = std::move(o.obj);
    meta = o.meta;
    return *this;
}

Introspectable::~Introspectable()
{}

}
