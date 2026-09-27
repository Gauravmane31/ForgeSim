# ForgeSim Concurrency Model

## 1. Overview

ForgeSim uses a custom thread pool to execute independent computations concurrently.

The concurrency model is based entirely on C++ standard-library facilities.

```text
Scheduler
    │
    ▼
Task Queue
    │
    ├──────────────┬──────────────┐
    ▼              ▼              ▼
Worker 1       Worker 2       Worker 3
    │              │              │
    └──────────────┴──────────────┘
                   │
                   ▼
                Results
```

---

# 2. Thread Pool Components

The thread pool contains:

```cpp
std::vector<std::thread> workers;
std::queue<std::function<void()>> tasks;
std::mutex queueMutex;
std::condition_variable condition;
bool stop;
```

Each component has a specific role.

| Component | Purpose |
|---|---|
| `std::thread` | Worker execution |
| `std::queue` | Pending tasks |
| `std::mutex` | Protect task queue |
| `condition_variable` | Sleep/wake workers |
| `std::function` | Generic task wrapper |
| `future` | Retrieve asynchronous results |
| `packaged_task` | Associate callable with future |

---

# 3. Worker Loop

Conceptually, each worker performs:

```text
while true:

    wait until:
        task available
        OR shutdown requested

    if shutdown requested
       and no tasks remain:
        exit

    retrieve task

    execute task
```

Workers do not continuously spin when no work is available.

They sleep using the condition variable.

---

# 4. Task Submission

When the scheduler submits a computation:

```text
Scheduler
    ↓
ThreadPool::submit()
    ↓
Create packaged task
    ↓
Create future
    ↓
Push task into queue
    ↓
notify_one()
    ↓
Worker wakes
```

The returned `future` allows the caller to observe completion and retrieve the result.

---

# 5. Mutex Protection

The task queue is shared between:

```text
Producer
    ↓
Scheduler

Consumers
    ↓
Worker Threads
```

Therefore queue access must be synchronized.

ForgeSim protects queue access using:

```cpp
std::mutex
```

A lock is held only around the queue operation rather than during task execution.

This is important because workers should not block one another while executing expensive computations.

---

# 6. Condition Variable

Workers use:

```cpp
std::condition_variable
```

instead of repeatedly polling the queue.

Without a condition variable, workers could repeatedly check:

```text
Is there work?
No.
Is there work?
No.
Is there work?
No.
```

This wastes CPU time.

With a condition variable:

```text
No work
   ↓
Worker sleeps
   ↓
Task submitted
   ↓
Worker notified
   ↓
Worker wakes
```

---

# 7. Futures and Packaged Tasks

ForgeSim uses:

```cpp
std::packaged_task
```

to wrap a callable.

The corresponding:

```cpp
std::future
```

allows the caller to observe the eventual result.

This separates:

```text
Task submission
```

from:

```text
Task completion
```

---

# 8. Thread Pool Shutdown

The thread pool uses RAII.

When the pool is destroyed:

```text
stop = true
      ↓
notify_all()
      ↓
workers wake
      ↓
workers finish remaining work
      ↓
workers exit
      ↓
join()
```

Joining the threads prevents the program from leaving active worker threads behind.

---

# 9. Exception Propagation

If a computation executed inside a packaged task throws an exception, the exception becomes associated with the future.

When the future is consumed, the exception can be rethrown.

This prevents worker-thread exceptions from silently terminating the process.

---

# 10. Why the Cache Is Not Independently Thread-Safe

`CacheManager` itself does not use a mutex.

This is intentional in the current architecture.

The scheduler coordinates cache mutations around task execution.

Worker tasks primarily perform computation.

Cache updates are performed by the scheduler after successful task completion.

This avoids adding unnecessary locking to every cache operation.

The current design assumes cache access is controlled by the scheduler.

---

# 11. Parallelism and DAG Structure

Concurrency is limited by dependency relationships.

Sequential graph:

```text
A → B → C → D
```

Parallel opportunity:

```text
        ┌── B ──┐
A ──────┼── C ──┼──► D
        └── E ──┘
```

The second graph exposes more independent work.

Therefore:

```text
Available Parallelism
        ↓
Graph Structure
        ↓
Potential Speedup
```

---

# 12. Scheduling Trade-off

The current scheduler uses dependency-ready batches.

This makes correctness and reasoning straightforward.

However, a fully dynamic work-stealing or event-driven scheduler could potentially reduce idle time in more irregular DAGs.

That is a future architectural direction rather than a requirement of the current prototype.

---

# 13. Thread Count

The thread pool accepts a configurable worker count.

The benchmark used:

```text
Workers = 8
```

The optimal worker count depends on:

- CPU core count
- Workload
- Graph parallelism
- Task granularity
- Synchronization overhead

Therefore the benchmark's worker count should be treated as part of the workload configuration.
