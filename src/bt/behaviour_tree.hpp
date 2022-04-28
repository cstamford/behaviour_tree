#pragma once

#include <cstdint>
#include <functional>
#include <memory>
#include <vector>

namespace bt
{
    enum class status : uint8_t
    {
        success,
        running,
        fail,
    };

    struct node_i
    {
        virtual ~node_i() = default;
        virtual status tick() = 0;
    };

    using node_ptr = std::unique_ptr<node_i>;
    using node_collection = std::vector<node_ptr>;

    namespace action
    {
        // Always returns success.
        class always_succeed final : public node_i
        {
        public:
            static node_ptr make();
            virtual status tick() override;
        };

        // Always returns failure.
        class always_fail final : public node_i
        {
        public:
            static node_ptr make();
            virtual status tick() override;
        };

        // Executes the function and returns the result.
        // This is a convenient way to implement an action without creating a custom action node.
        class functor final : public node_i
        {
        public:
            using func = std::function<status()>;
            functor(func&& func);
            static node_ptr make(func&& func);

            virtual status tick() override;

        private:
            func _func;
        };

        inline node_ptr always_succeed::make()
        {
            return std::make_unique<always_succeed>();
        }

        inline node_ptr always_fail::make()
        {
            return std::make_unique<always_fail>();
        }

        inline node_ptr functor::make(func&& func)
        {
            return std::make_unique<functor>(std::move(func));
        }
    }

    namespace composite
    {
        // Selectors tick their children from first to last.
        // * If a child returns success or running, the selector returns the same.
        // * If a child returns fail, the selector moves onto the next child.
        // * If all children return fail, the selector returns fail.
        class selector final : public node_i
        {
        public:
            selector(node_collection&& children);

            template <typename ... Args>
            static node_ptr make(Args&& ... children);

            virtual status tick() override;

        private:
            node_collection _children;
        };

        // Sequences tick their children from first to last.
        // * If a child returns success, the sequence moves onto the next child.
        // * If a child returns running or fail, the sequence returns the same.
        class sequence final : public node_i
        {
        public:
            sequence(node_collection&& children);

            template <typename ... Args>
            static node_ptr make(Args&& ... children);

            virtual status tick() override;

        private:
            node_collection _children;
        };

        template <typename ... Args>
        inline node_ptr selector::make(Args&& ... children)
        {
            node_collection col;
            (col.emplace_back(std::forward<Args>(children)), ...);
            return std::make_unique<selector>(std::move(col));
        }

        template <typename ... Args>
        inline node_ptr sequence::make(Args&& ... children)
        {
            node_collection col;
            (col.emplace_back(std::forward<Args>(children)), ...);
            return std::make_unique<sequence>(std::move(col));
        }
    }

    namespace decorator
    {
        // Conditions evaluate a predicate and (conditionally) executes their child.
        // * If the predicate returns false, the condition returns fail.
        // * If the predicate returns true, the condition returns the child's status.
        class condition final : public node_i
        {
        public:
            using pred = std::function<bool()>;
            condition(pred&& predicate, node_ptr&& child);
            static node_ptr make(pred&& predicate, node_ptr&& child);

            virtual status tick() override;

        private:
            pred _predicate;
            node_ptr _child;
        };

        // Inverters execute their child and then invert their result.
        // * If the child returns success, the inverter returns fail.
        // * If the child returns running, the inverter returns running.
        // * If the child returns fail, the inverter returns success.
        class inverter final : public node_i
        {
        public:
            inverter(node_ptr&& child);
            static node_ptr make(node_ptr&& child);

            virtual status tick() override;

        private:
            node_ptr _child;
        };

        inline node_ptr condition::make(pred&& predicate, node_ptr&& child)
        {
            return std::make_unique<condition>(std::move(predicate), std::move(child));
        }

        inline node_ptr inverter::make(node_ptr&& child)
        {
            return std::make_unique<inverter>(std::move(child));
        }
    }
}
