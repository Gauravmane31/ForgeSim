# ForgeSim Testing Strategy

## 1. Overview

ForgeSim uses **GoogleTest** for automated testing.

The test suite focuses on externally observable behavior of the core components and execution engine.

The objective is to verify:

```text
Correctness
+
Error Handling
+
Graph Algorithms
+
Caching
+
Incremental Execution
+
Parallel Execution
```

---

# 2. Node Tests

`NodeTest.cpp` verifies:

- Constructor behavior
- Initial state
- Dependency insertion
- Dependent insertion
- Duplicate dependency prevention
- Duplicate dependent prevention
- Version changes
- State transitions
- Dirty-state behavior

Example behaviors:

```text
DIRTY
READY
RUNNING
COMPLETED
FAILED
```

---

# 3. Graph Tests

`GraphTest.cpp` verifies:

- Node insertion
- Duplicate node rejection
- Dependency creation
- Reverse dependency creation
- Acyclic graph detection
- Cyclic graph detection
- Topological ordering
- Dirty node detection
- Duplicate dependency prevention

The tests validate both the graph structure and the graph algorithms.

---

# 4. Computation Tests

`ComputationTest.cpp` verifies:

### Constant

```text
Configured value is returned.
```

### Multiplication

```text
2 × 3 × 4 = 24
```

### Division

```text
20 / 4 = 5
```

### Error handling

Invalid situations are expected to throw exceptions:

```text
Empty multiplication input
Invalid multiplication input
Incorrect division input count
Division by zero
```

---

# 5. Cache Tests

`CacheManagerTest.cpp` verifies:

- Empty cache behavior
- Result storage
- Result retrieval
- Cache invalidation
- Cache clearing
- Cache size

An important distinction is tested:

```text
Cached valid result
```

versus:

```text
Invalid result
```

---

# 6. Impact Analyzer Tests

`ImpactAnalyzerTest.cpp` verifies:

### Downstream propagation

A changed node affects its dependents.

### Unrelated branches

Unconnected branches are not marked as affected.

### Diamond dependency

Nodes reachable through multiple paths are processed without duplicate traversal.

Example:

```text
      B
     ↗
A ──► C ──► D
     ↘
      E
```

---

# 7. Scheduler Tests

`SchedulerTest.cpp` verifies:

### Sequential execution

The scheduler produces correct results.

### Parallel execution

Parallel scheduling produces the same expected results.

### Incremental execution

Changing an input causes only the affected portion to be recomputed.

### No-change execution

When nothing changes, cached results are reused.

---

# 8. Test Philosophy

The tests primarily verify behavior rather than implementation details.

For example:

```cpp
EXPECT_DOUBLE_EQ(
    result.getValue(),
    750.0
);
```

tests the observable result.

This reduces coupling between tests and internal implementation choices.

---

# 9. Running Tests

From the project root:

```bash
cmake --build build
ctest --test-dir build --output-on-failure
```

---

# 10. Clean Test Run

For a clean build:

```bash
rm -rf build

cmake -S . -B build -DBUILD_TESTING=ON

cmake --build build

ctest --test-dir build --output-on-failure
```

---

# 11. Expected Outcome

All registered tests should pass.

A successful test run confirms the current implementation of:

```text
Node
Graph
Computation
CacheManager
ImpactAnalyzer
Scheduler
ThreadPool integration
```

---

# 12. Why Automated Testing Matters

ForgeSim contains several interacting systems:

```text
Graph
   ↓
Impact Analysis
   ↓
Cache
   ↓
Scheduler
   ↓
Thread Pool
```

A change in one component can affect another.

Automated tests provide a regression safety net when modifying these components.
