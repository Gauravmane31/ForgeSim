# ForgeSim Architecture

## 1. Overview

ForgeSim is organized into two major layers:

```text
Core Layer
    ↓
Execution Layer
```

The Core Layer represents the computational model.

The Execution Layer determines how that model is evaluated.

The main architectural goal is to keep dependency representation, computation logic, caching, scheduling, and concurrency independently testable.

---

## 2. High-Level Architecture

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
      │ Impact Analyzer │       │ Graph Analysis  │
      └────────┬────────┘       └────────┬────────┘
               │                         │
               └────────────┬────────────┘
                            ▼
                    ┌───────────────┐
                    │ Cache Manager │
                    └───────┬───────┘
                            ▼
                    ┌───────────────┐
                    │   Scheduler   │
                    └───────┬───────┘
                            │
                            ▼
                    ┌───────────────┐
                    │   ThreadPool  │
                    └───────┬───────┘
                            ▼
                    ┌───────────────┐
                    │ Computations  │
                    └───────┬───────┘
                            ▼
                    ┌───────────────┐
                    │    Results    │
                    └───────────────┘
```

---

## 3. Core Layer

### Node

`Node` represents one computation entity in the graph.

Responsibilities:

- Store node ID
- Store node name
- Store dependencies
- Store dependents
- Track execution state
- Track version

A node stores both incoming and outgoing relationships because both are required by different parts of the engine.

---

### Graph

`Graph` owns the collection of nodes and manages dependency relationships.

Responsibilities:

- Add nodes
- Look up nodes
- Add dependencies
- Detect cycles
- Generate topological ordering
- Mark nodes as changed
- Identify dirty nodes

The graph uses:

```cpp
std::unordered_map<int, Node>
```

for node lookup by ID.

---

### Result

`Result` represents the output of a computation.

It contains:

```text
value
valid
```

A valid numerical result of `0.0` is therefore distinguishable from an invalid result.

---

### Computation

`Computation` is the abstract interface implemented by concrete computations.

```cpp
virtual Result calculate(
    const std::vector<Result>& inputs
) const = 0;
```

Current implementations include:

- `ConstantComputation`
- `MultiplyComputation`
- `DivideComputation`
- `WorkComputation`

The scheduler works with the interface rather than depending on individual mathematical implementations.

---

### ImpactAnalyzer

`ImpactAnalyzer` determines which nodes are downstream of changed nodes.

It uses the reverse dependency relationship:

```text
Changed Node
     ↓
Dependents
     ↓
More Dependents
     ↓
Affected Subgraph
```

A set is used to avoid processing the same node multiple times.

---

## 4. Execution Layer

### Scheduler

`Scheduler` is the primary execution orchestrator.

It coordinates:

```text
Graph
Computation
Impact Analysis
Cache
Thread Pool
Metrics
```

It provides three execution strategies:

```cpp
execute()
executeParallel()
executeIncremental()
```

---

### ThreadPool

`ThreadPool` manages reusable worker threads.

It contains:

- Worker threads
- Task queue
- Mutex
- Condition variable
- Shutdown state

The scheduler submits executable tasks to the pool.

---

### CacheManager

`CacheManager` stores valid results for completed computations.

Responsibilities:

```text
store
contains
get
invalidate
clear
```

The cache is currently in-memory.

---

### ExecutionMetrics

Tracks:

- Total nodes
- Computed nodes
- Cached nodes
- Cache hit rate

This makes the effect of incremental execution measurable.

---

### Benchmark

`Benchmark` provides timing support for performance experiments.

It uses C++ `<chrono>` facilities.

---

## 5. Data Flow

### Full Execution

```text
Graph
  ↓
Topological Ordering
  ↓
Scheduler
  ↓
Computation
  ↓
Result
  ↓
Cache
```

### Incremental Execution

```text
Changed Node
     ↓
Impact Analyzer
     ↓
Affected Nodes
     ↓
Invalidate Affected Cache
     ↓
Dependency Readiness
     ↓
Scheduler
     ↓
Thread Pool
     ↓
Computation
     ↓
Cache
```

---

## 6. Dependency Direction

For:

```text
A → B
```

B depends on A.

ForgeSim stores:

```text
B.dependencies = {A}
A.dependents   = {B}
```

This is intentional.

The dependency list answers:

> What must be completed before this node can execute?

The dependent list answers:

> Which nodes become potentially affected if this node changes?

---

## 7. Component Responsibilities

| Component | Responsibility |
|---|---|
| `Node` | Node state and dependency metadata |
| `Graph` | Graph structure and graph algorithms |
| `Result` | Computation output and validity |
| `Computation` | Computation interface |
| `ImpactAnalyzer` | Downstream change propagation |
| `CacheManager` | Result caching |
| `Scheduler` | Execution orchestration |
| `ThreadPool` | Worker management |
| `ExecutionMetrics` | Execution statistics |
| `Benchmark` | Timing measurements |

---

## 8. Architectural Boundary

The scheduler does not need to know how a computation is implemented.

For example:

```text
                 Computation
                      │
          ┌───────────┼───────────┐
          ▼           ▼           ▼
      Constant     Multiply     Divide
```

This allows new computation types to be added without changing the scheduler interface.
