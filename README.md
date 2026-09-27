# ForgeSim

## Incremental Parallel Engineering Simulation Engine

> **Change one thing → don't recompute everything.**

ForgeSim is a **C++20 incremental and parallel computation engine** designed around dependency-aware execution.

The system models computations as a **Directed Acyclic Graph (DAG)**, analyzes dependency relationships, caches previously computed results, identifies the portion of the graph affected by a change, and executes independent computations concurrently using a custom **thread pool**.

The project is designed as a prototype for engineering-style simulation workloads where recalculating an entire model after every small change can be unnecessarily expensive.

---

# Key Features

- Dependency graph based computation model
- Directed Acyclic Graph (DAG) execution
- Cycle detection
- Topological sorting
- Forward dependency tracking
- Reverse dependency tracking
- Change detection
- Incremental impact analysis
- Result caching
- Cache invalidation
- Cache-hit measurement
- Sequential execution
- Parallel DAG execution
- Custom C++ thread pool
- Dependency-aware scheduling
- Runtime polymorphism for computations
- Exception-based error handling
- GoogleTest automated test suite
- Synthetic performance benchmarking
- Engineering-oriented simulation example
- C++20 implementation using standard library facilities

---

# Architecture

```text
                         ForgeSim
                            │
                            ▼
                    ┌───────────────┐
                    │ Model / Graph │
                    └───────┬───────┘
                            │
                            ▼
                    ┌───────────────┐
                    │ Dependency DAG│
                    └───────┬───────┘
                            │
               ┌────────────┴────────────┐
               │                         │
               ▼                         ▼
      ┌─────────────────┐       ┌─────────────────┐
      │ Impact Analyzer │       │ Topological     │
      │                 │       │ Analysis        │
      └────────┬────────┘       └────────┬────────┘
               │                         │
               └────────────┬────────────┘
                            │
                            ▼
                    ┌───────────────┐
                    │    Cache      │
                    │    Manager    │
                    └───────┬───────┘
                            │
                            ▼
                    ┌───────────────┐
                    │   Scheduler   │
                    └───────┬───────┘
                            │
                  ┌─────────┴─────────┐
                  │                   │
                  ▼                   ▼
          Sequential Path      Parallel Path
                                      │
                                      ▼
                              ┌───────────────┐
                              │  Thread Pool  │
                              └───────┬───────┘
                                      │
                                      ▼
                              ┌───────────────┐
                              │ Computations  │
                              └───────┬───────┘
                                      │
                                      ▼
                              ┌───────────────┐
                              │ Result Store  │
                              └───────────────┘
```

---

# Why ForgeSim?

Consider a simulation containing hundreds or thousands of computations.

If one input changes, a naive simulation engine may recompute the entire model.

ForgeSim instead determines:

```text
What changed?
      ↓
Which nodes depend on it?
      ↓
Which cached results are now invalid?
      ↓
Which computations actually need to run?
      ↓
Which of those computations can run in parallel?
```

This combines:

```text
Dependency Analysis
        +
Incremental Computation
        +
Caching
        +
Parallel Execution
        +
C++ Concurrency
```

---

# Core Design

ForgeSim separates the system into two major layers.

## Core Layer

Responsible for representing the computational model.

```text
Node
Graph
Result
Computation
ImpactAnalyzer
```

## Execution Layer

Responsible for executing the computational model efficiently.

```text
Scheduler
ThreadPool
CacheManager
ExecutionMetrics
Benchmark
```

This separation allows the dependency model and execution strategy to evolve independently.

---

# Dependency Graph

Every computation is represented as a node in a dependency graph.

For example:

```text
        Engine RPM
             │
             ▼
        Output RPM
             ▲
             │
        Gear Ratio
```

The dependency relationships are stored in both directions.

For:

```text
A → B
```

ForgeSim stores:

```text
B.dependencies = {A}
A.dependents   = {B}
```

The forward relationship is useful for determining what a node requires.

The reverse relationship is particularly important for incremental execution because it allows ForgeSim to determine what becomes affected when a node changes.

---

# Example Engineering Model

The current demonstration models a simple drivetrain calculation.

```text
Engine RPM ─────────┐
                    │
                    ▼
                Output RPM
                    ▲
                    │
Gear Ratio ─────────┘
```

and:

```text
Engine Torque ──────┐
                    │
                    ▼
               Output Torque
                    ▲
                    │
Gear Ratio ─────────┘
```

The computations are:

```text
Output RPM = Engine RPM / Gear Ratio

Output Torque = Engine Torque × Gear Ratio
```

Initial values:

```text
Engine RPM     = 3000
Gear Ratio     = 4
Engine Torque  = 200
```

Therefore:

```text
Output RPM     = 3000 / 4 = 750

Output Torque  = 200 × 4 = 800
```

---

# Incremental Execution

Suppose the Engine RPM changes:

```text
3000 → 3200
```

Only the Engine RPM branch and computations depending on it need to be recomputed.

The torque branch remains unchanged.

ForgeSim performs:

```text
1. Detect changed node
2. Find affected downstream nodes
3. Invalidate affected cache entries
4. Determine dependency readiness
5. Schedule affected computations
6. Execute ready computations
7. Store new results
8. Preserve unaffected cached results
```

---

# Impact Analysis

ForgeSim uses reverse dependency relationships to perform downstream impact analysis.

Example:

```text
       B
      ↗
A ───► C ───► D
      ↘
       E
```

If `A` changes, the affected nodes are:

```text
A
B
C
D
E
```

The `ImpactAnalyzer` performs BFS-style traversal.

An `unordered_set` prevents duplicate processing when the graph contains structures such as diamond dependencies.

Complexity:

```text
O(V + E)
```

where:

```text
V = number of nodes
E = number of dependency edges
```

---

# DAG Validation

ForgeSim requires the computation model to be a Directed Acyclic Graph.

Valid:

```text
A → B → C → D
```

Invalid:

```text
A → B
↑   ↓
└── C
```

ForgeSim uses **Kahn's algorithm** for:

- Cycle detection
- Topological sorting

Complexity:

```text
Time  : O(V + E)
Space : O(V)
```

---

# Execution Modes

ForgeSim supports three execution modes.

## 1. Sequential Execution

The complete graph is evaluated in topological order.

API:

```cpp
scheduler.execute(graph);
```

This provides a baseline execution strategy.

---

## 2. Parallel Execution

Independent nodes can execute concurrently.

Example:

```text
             ┌── Node B ──┐
             │             │
Node A ──────┼── Node C ───┼──► Node E
             │             │
             └── Node D ───┘
```

Nodes B, C and D may execute concurrently because they depend on A but not on one another.

ForgeSim uses a custom thread pool.

API:

```cpp
scheduler.executeParallel(graph, workerCount);
```

---

## 3. Incremental Execution

Incremental execution combines:

```text
Change Detection
        +
Impact Analysis
        +
Cache
        +
Dependency Scheduling
        +
Parallel Execution
```

API:

```cpp
scheduler.executeIncremental(graph, workerCount);
```

The engine recomputes only the affected portion of the graph.

---

# Thread Pool

ForgeSim contains a custom thread pool implemented using C++ standard library concurrency primitives.

```text
                 Task Queue
                     │
       ┌─────────────┼─────────────┐
       ▼             ▼             ▼
   Worker 1      Worker 2      Worker 3
       │             │             │
       └─────────────┼─────────────┘
                     ▼
                 Results
```

The implementation uses:

```cpp
std::thread
std::mutex
std::condition_variable
std::queue
std::future
std::packaged_task
std::function
```

Workers wait when there is no work available.

When a task is submitted:

```text
Task submitted
      ↓
Task queue
      ↓
condition_variable notification
      ↓
Worker wakes
      ↓
Task executes
```

---

# Why a Thread Pool?

Creating and destroying a thread for every computation can introduce unnecessary overhead.

A thread pool allows workers to be created once and reused.

Benefits:

- Reduced thread creation overhead
- Controlled concurrency
- Reusable worker threads
- Centralized task management
- RAII-based shutdown

---

# Cache System

ForgeSim maintains an in-memory cache of computed results.

Conceptually:

```text
Node ID
   │
   ▼
Cached Result
```

The cache provides:

```text
store()
contains()
get()
invalidate()
clear()
```

An invalid result is distinguished from a valid numerical result.

For example:

```text
0.0
```

is a valid result.

It is not equivalent to:

```text
invalid result
```

---

# Cache Invalidation

When a node changes:

```text
Changed Node
     │
     ▼
Impact Analysis
     │
     ▼
Affected Nodes
     │
     ▼
Invalidate Cache
```

Unaffected nodes retain their cached values.

For example:

```text
1000 total nodes

998 unaffected
2 affected
```

The scheduler can reuse the 998 cached results.

---

# Computation Abstraction

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

Current computation types:

```text
ConstantComputation
MultiplyComputation
DivideComputation
WorkComputation
```

This allows the scheduler to remain independent of the actual mathematical implementation.

---

# Result Model

Each computation produces a `Result`.

A result contains:

```text
Value
Validity
```

This allows the engine to distinguish:

```text
Valid result = 0
```

from:

```text
Invalid result
```

---

# Node State

Nodes maintain an execution state:

```cpp
enum class NodeState
{
    DIRTY,
    READY,
    RUNNING,
    COMPLETED,
    FAILED
};
```

The states represent:

```text
DIRTY
  │
  ▼
READY
  │
  ▼
RUNNING
  │
  ├──────► COMPLETED
  │
  └──────► FAILED
```

Nodes also maintain a version counter that changes when they are marked dirty.

---

# Error Handling

ForgeSim uses C++ exceptions for invalid computation states.

Examples include:

```text
Invalid computation input
Division by zero
Incorrect number of inputs
Null computation registration
Missing graph node
Cyclic dependency graph
```

Exception types include:

```cpp
std::runtime_error
std::invalid_argument
```

---

# Metrics

ForgeSim tracks:

```text
Total Nodes
Computed Nodes
Cached Nodes
Cache Hit Rate
```

Example:

```text
Total Nodes : 1000
Computed     : 2
Cached       : 998
Cache Hit    : 99.80%
```

---

# Benchmark

A synthetic benchmark was created to evaluate:

1. Sequential execution
2. Parallel execution
3. Incremental execution
4. No-change cache reuse

Configuration:

```text
Nodes         : 1000
Workers       : 8
Work / Node   : 50000 iterations
```

## Results

```text
Sequential Full Execution
Time      : 1801.609 ms
Computed  : 1000

Parallel Full Execution
Time      : 274.876 ms
Computed  : 1000
Speedup   : 6.55x

Initial Incremental Execution
Time      : 278.299 ms
Computed  : 1000
Cached    : 0

Incremental Execution After Change
Changed Node : 500
Time         : 6.961 ms
Computed     : 2
Cached       : 998
Cache Hit    : 99.80%

No-Change Execution
Time         : 0.493 ms
Computed     : 0
Cached       : 1000
Cache Hit    : 100.00%
```

## Summary

| Execution Mode | Time | Computed | Cached |
|---|---:|---:|---:|
| Sequential Full | 1801.609 ms | 1000 | 0 |
| Parallel Full | 274.876 ms | 1000 | 0 |
| Incremental After Change | 6.961 ms | 2 | 998 |
| No Change | 0.493 ms | 0 | 1000 |

Measured full-execution speedup:

```text
1801.609 / 274.876 ≈ 6.55x
```

Incremental cache hit rate after changing Node 500:

```text
998 / 1000 × 100 = 99.80%
```

No-change cache hit rate:

```text
1000 / 1000 × 100 = 100%
```

> These measurements are specific to the stated synthetic workload, graph topology, worker count, compiler, hardware, and runtime environment. They are not universal performance guarantees.

---

# Parallelism Depends on Graph Structure

Parallel execution is not automatically faster for every dependency graph.

Highly sequential graph:

```text
A → B → C → D → E → F
```

There is little independent work.

Parallel graph:

```text
        ┌── B ──┐
        │       │
A ──────┼── C ──┼──► F
        │       │
        └── D ──┘
```

B, C and D can execute concurrently.

An earlier benchmark with a more dependency-heavy topology produced:

```text
Sequential : 851.942 ms
Parallel   : 1124.630 ms
Speedup    : 0.76x
```

This demonstrates that parallel execution introduces scheduling and synchronization overhead.

Therefore, parallel performance depends strongly on the amount of independent work exposed by the dependency graph.

---

# Testing

ForgeSim uses **GoogleTest**.

Tests cover:

### Node

- Construction
- Initial state
- Dependencies
- Dependents
- Duplicate dependency prevention
- State transitions
- Version changes

### Graph

- Node insertion
- Duplicate node rejection
- Dependency creation
- Cycle detection
- Topological sorting
- Dirty node detection

### Computation

- Constant computation
- Multiplication
- Division
- Invalid inputs
- Division by zero
- Exception handling

### Cache

- Empty cache
- Store
- Retrieve
- Invalidation
- Clear

### Impact Analyzer

- Downstream propagation
- Unrelated branches
- Diamond dependencies

### Scheduler

- Sequential execution
- Parallel execution
- Incremental execution
- Cache reuse

Run tests with:

```bash
ctest --test-dir build --output-on-failure
```

---

# Project Structure

```text
ForgeSim/
│
├── CMakeLists.txt
├── README.md
│
├── include/
│   └── ForgeSim/
│       │
│       ├── core/
│       │   ├── Node.hpp
│       │   ├── Graph.hpp
│       │   ├── Result.hpp
│       │   ├── Computation.hpp
│       │   ├── ConstantComputation.hpp
│       │   ├── MultiplyComputation.hpp
│       │   ├── DivideComputation.hpp
│       │   ├── ImpactAnalyzer.hpp
│       │   ├── BenchmarkGraph.hpp
│       │   └── WorkComputation.hpp
│       │
│       └── execution/
│           ├── Scheduler.hpp
│           ├── ThreadPool.hpp
│           ├── CacheManager.hpp
│           ├── ExecutionMetrics.hpp
│           └── Benchmark.hpp
│
├── src/
│   │
│   ├── main.cpp
│   │
│   ├── core/
│   │   ├── Node.cpp
│   │   ├── Graph.cpp
│   │   ├── Result.cpp
│   │   ├── ConstantComputation.cpp
│   │   ├── MultiplyComputation.cpp
│   │   ├── DivideComputation.cpp
│   │   ├── ImpactAnalyzer.cpp
│   │   ├── BenchmarkGraph.cpp
│   │   └── WorkComputation.cpp
│   │
│   └── execution/
│       ├── Scheduler.cpp
│       ├── ThreadPool.cpp
│       ├── CacheManager.cpp
│       ├── ExecutionMetrics.cpp
│       └── Benchmark.cpp
│
├── tests/
│   ├── NodeTest.cpp
│   ├── GraphTest.cpp
│   ├── ComputationTest.cpp
│   ├── CacheManagerTest.cpp
│   ├── ImpactAnalyzerTest.cpp
│   └── SchedulerTest.cpp
│
├── benchmarks/
├── examples/
│
└── docs/
```

---

# Technology Stack

| Category | Technology |
|---|---|
| Language | C++20 |
| Compiler | GCC |
| Build System | CMake |
| Testing | GoogleTest |
| Concurrency | C++ Standard Library |
| Data Structures | STL |
| Platform | Linux |
| Version Control | Git |

---

# Build and Run

## Prerequisites

Install:

- C++20 compatible compiler
- CMake
- GoogleTest

---

## Configure

```bash
cmake -S . -B build -DBUILD_TESTING=ON
```

## Build

```bash
cmake --build build
```

## Run Tests

```bash
ctest --test-dir build --output-on-failure
```

## Run ForgeSim

```bash
./build/forgesim
```

---

# Clean Build

If a completely fresh build is required:

```bash
rm -rf build

cmake -S . -B build -DBUILD_TESTING=ON

cmake --build build

ctest --test-dir build --output-on-failure
```

---

# Design Principles

ForgeSim follows several C++ and software engineering principles.

## Separation of Concerns

Graph management, computation logic, caching, scheduling, concurrency, metrics, and benchmarking are separated into independent components.

## Runtime Polymorphism

Different computations implement the common `Computation` interface.

## RAII

Resources such as worker threads are managed through object lifetime.

## Const Correctness

Read-only operations use `const` where appropriate.

## STL-Based Design

ForgeSim uses standard C++ containers and concurrency primitives.

## Testability

Core components are independently testable using GoogleTest.

## Dependency-Aware Execution

A computation is executed only when all required dependencies are ready.

---

# Limitations

ForgeSim is currently a prototype rather than a production-grade industrial simulation platform.

Current limitations include:

- Input models are currently constructed programmatically.
- No persistent model file format exists yet.
- Cache persistence across program executions is not implemented.
- Scheduler parallelism currently operates in dependency-ready batches.
- Cache storage is in-memory.
- The current engineering model is intentionally small.
- The benchmark topology is synthetic.
- No distributed execution is implemented.
- No GPU execution is implemented.

These limitations define the current prototype scope rather than indicating missing core functionality.

---

# Future Directions

Potential future extensions include:

- External model file parser
- Persistent cache
- Version-aware cache entries
- More dynamic task scheduling
- Additional engineering computation types
- Dependency graph visualization
- Distributed execution
- GPU acceleration
- Source-code dependency analysis adapter

The source-code dependency analysis concept is a possible future application of the same dependency-graph engine and is not part of the current implementation.

---

# What This Project Demonstrates

ForgeSim demonstrates practical understanding of several C++ and systems concepts:

```text
C++20
  │
  ├── Object-Oriented Design
  ├── Runtime Polymorphism
  ├── RAII
  ├── Const Correctness
  ├── STL
  │
  ├── Graph Algorithms
  │     ├── DAG
  │     ├── BFS
  │     ├── Topological Sort
  │     └── Cycle Detection
  │
  ├── Concurrency
  │     ├── std::thread
  │     ├── mutex
  │     ├── condition_variable
  │     ├── future
  │     └── packaged_task
  │
  ├── Caching
  ├── Incremental Computation
  ├── Task Scheduling
  ├── Automated Testing
  └── Performance Benchmarking
```

---

# Interview Summary

A concise description of ForgeSim:

> **ForgeSim is a C++20 incremental parallel engineering simulation engine. I model computations as a dependency DAG, use reverse dependency analysis to identify affected nodes, cache computed results, and use a custom thread pool to execute independent computations concurrently. The engine supports sequential, parallel, and incremental execution and includes automated testing and performance benchmarking.**

### Core engineering idea

```text
Change one thing
      ↓
Find affected computations
      ↓
Invalidate only affected cache
      ↓
Execute only necessary work
      ↓
Parallelize independent computations
```

---

# Author

**Gaurav Mane**

Computer Science & Engineering  
Walchand College of Engineering, Sangli

---

## License

This project is intended as a personal engineering and learning project.
