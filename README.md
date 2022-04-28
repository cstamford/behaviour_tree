# behaviour_tree

## Description

This is a simple library which provides building blocks for creating and running behaviour trees in C++. In the future I will be creating a complementary GUI application to edit behaviour trees which can be stored as data. Custom node types and user-defined enumerations will be supported.

Why did I make this? I couldn't find a lightweight library with no dependencies to prototype game AI with. Every library I tested was either really heavy or needlessly complex. I wanted something nice and simple, and that's why we're here.

Why use this over alternatives such as [BehaviorTree.CPP](https://www.behaviortree.dev/)? Unless you are an advanced user who knows exactly what you want, you probably shouldn't. If you are an advanced user, you might appreciate how simple and flexible this library is. It's particularly suitable for creating AIs in video games.

## Example

The default nodes support constructing a behaviour tree using a fluid builder-style.

```cpp
node_ptr root =
    selector::make(
        always_fail::make(),
        inverter::make(
            always_succeed::make()),
        functor::make(
            []() { return status::running; }));

assert(root->tick() == status::running);
```

Trees can be trivially nested.

```cpp
node_ptr sub_tree = make_my_subtree(); // user code

node_ptr root =
    selector::make(
        always_fail::make(),
        std::move(sub_tree));
```

## Supported nodes

This library supports three node types: actions, composites, and decorators. In terms of implementation, there is no difference between these three types of nodes - they implement the same interface. They are conceptually distinct, not practically.

### node_i

`node_i` is the interface type for nodes. A node has only one requirement - it must implement the `tick` function which returns a `status` (success, running, or fail).

### Actions

Actions have no children. They should "do something". Think of them like a leaf in a tree. Users are expected to implement the majority of their logic by implementing custom actions which inherit from `node_i`.

#### always_succeed

Returns `status::success`.

#### always_fail

Returns `status::fail`.

#### functor

An alternative to implementing custom nodes, the user can instead provide a function which does the same thing.

### Composites

Composites may have multiple children. They are a holder of nodes which determine in which order their children are visited and processed.

#### selector

Selectors tick their children from first to last.

* If a child returns success or running, the selector returns the same.
* If a child returns fail, the selector moves onto the next child.
* If all children return fail, the selector returns fail.

#### sequence

Sequences tick their children from first to last.

* If a child returns success, the sequence moves onto the next child.
* If a child returns running or fail, the sequence returns the same.

### Decorators

Decorators have one child and may mutate the output of that child (or, indeed, omit updating the child as appropriate).

#### condition

Conditions evaluate a predicate and (conditionally) executes their child.

* If the predicate returns false, the condition returns fail.
* If the predicate returns true, the condition returns the child's status.

#### inverter

Inverters execute their child and then invert their result.

* If the child returns success, the inverter returns fail.
* If the child returns running, the inverter returns running.
* If the child returns fail, the inverter returns success.

## A note on performance

Every node is allocated on the heap and its lifetime managed by `std::unique_ptr`. Ticks are implemented using virtual functions. I don't think this is a big issue - but you might! If you do, let me know.

## Building

This is a cmake library, so just use cmake to link to the library and use it. Check the code under the test subdirectory for a minimal example.
