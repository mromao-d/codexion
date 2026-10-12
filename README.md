*This project has been created as part of the 42 curriculum by mromao-s.*

# Codexion

## Description

Codexion is a concurrency simulation inspired by the classic Dining Philosophers problem, reimagined in a software development context. A number of coders sit around a shared Quantum Compiler, each requiring two USB dongles simultaneously to compile their quantum code. Because there are only as many dongles as coders, and each dongle is shared between two adjacent coders, they must coordinate access carefully to avoid both deadlock and burnout.

Each coder cycles through three phases: **compiling** (holds two dongles), **debugging**, and **refactoring**. If a coder fails to start compiling within `time_to_burnout` milliseconds of their last compile (or the simulation start), they burn out and the simulation stops. The simulation also ends cleanly when every coder has compiled at least `number_of_compiles_required` times.

The program supports two scheduling policies for dongle arbitration:
- **fifo** — First In, First Out: dongles are granted in request-arrival order.
- **edf** — Earliest Deadline First: the coder whose burnout deadline is closest gets priority.

A dedicated monitor thread tracks burnout deadlines and stops the simulation within 10 ms of any actual burnout event.

## Instructions

### Compilation

```bash
make
```

This produces the `codexion` binary. To clean object files:

```bash
make clean
```

To remove the binary as well:

```bash
make fclean
```

To rebuild from scratch:

```bash
make re
```

### Execution

```
./codexion number_of_coders time_to_burnout time_to_compile time_to_debug time_to_refactor number_of_compiles_required dongle_cooldown scheduler
```

| Argument | Type | Description |
|---|---|---|
| `number_of_coders` | integer ≥ 1 | Number of coders (and dongles) |
| `time_to_burnout` | integer ms | Max time between compile starts before burnout |
| `time_to_compile` | integer ms | Duration of the compile phase (holds both dongles) |
| `time_to_debug` | integer ms | Duration of the debug phase |
| `time_to_refactor` | integer ms | Duration of the refactor phase |
| `number_of_compiles_required` | integer ≥ 1 | Target compile count per coder to end simulation cleanly |
| `dongle_cooldown` | integer ms | How long a dongle is unavailable after being released |
| `scheduler` | `fifo` or `edf` | Arbitration policy |

All arguments are mandatory. Negative numbers, non-integers, and scheduler values other than `fifo` or `edf` are rejected.

### Example

```bash
./codexion 4 800 200 200 400 5 0 fifo
```

Expected log format:

```
0 1 has taken a dongle
1 1 has taken a dongle
1 1 is compiling
201 1 is debugging
401 1 is refactoring
...
1505 4 burned out
```

Each line starts with a timestamp in milliseconds relative to the simulation start, followed by the coder number and their current action.

## Blocking Cases Handled

### Deadlock Prevention
A deadlock occurs when every coder holds one dongle and waits for the other indefinitely (Coffman's conditions: mutual exclusion, hold-and-wait, no preemption, circular wait). This is prevented structurally: a coder only picks up both dongles atomically, under one lock, once both are free. No coder ever holds one dongle while waiting for the other, breaking the hold-and-wait condition regardless of scheduling policy.

### Starvation Prevention and concurrent compiling
Arbitration is **per-dongle**, not global. A dongle is physically shared only between two adjacent coders (coder `i` and coder `i+1`), so a coder only needs to defer to its two actual neighbors, not to every other coder in the simulation. Concretely, a coder may take its dongles as soon as both are free, unless one of its two neighbors is also currently waiting **and** has higher scheduling priority for that shared dongle (`fifo`: earlier arrival; `edf`: earlier burnout deadline, with coder ID as a deterministic tie-break). This means non-adjacent coders — e.g. coder 1 and coder 3 in a 4-coder run — can compile at the same time, since they share no dongle and never contend with each other.

Under `fifo`, both arbitration keys (arrival order for `fifo`, deadline for `edf`) are tracked through the same priority-queue mechanism, so a coder that just compiled is re-queued with a fresh, later priority key next time it waits, and naturally yields to coders who have been waiting longer. The burnout check in the monitor also provides a hard guarantee: if any coder is starved long enough regardless, the simulation detects and reports it.

### Cooldown Handling
After a coder releases both dongles, a dedicated cooldown thread (`drop_dongles`) sleeps for `dongle_cooldown` milliseconds before marking the dongles as free and broadcasting to waiting coders. This ensures the cooldown period is respected before any other coder can acquire the same dongle.

### Precise Burnout Detection
A separate monitor thread polls all coders every 1 ms. For each coder it computes `deadline = last_compile_start + time_to_burnout`. If the current time exceeds that deadline, the monitor immediately prints the burnout message and sets the simulation's `dead` flag, stopping all threads within the 10 ms precision requirement.

### Log Serialization
All log output is protected by a dedicated `print` mutex. Any thread that needs to print locks this mutex, writes its complete message, then unlocks it. This guarantees that log lines from different threads never interleave on the same output line.

## Thread Synchronization Mechanisms

### `pthread_mutex_t` usage

Three mutexes are used:

- **`props->scheduler_mutex`** — the central lock protecting all shared mutable state: the wait queue, each dongle's `is_free` flag, the `dead` flag, and coders' `last_compile_start` timestamps. Any thread reading or writing these values must hold this mutex.
- **`props->print`** — protects all calls to `printf`/`write` so that log lines are never interleaved.
- **`dongle->dongle`** (one per dongle) — declared and initialized per dongle as required infrastructure. Mutual exclusion over dongle state is centralized in `scheduler_mutex` rather than split across per-dongle locks: see "Why dongle state is guarded by one shared mutex" below.

#### Why dongle state is guarded by one shared mutex

The subject requires protecting each dongle's state with a `pthread_mutex_t` to prevent coders from duplicating dongles — i.e., two coders must never both see a dongle as free and both take it. This is a check-then-act race: the read of `is_free` and the write that claims it must be atomic with respect to every other coder.

Every place `is_free` is read or written (`take_dongles`, `drop_dongles`) happens while the calling thread already holds `scheduler_mutex`. No two threads can ever be inside those sections concurrently, so the "check both dongles are free, then take both" sequence is atomic — which is exactly what prevents duplication.

A single shared mutex is a deliberate choice, not a shortcut. Compiling requires acquiring **two** dongles atomically: a coder must never hold one while blocked waiting for the other, since that is the hold-and-wait condition that causes deadlock (see Coffman's conditions below). If each dongle had an independent mutex, a coder would need to lock dongle A, then lock dongle B — and if two coders acquire their shared dongles in opposite order, the result is the classic ABBA lock-order deadlock. Guarding the whole table with one mutex makes "check both, take both, or wait" a single atomic transaction, eliminating that deadlock class entirely. `dongle->dongle` is still initialized and destroyed per dongle as declared per-resource infrastructure, even though mutual exclusion is centralized for correctness.

### `pthread_cond_t` usage

A single condition variable `props->scheduler_cond` is paired with `scheduler_mutex`. Coders waiting for their turn call `pthread_cond_wait`, atomically releasing `scheduler_mutex` and sleeping. Any event that might unblock a coder — a dongle becoming free after cooldown, a rival neighbor finishing its wait, or the simulation ending — triggers `pthread_cond_broadcast`, waking all waiting coders to independently re-evaluate their own dongle/rival condition.

### Monitor thread

A dedicated monitor thread runs alongside the coder threads from simulation start. It loops with a 1 ms sleep, locks `scheduler_mutex`, then checks:
1. Whether all coders have reached `number_of_compiles_required` (clean stop).
2. Whether any coder's `last_compile_start + time_to_burnout` has passed (burnout stop).

If either condition is true, it sets `dead = 1` and broadcasts on `scheduler_cond`. All sleeping coder threads wake up, see `dead == 1`, and exit cleanly.

### Race condition prevention

- **Start race**: coder threads are created before `start_time` is set. Each coder waits on `scheduler_cond` for `props->start == 1` before doing anything, so no coder reads `start_time` before the main thread has written it.
- **Deadline reset race**: `last_compile_start` is written inside `scheduler_mutex` at the moment both dongles are taken, before printing any log. The monitor always reads it under the same lock, so it never observes a stale deadline.
- **Cooldown-to-free race**: `drop_dongles` sets `is_free = 1` and broadcasts while holding `scheduler_mutex`, so no coder can observe a dongle as free before the cooldown has actually elapsed.

### Custom priority queue (heap)

The scheduler queue is a min-heap of coder IDs stored in `props->queue[0..queue_size-1]`, used uniformly for both scheduling policies via a single comparator (`has_priority`): for `edf`, the key is each coder's deadline (`last_compile_start + time_to_burnout`, tie-broken by coder ID); for `fifo`, the key is a monotonically increasing arrival sequence number assigned when the coder is enqueued. Insertion uses `sift_up`. Removal supports removing *any* coder, not just the heap root — a coder's last element is swapped into the removed slot, then both `sift_down` and `sift_up` are tried from that position (only one ever does work), since arbitration is now resolved per-neighbor rather than strictly front-of-queue.

The queue membership and priority ordering are also used directly for arbitration: `is_blocked_by_rival` checks whether a coder's left or right neighbor (the only two coders who could ever contend for either of its dongles) is currently queued with higher priority. This is what allows non-conflicting coders to compile concurrently instead of being serialized behind an unrelated coder's turn.

## Resources

### Concurrency and synchronization
- *The Art of Multiprocessor Programming* — Herlihy & Shavit (deadlock, starvation, fairness)
- POSIX Threads Programming — Blaise Barney, Lawrence Livermore National Laboratory
- `man pthread_mutex_init`, `man pthread_cond_wait`, `man pthread_cond_timedwait`
- *Operating System Concepts* (Silberschatz) — Chapter on synchronization and deadlock

### Scheduling
- *Real-Time Systems* — Jane Liu — Earliest Deadline First scheduling theory
- Wikipedia: [Earliest deadline first scheduling](https://en.wikipedia.org/wiki/Earliest_deadline_first_scheduling)

### AI usage
Claude (claude.ai) was used during this project for the following:
- Identifying logic bugs in the initial implementation (ABBA deadlock in dongle acquisition, incorrect coder ID initialization, wrong scheduler string comparison).
- Explaining the correct approach for implementing a separate monitor thread and how it interacts with condition variables.
- Clarifying the difference between `pthread_cond_wait` and `pthread_cond_timedwait` in the context of externally-triggered stop conditions.
- Generating the initial structure of the EDF min-heap (`sift_up`/`sift_down`) which was then reviewed, tested, and integrated manually.

All AI-generated content was reviewed, understood, and validated before inclusion.
