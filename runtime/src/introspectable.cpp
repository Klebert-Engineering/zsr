#include "zsr/introspectable.hpp"

namespace zsr {

Introspectable::Introspectable(const Compound* meta,
                               std::shared_ptr<impl::InstanceBase> obj)
    : obj(std::move(obj))
{
    this->obj->meta = meta;
}

Introspectable::Introspectable(const Introspectable& o) : obj(o.obj) {}

Introspectable& Introspectable::operator=(const Introspectable& o)
{
    obj = o.obj;
    return *this;
}

Introspectable::Introspectable(Introspectable&& o) : obj(std::move(o.obj)) {}

Introspectable& Introspectable::operator=(Introspectable&& o)
{
    obj = std::move(o.obj);
    return *this;
}

Introspectable::~Introspectable() {}

Introspectable Introspectable::copy() const
{
    return Introspectable(obj->meta, obj->copy());
}

bool Introspectable::isOwning() const
{
    return obj && obj->isOwning();
}

const Compound* Introspectable::meta() const
{
    return obj ? obj->meta : nullptr;
}

bool Introspectable::operator==(const Introspectable& o) const
{
    return obj == o.obj;
}

bool Introspectable::operator!=(const Introspectable& o) const
{
    return !(*this == o);
}

} // namespace zsr
