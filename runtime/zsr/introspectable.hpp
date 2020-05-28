#pragma once

#include <any>
#include <memory>

#include "export.hpp"
#include "introspectable-private.hpp"

#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable:4251)
#endif

namespace zsr {

struct Compound;

/**
 * Introspectable, type-erasing, container
 * for zserio structure instances.
 *
 * Instanciated by `Compound::alloc`.
 */
class ZSR_EXPORT Introspectable
{
public:
    struct FromObjectTag {};

    template <class _Type>
    Introspectable(FromObjectTag, const Compound* meta, _Type&& value)
        : obj(impl::makeUniqueInstance(std::forward<_Type>(value)))
    {
        obj->meta = meta;
    }

    Introspectable(const Compound*, std::shared_ptr<impl::InstanceBase>);

    Introspectable(const Introspectable&);
    Introspectable& operator=(const Introspectable&);

    Introspectable(Introspectable&&);
    Introspectable& operator=(Introspectable&&);

    ~Introspectable();

    Introspectable copy() const;

    bool isOwning() const;
    const Compound* meta() const;

    std::shared_ptr<impl::InstanceBase> obj;
};

} // namespace zsr

#ifdef _MSC_VER
#pragma warning(pop)
#endif
