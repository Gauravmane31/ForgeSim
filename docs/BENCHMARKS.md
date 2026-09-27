# ForgeSim Benchmarks

## 1. Purpose

The benchmark evaluates four execution scenarios:

1. Sequential full execution
2. Parallel full execution
3. Incremental execution after a change
4. No-change execution using the cache

The objective is to demonstrate:

```text
Parallelism
+
Incremental recomputation
+
Cache reuse
```

---

# 2. Benchmark Configuration

```text
Nodes         : 1000
Workers       : 8
Work / Node   : 50000 iterations
```

The graph is synthetic and intentionally exposes substantial independent work for the parallel execution experiment.

---

# 3. Sequential Full Execution

```text
Time      : 1801.609 ms
Computed  : 1000
```

Every node is computed.

---

# 4. Parallel Full Execution

```text
Time      : 274.876 ms
Computed  : 1000
```

Measured speedup:

```text
1801.609 / 274.876 ≈ 6.55x
```

This demonstrates the effect of executing independent computations concurrently for this particular workload.

---

# 5. Initial Incremental Execution

```text
Time      : 278.299 ms
Computed  : 1000
Cached    : 0
```

The first incremental execution behaves like a full computation because no previous results exist in the cache.

---

# 6. Incremental Execution After Change

Changed node:

```text
500
```

Results:

```text
Time         : 6.961 ms
Total Nodes  : 1000
Computed     : 2
Cached       : 998
Cache Hit    : 99.80%
```

This demonstrates the main optimization of ForgeSim.

Instead of recomputing all 1000 nodes, only two nodes are recomputed for the selected benchmark topology and changed node.

---

# 7. No-Change Execution

```text
Time         : 0.493 ms
Computed     : 0
Cached       : 1000
Cache Hit    : 100.00%
```

No computation is required because the graph has not changed and all required results are already cached.

---

# 8. Summary

| Execution Mode | Time | Computed | Cached |
|---|---:|---:|---:|
| Sequential Full | 1801.609 ms | 1000 | 0 |
| Parallel Full | 274.876 ms | 1000 | 0 |
| Incremental After Change | 6.961 ms | 2 | 998 |
| No Change | 0.493 ms | 0 | 1000 |

---

# 9. Cache Hit Calculation

After changing node 500:

```text
Cache Hit Rate
= Cached Nodes / Total Nodes × 100

= 998 / 1000 × 100

= 99.80%
```

For the no-change execution:

```text
= 1000 / 1000 × 100
= 100%
```

---

# 10. Important Benchmark Qualification

The benchmark results depend on:

- CPU
- Number of worker threads
- Compiler
- Optimization settings
- Operating system
- Graph topology
- Computation workload
- Runtime conditions

The results should therefore be interpreted as measurements of the tested configuration rather than universal performance guarantees.

---

# 11. Parallel Benchmark Observation

An earlier benchmark with a more dependency-heavy graph produced:

```text
Sequential : 851.942 ms
Parallel   : 1124.630 ms
Speedup    : 0.76x
```

This is an important engineering observation.

Parallel execution has overhead:

```text
Task creation
Queue synchronization
Thread scheduling
Context switching
Future/result handling
```

If the graph does not contain enough independent work, this overhead can outweigh the benefit of parallel execution.

Therefore:

> Parallelism is useful when the workload exposes enough independent computation to amortize scheduling overhead.

---

# 12. Benchmark Interpretation

The benchmark demonstrates two separate optimization mechanisms.

### Parallel execution

Reduces elapsed time by executing independent work concurrently.

### Incremental execution

Reduces the amount of work by avoiding recomputation of unaffected nodes.

These optimizations address different bottlenecks:

```text
Parallelism
    → How quickly can necessary work execute?

Incremental computation
    → How much work is actually necessary?
```

This distinction is central to ForgeSim's design.
