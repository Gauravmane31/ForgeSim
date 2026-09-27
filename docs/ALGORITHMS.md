# ForgeSim Algorithms

## 1. Graph Representation

ForgeSim represents computations as a directed graph.

For an edge:

```text
A → B
```

B depends on A.

Each node stores:

```text
dependencies
dependents
```

This provides both forward and reverse traversal.

---

# 2. Duplicate Edge Prevention

When adding dependencies, ForgeSim checks whether the dependency already exists before inserting it.

This prevents:

```text
A → B
A → B
A → B
```

from becoming multiple duplicate edges.

The same principle is used for dependent relationships.

---

# 3. Cycle Detection

ForgeSim uses Kahn's algorithm.

### Idea

For every node, calculate its indegree:

```text
indegree(node) =
number of incoming dependency edges
```

Nodes with zero indegree can be processed first.

Example:

```text
A → C
B → C
C → D
```

Initial indegrees:

```text
A = 0
B = 0
C = 2
D = 1
```

A and B are initially ready.

After processing them:

```text
C = 0
```

C becomes ready.

Then D becomes ready.

If the algorithm cannot process all nodes, the graph contains a cycle.

### Complexity

```text
Time  : O(V + E)
Space : O(V)
```

---

# 4. Topological Sorting

The same Kahn-style approach produces a topological ordering.

A valid result satisfies:

```text
Every dependency appears before the node that depends on it.
```

For:

```text
A → C
B → C
C → D
```

a valid order is:

```text
A, B, C, D
```

The relative order of independent nodes is not semantically important.

---

# 5. Impact Analysis

When a node changes, ForgeSim needs to identify downstream computations.

Example:

```text
A → B → C → D
```

If A changes:

```text
Affected = {A, B, C, D}
```

ForgeSim traverses:

```text
dependents
```

rather than:

```text
dependencies
```

because the required question is:

> Which nodes depend on the changed node?

---

# 6. BFS-Style Traversal

The impact analyzer uses a queue.

Conceptually:

```text
queue = changed nodes
affected = empty set

while queue not empty:

    node = queue.front()
    remove node

    for each dependent:

        if dependent not already affected:
            add dependent
            push dependent into queue
```

A set prevents duplicate processing.

### Complexity

For the reachable affected subgraph:

```text
O(V + E)
```

in the worst case.

---

# 7. Diamond Dependencies

Consider:

```text
       B
      ↗
A ───► C ───► D
      ↘
       E
```

Or more explicitly:

```text
       B ───┐
      ↗     │
A ───►      ├──► D
      ↘     │
       C ───┘
```

D may be reachable through multiple paths.

Without a visited set, D could be processed multiple times.

ForgeSim uses an `unordered_set<int>` to guarantee that each node is added to the affected set once.

---

# 8. Incremental Scheduling

After impact analysis, ForgeSim has the affected subgraph.

For each affected node, it determines how many of its dependencies are also affected and still incomplete.

Conceptually:

```text
remainingDependencies[node]
```

A node is ready when:

```text
remainingDependencies[node] == 0
```

---

# 9. Dependency Readiness

Suppose:

```text
A → C
B → C
```

If both A and B are affected:

```text
C remaining dependencies = 2
```

After A completes:

```text
C remaining dependencies = 1
```

After B completes:

```text
C remaining dependencies = 0
```

C can now execute.

This allows dependency constraints to be preserved while still exposing parallel work.

---

# 10. Incremental Execution Flow

```text
Changed Nodes
      ↓
Impact Analysis
      ↓
Affected Set
      ↓
Invalidate Affected Cache
      ↓
Calculate Remaining Dependencies
      ↓
Find Ready Nodes
      ↓
Submit Ready Tasks
      ↓
Task Completes
      ↓
Release Dependents
      ↓
Submit Newly Ready Tasks
```

The current implementation processes ready work in dependency-aware batches.

---

# 11. Sequential Execution Complexity

Graph analysis is:

```text
O(V + E)
```

Actual computation cost depends on the computation implementation.

Therefore:

```text
Total Cost =
Graph Processing
+
Computation Cost
```

---

# 12. Parallel Execution

Parallel execution does not change dependency correctness.

It changes only when independent computations are executed.

Example:

```text
        ┌── B ──┐
A ──────┼── C ──┼──► D
        └── E ──┘
```

After A:

```text
B, C, E
```

can potentially execute concurrently.

D must wait for all required inputs.

---

# 13. Cache Complexity

The cache uses an unordered map.

Average lookup:

```text
O(1)
```

Average insertion:

```text
O(1)
```

The cache is currently in-memory and local to the scheduler instance.

---

# 14. Overall Incremental Complexity

The graph-related portion is approximately:

```text
Impact Analysis     O(V + E)
Dependency Analysis O(V + E)
Cache Operations    O(V) average for affected/checked nodes
```

The actual runtime is dominated by the cost of the computations that must be executed.

The primary optimization is therefore not changing the asymptotic complexity of the computation itself, but reducing how many computations need to run.
