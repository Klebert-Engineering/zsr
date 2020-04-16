#pragma once

#include <any>
#include <memory>

#include "introspectable-private.hpp"

namespace zsr {

struct Compound;

/**
 * Introspectable, type-erasing, container
 * for zserio structure instances.
 *
 * Instanciated by `Compound::alloc`.
 */
class Introspectable
{
public:
    Introspectable(const Compound*, std::shared_ptr<impl::InstanceBase>);

    Introspectable(const Introspectable&);
    Introspectable& operator=(const Introspectable&);

    Introspectable(Introspectable&&);
    Introspectable& operator=(Introspectable&&);

    ~Introspectable();

    bool isOwning() const;
    const Compound* meta() const;

    std::shared_ptr<impl::InstanceBase> obj;
};

} // namespace zsr
