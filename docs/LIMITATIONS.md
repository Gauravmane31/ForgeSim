# ForgeSim Limitations and Future Work

## 1. Purpose of This Document

ForgeSim is a prototype demonstrating an incremental parallel computation architecture.

It is not intended to claim the completeness of a production industrial simulation platform.

The limitations below define the current scope.

---

# 2. Programmatic Model Construction

The current demonstration constructs the computation graph directly in C++.

There is currently no external model description format.

A future implementation could load models from a structured file format.

---

# 3. In-Memory Cache

The cache exists only during the current process.

If ForgeSim exits:

```text
Cached results
     ↓
lost
```

Persistent caching could be introduced later.

---

# 4. No Cross-Process Cache

The current cache belongs to a running scheduler instance.

It is not shared between processes or machines.

---

# 5. Synthetic Benchmark

The benchmark graph is intentionally designed to expose parallel work.

It should not be interpreted as representative of every engineering simulation topology.

Real engineering models may have:

- Long dependency chains
- Uneven task costs
- Different branching factors
- Strong coupling
- Very small tasks
- Very large tasks

Performance can therefore differ significantly.

---

# 6. Batch-Oriented Parallel Scheduling

The current scheduler uses dependency-ready batches.

This is simpler to reason about and provides clear dependency boundaries.

However, a more advanced scheduler could use:

- Dynamic task submission
- Work stealing
- Priority scheduling
- Event-driven readiness

These are potential future extensions.

---

# 7. Limited Engineering Model

The current demonstration contains a small set of computation types:

```text
Constant
Multiply
Divide
Work
```

The architecture is designed to support additional computation implementations.

---

# 8. No External Visualization

The dependency graph is currently represented internally.

There is no graphical dependency visualization.

A future UI could display:

```text
Nodes
Dependencies
Affected Nodes
Execution State
Cache State
```

---

# 9. No Distributed Execution

ForgeSim currently executes in a single process on a single machine.

There is no:

```text
RPC
Distributed Scheduler
Cluster Execution
Remote Worker
```

support.

---

# 10. No GPU Execution

The current engine uses CPU-based C++ execution.

GPU acceleration is not implemented.

---

# 11. No Persistent Model Format

There is no standardized representation for storing and loading complete simulation models.

The current example constructs its model directly in the program.

---

# 12. Current Cache Semantics

Cache validity is currently coordinated by the scheduler and graph change tracking.

The caller is expected to mark a graph node as changed after modifying a computation.

For example:

```text
Change computation
       ↓
graph.markChanged(nodeId)
       ↓
incremental execution
```

A future version could make cache validity more explicitly version-aware.

---

# 13. Future Directions

Possible future extensions include:

- External model parser
- Persistent cache
- Version-aware cache entries
- Dynamic task scheduling
- Work-stealing scheduler
- More engineering computations
- Graph visualization
- Distributed execution
- GPU acceleration
- More advanced metrics
- Source-code dependency analysis adapter

These are possible future directions, not requirements for the current prototype.

---

# 14. Scope Boundary

The current project is intentionally focused on demonstrating:

```text
Dependency Graph
       +
Impact Analysis
       +
Caching
       +
Incremental Computation
       +
Parallel Scheduling
       +
C++ Concurrency
```

The absence of distributed execution, GPU execution, persistent storage, or a graphical interface does not affect the core demonstration of these concepts.

---

# 15. Future Source-Code Adapter

The dependency graph architecture could potentially be reused for source-code dependency analysis.

For example:

```text
Header A
   ↓
Source B
   ↓
Module C
```

A future adapter could determine which source files are affected by a change.

This is a possible future application of the graph engine and is **not part of the current ForgeSim implementation**.
