#include <bt/behaviour_tree.hpp>
#include <cassert>

int main()
{
    using namespace bt::action;
    using namespace bt::composite;
    using namespace bt::decorator;

    bool success = false;

    bt::node_ptr root =
        selector::make(
            always_fail::make(),
            inverter::make(
                always_succeed::make()),
            functor::make(
                []() { return bt::status::fail; }),
            condition::make(
                [&]() { return success; },
                always_succeed::make()));

    const bt::status should_be_fail = root->tick();
    assert(should_be_fail == bt::status::fail);

    success = true;
    const bt::status should_be_success = root->tick();
    assert(should_be_success == bt::status::success);
}
