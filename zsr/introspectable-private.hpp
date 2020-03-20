#pragma once

#include "zsr/error.hpp"

namespace zsr {
namespace impl {

template <class _T>
struct Instance;

struct InstanceBase
{
    virtual ~InstanceBase() = default;

    template <class _T>
    Instance<_T>& as()
    {
        return static_cast<Instance<_T>&>(*this);
    }

    template <class _T>
    const Instance<_T>& as() const
    {
        return static_cast<const Instance<_T>&>(*this);
    }

    std::any parameters;
};

template <class _T>
struct Instance : InstanceBase {
    Instance(std::shared_ptr<_T> obj)
        : obj(obj)
    {}

    std::shared_ptr<_T> obj;
};

template <class _T>
auto makeWeakInstance(const std::shared_ptr<void>& master,
                      _T* obj) {
    return std::make_shared<Instance<_T>>(
        std::shared_ptr<_T>(master, obj));
}

template <class _T>
auto makeUniqueInstance() {
    return std::make_shared<Instance<_T>>(
        std::make_shared<_T>());
}

}
}
