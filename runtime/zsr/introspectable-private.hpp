#pragma once

#include "error.hpp"
#include "export.hpp"

#include <cassert>
#include <unordered_map>
#include <vector>

#include <zserio/BitStreamReader.h>

namespace zsr {

struct Compound;

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

    template <class _MetaType>
    void makeChildrenOwning(_MetaType* meta)
    {
        auto iter = children.find(reinterpret_cast<uintptr_t>(meta));
        if (iter != children.end()) {
            for (auto weakChild : iter->second) {
                if (auto child = weakChild.lock()) {
                    child->makeOwning();
                }
            }

            children.erase(iter);
        }
    }

    /**
     * Copies the pointed to zserio compound
     */
    virtual std::shared_ptr<InstanceBase> copy() = 0;

    /**
     * Copies the pointed to zserio compound if it is a weak-ref.
     */
    virtual void makeOwning() = 0;


    /**
     * Returns if this instance holds a strong (true) or a weak (false)
     * reference to its zserio object.
     */
    virtual bool isOwning() const = 0;


    /**
     * Introspection metadata.
     */
    const Compound* meta;

    /**
     * Introspectable has to keep parameters alive
     * because zserio generated code stores pointers
     * to them.
     */
    std::any parameters;

    /**
     * Owner/child relationship.
     *
     * Children have to keep alive their parent object, to keep
     * alive their (non-owned) zserio object pointer.
     *
     * Children do not remove themselves (we would need to store
     * the source meta uint-ptr with the object to be (efficiently) able
     * to remove ourselves from the map).
     *
     * Objects allocated using `alloc` (owning objects) have no
     * parent.
     */
    std::shared_ptr<InstanceBase> parent;
    std::unordered_map<uintptr_t, std::vector<std::weak_ptr<InstanceBase>>>
        children;
};

template <class _T>
struct Instance : InstanceBase
{
    Instance(std::shared_ptr<_T> obj, bool owning)
        : owning(owning)
        , obj(obj)
    {}

    std::shared_ptr<InstanceBase> copy() override
    {
        return std::make_shared<Instance<_T>>(
            std::make_shared<_T>(*obj), true);
    }

    void makeOwning() override
    {
        if (!owning) {
            auto copy = std::make_shared<_T>(*obj);
            obj.swap(copy);
            owning = true;

            /* Our children point to our prev. obj and keep it alive.
             * We still need to hold our parent, to keep our own
             * parameters alive. */
            children.clear();
        }
    }

    bool isOwning() const override
    {
        return owning;
    }

    bool owning;
    std::shared_ptr<_T> obj;
};

template <class _MetaType, class _T>
auto makeWeakInstance(const std::shared_ptr<InstanceBase>& master,
                      _MetaType* meta,
                      _T* obj)
{
    assert(master.get());
    assert(meta);
    assert(obj);

    auto instance =
        std::make_shared<Instance<_T>>(std::shared_ptr<_T>(master, obj), false);

    master->children[reinterpret_cast<uintptr_t>(meta)].push_back(instance);
    instance->parent = master;

    return instance;
}

template <class _T>
auto makeUniqueInstance()
{
    return std::make_shared<Instance<_T>>(
        std::make_shared<_T>(), true);
}

template <class _T, class... _Args>
auto readUniqueInstance(::zserio::BitStreamReader& reader, _Args&& ...args)
{
    return std::make_shared<Instance<_T>>(
        std::make_shared<_T>(reader, std::forward<_Args>(args)...), true);
}

template <class _T>
auto makeUniqueInstance(_T&& v)
{
    using Type = std::decay_t<_T>;

    return std::make_shared<Instance<Type>>(
        std::make_shared<Type>(std::forward<_T>(v)), true);
}

} // namespace impl
} // namespace zsr
