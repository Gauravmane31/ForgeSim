# ForgeSim Design Decisions

## 1. Purpose

This document records the major C++ and software-engineering decisions used in ForgeSim.

The goal is not only to make the system work, but to keep responsibilities separated and the implementation understandable and testable.

---

## 2. C++20

ForgeSim uses C++20.

The project primarily relies on standard C++ facilities for:

- Containers
- Smart pointers
- Threads
- Futures
- Synchronization
- Chrono timing
- Type traits
- Algorithms

No unnecessary framework is used for the core execution engine.

---

## 3. Runtime Polymorphism

ForgeSim uses an abstract `Computation` interface:

```cpp
class Computation
{
public:
    virtual ~Computation() = default;

    virtual Result calculate(
        const std::vector<Result>& inputs
    ) const = 0;
};
```

Concrete computation classes derive from this interface.

### Why?

The scheduler should not contain logic such as:

```text
if node is multiplication
if node is division
if node is constant
```

Instead, it can work with:

```cpp
std::shared_ptr<Computation>
```

and call:

```cpp
calculate(inputs)
```

This follows the principle of programming against an abstraction.

---

## 4. Virtual Destructor

The base class contains:

```cpp
virtual ~Computation() = default;
```

This is important because objects are used polymorphically.

Destroying a derived computation through a base-class pointer must invoke the correct derived destructor.

---

## 5. `const` Correctness

Read-only operations are marked `const`.

For example:

```cpp
const std::string& getName() const;
```

This communicates two things:

1. The returned string is not copied.
2. Calling the function does not modify the `Node`.

The same principle is used throughout the project.

---

## 6. References and `const` References

ForgeSim uses references when copying an object is unnecessary.

Example:

```cpp
const std::vector<Result>& inputs
```

This avoids copying the entire input vector.

The `const` qualifier prevents accidental modification.

---

## 7. `std::shared_ptr` for Computations

The scheduler stores computations as:

```cpp
std::unordered_map<
    int,
    std::shared_ptr<Computation>
>
```

### Why?

A computation object can be shared between the scheduler and the caller.

For example, a `ConstantComputation` may be retained by the caller so its value can later be changed.

`shared_ptr` provides shared ownership and automatic lifetime management.

The project does not manually call `delete`.

---

## 8. `std::move`

Move semantics are used when ownership of an object can be transferred instead of copied.

Example:

```cpp
computations[nodeId] = std::move(computation);
```

This avoids an unnecessary shared-pointer copy and clearly communicates transfer of the local handle into the scheduler.

---

## 9. `unordered_map`

The graph uses:

```cpp
std::unordered_map<int, Node>
```

### Reason

Nodes are identified by integer IDs.

Average lookup is approximately:

```text
O(1)
```

This is useful because the scheduler and graph algorithms frequently need to find nodes by ID.

---

## 10. `vector`

Vectors are used for:

- Dependencies
- Dependents
- Worker threads
- Computation inputs
- Test data

The dependency lists are typically small, so a vector provides a simple contiguous representation.

---

## 11. `unordered_set`

Impact analysis uses an unordered set to track visited nodes.

This prevents duplicate traversal.

This matters for graphs such as:

```text
       B
      ↗
A ───► C ───► D
      ↘
       E
```

A node can be reachable through multiple paths.

The set ensures each affected node is processed once.

---

## 12. Exceptions

ForgeSim uses exceptions for invalid execution conditions.

Examples:

```cpp
std::runtime_error
std::invalid_argument
```

Examples of invalid conditions:

- Division by zero
- Invalid computation input
- Missing node
- Null computation
- Cyclic graph

The intention is to fail explicitly instead of silently producing incorrect results.

---

## 13. RAII

ForgeSim follows RAII for resource management.

The most important example is the thread pool.

The `ThreadPool` destructor:

```text
signals shutdown
      ↓
wakes workers
      ↓
waits for workers
      ↓
joins threads
```

The caller does not manually manage worker-thread lifetime.

---

## 14. Deleted Copy Operations

The thread pool is non-copyable:

```cpp
ThreadPool(const ThreadPool&) = delete;
ThreadPool& operator=(const ThreadPool&) = delete;
```

A thread pool owns synchronization primitives and worker threads.

Copying it would not represent meaningful ownership semantics.

---

## 15. Result Validity

ForgeSim deliberately separates numerical value from validity.

These two states are different:

```text
value = 0, valid = true
```

and:

```text
value = 0, valid = false
```

This avoids using a special numerical value as an error indicator.

---

## 16. Separation of Concerns

The system avoids placing everything inside `Scheduler`.

For example:

```text
Graph
    → graph structure

ImpactAnalyzer
    → affected nodes

CacheManager
    → cached results

ThreadPool
    → worker execution

Scheduler
    → orchestration
```

This makes individual components easier to test and reason about.

---

## 17. Current Ownership Model

Conceptually:

```text
Scheduler
 ├── owns CacheManager
 ├── owns ImpactAnalyzer
 ├── owns ExecutionMetrics
 └── stores shared ownership handles to Computations
```

The graph is supplied to execution methods by reference.

The caller therefore controls graph lifetime.

---

## 18. Design Trade-offs

ForgeSim intentionally favors clarity over premature generalization.

Examples:

### In-memory cache

Simple and fast for the prototype, but it does not survive process restarts.

### Batch-oriented parallel execution

Simpler to reason about than a fully dynamic scheduler, but it can leave some available parallelism unused.

### Synthetic benchmark

Useful for demonstrating behavior, but it does not represent every real engineering workload.

These are explicit prototype trade-offs rather than accidental behavior.
