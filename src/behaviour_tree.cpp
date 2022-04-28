#include <bt/behaviour_tree.hpp>

namespace bt
{
    status action::always_succeed::tick()
    {
        return status::success;
    }

    status action::always_fail::tick()
    {
        return status::fail;
    }

    action::functor::functor(func&& func)
        : _func(std::move(func))
    { }

    status action::functor::tick()
    {
        return _func();
    }

    composite::selector::selector(node_collection&& children)
        : _children(std::move(children))
    { }

    status composite::selector::tick()
    {
        for (node_ptr& node : _children)
        {
            const status ret = node->tick();

            if (ret == status::fail)
                continue;

            return ret;
        }

        return status::fail;
    }

    composite::sequence::sequence(node_collection&& children)
        : _children(std::move(children))
    { }

    status composite::sequence::tick()
    {
        for (node_ptr& node : _children)
        {
            const status ret = node->tick();

            if (ret == status::success)
                continue;

            return ret;
        }

        return status::success;
    }

    decorator::condition::condition(pred&& predicate, node_ptr&& child)
        : _predicate(std::move(predicate)), _child(std::move(child))
    { }

    status decorator::condition::tick()
    {
        return _predicate() ? _child->tick() : status::fail;
    }

    decorator::inverter::inverter(node_ptr&& child)
        : _child(std::move(child))
    { }

    status decorator::inverter::tick()
    {
        const status ret = _child->tick();

        if (ret == status::success)
            return status::fail;

        if (ret == status::fail)
            return status::success;

        return status::running;
    }
}
