*This project has been created as part of the 42 curriculum by mromao-s.*

# Codexion

## Description

Codexion is a concurrency simulation inspired by the Dining Philosophers problem. Coders sit in a circle around a shared Quantum Compiler. There is one USB dongle between each pair of neighbours, and a coder needs both of their dongles to compile.

Each coder cycles through three phases: **compiling** (holds two dongles), **debugging** and **refactoring**. A coder who does not start compiling within `time_to_burnout` milliseconds of their last compile start (or of the simulation start) burns out, and the simulation stops. It also stops once every coder has compiled at least `number_of_compiles_required` times.

Each dongle decides who gets it next with one of two policies:
- **fifo**: the coder whose request arrived first.
- **edf**: the coder with the earliest burnout deadline (`last_compile_start + time_to_burnout`).

## Instructions

### Compilation

```bash
make        # builds ./codexion
make clean  # removes object files
make fclean # removes object files and the binary
make re     # rebuilds from scratch
```

### Execution

```
./codexion number_of_coders time_to_burnout time_to_compile time_to_debug time_to_refactor number_of_compiles_required dongle_cooldown scheduler
```

| Argument | Type | Description |
|---|---|---|
| `number_of_coders` | integer ≥ 1 | Number of coders (and dongles) |
| `time_to_burnout` | integer ms ≥ 0 | Max time between compile starts before burnout |
| `time_to_compile` | integer ms ≥ 0 | Duration of the compile phase (holds both dongles) |
| `time_to_debug` | integer ms ≥ 0 | Duration of the debug phase |
| `time_to_refactor` | integer ms ≥ 0 | Duration of the refactor phase |
| `number_of_compiles_required` | integer ≥ 0 | Compiles each coder must finish for a clean stop |
| `dongle_cooldown` | integer ms ≥ 0 | How long a dongle is unavailable after release |
| `scheduler` | `fifo` or `edf` | Arbitration policy |

All arguments are mandatory. Negative numbers, non-integers, values above `INT_MAX` and any scheduler other than `fifo` or `edf` are rejected with an error on stderr and exit status 1.

### Example

```bash
./codexion 4 800 200 200 200 5 0 fifo
```

```
0 1 has taken a dongle
0 1 has taken a dongle
0 1 is compiling
0 3 has taken a dongle
0 3 has taken a dongle
0 3 is compiling
200 1 is debugging
200 2 has taken a dongle
...
```

Each line is a timestamp in milliseconds since the start, the coder number and the new state.

## Blocking cases handled

### Deadlock prevention
A coder takes its two dongles one after the other, so hold-and-wait exists. Deadlock is prevented by breaking **circular wait** (one of Coffman's four conditions): odd-numbered coders take their right dongle first, even-numbered coders take their left dongle first. Two neighbours therefore compete for the same first dongle, and the one that loses waits while holding nothing. A full cycle of coders each holding one dongle and waiting for the next cannot form.

### Starvation prevention
Each dongle keeps its own priority queue of pending requests and is only granted to the request at the top. Under `fifo` the key is the arrival order on that dongle, so a coder that has just compiled queues behind a neighbour who was already waiting. Under `edf` the key is the requester's burnout deadline, with the coder number as a deterministic tie-breaker. Even-numbered coders also start 1 ms late, so that at the start odd coders compile first and even coders second instead of neighbours blocking each other.

### Cooldown handling
When a dongle is released it records `free_at = now + dongle_cooldown`. It cannot be taken before that time. The coder at the top of the queue waits with `pthread_cond_timedwait` until `free_at`, so no extra thread is needed and the wait ends exactly when the cooldown does.

### Precise burnout detection
A separate monitor thread checks every coder about twice per millisecond. If `now - last_compile_start >= time_to_burnout` it prints the burnout line and stops the simulation, well inside the 10 ms limit.

### Log serialization
Every log line is printed by `log_state` while holding the `print` mutex. The stop flag is protected by that same mutex and is checked before printing, so lines never interleave and nothing is printed after `burned out`.

### Clean shutdown
All waits are interruptible: sleeps are done in 0.5 ms slices that check the stop flag, and stopping broadcasts on every dongle's condition variable. A coder that is stopped while holding its first dongle releases it. Threads are all joined and all memory is freed.

## Thread synchronization mechanisms

### `pthread_mutex_t`
- **`dongle->mutex`** (one per dongle) protects that dongle's state: `is_free`, `free_at`, the FIFO counter and its request queue. Checking that a dongle is free and marking it taken happen under this lock, so two coders can never take the same dongle.
- **`props->print`** protects standard output and the `dead` stop flag.
- **`props->state`** protects each coder's `last_compile_start` and `nb_comp`, which are written by the coder and read by the monitor.

Locks are only nested in one order (a dongle mutex, then `print`), so the locks themselves cannot deadlock.

### `pthread_cond_t`
Each dongle has a condition variable paired with its mutex. A coder that cannot take the dongle waits on it with `pthread_cond_wait`, or with `pthread_cond_timedwait` when the only thing missing is the end of the cooldown. Releasing a dongle and stopping the simulation both call `pthread_cond_broadcast`, and every waiter re-checks its condition in a loop.

### Priority queue (heap)
Each dongle owns a binary min-heap of requests (`heap.c`: `heap_push`, `heap_remove`, `sift_up`, `sift_down`). A request stores the coder number and its key, computed when the request is made. The dongle is granted only to the request at the root. Since a dongle sits between two coders, its heap never holds more than two requests.

### Monitor and stop communication
The monitor reads coder state under `state`. To stop, it sets `dead` under `print`, then locks each dongle mutex and broadcasts. A waiting coder holds the dongle mutex from the moment it checks `dead` until it sleeps on the condition variable, so the broadcast cannot be missed.

### Race conditions prevented
- **Dongle duplication**: check and take are one critical section under the dongle mutex.
- **Stale deadline**: `last_compile_start` is written under `state` before the compile is logged, and the monitor reads it under the same lock.
- **Log after burnout**: the burnout line and the `dead` flag are set in one critical section under `print`.
- **Start**: the start time and every coder's first deadline are set before any thread is created.

## Resources

### Concurrency and synchronization
- *The Art of Multiprocessor Programming*, Herlihy & Shavit (deadlock, starvation, fairness)
- POSIX Threads Programming, Blaise Barney, Lawrence Livermore National Laboratory
- `man pthread_mutex_init`, `man pthread_cond_wait`, `man pthread_cond_timedwait`
- *Operating System Concepts*, Silberschatz (synchronization and deadlock chapters)

### Scheduling
- *Real-Time Systems*, Jane Liu (Earliest Deadline First scheduling)
- Wikipedia: [Earliest deadline first scheduling](https://en.wikipedia.org/wiki/Earliest_deadline_first_scheduling)

### AI usage
Claude was used during this project for the following:
- Reviewing the project against the subject and finding bugs (argument validation, a data race on the stop flag, burnouts caused by the first arbitration design).
- Reworking dongle arbitration into the per-dongle design described above (dongle mutex, condition variable and request heap) and the interruptible sleep and logging helpers.
- Explaining monitor threads, condition variables and the difference between `pthread_cond_wait` and `pthread_cond_timedwait`.
- Writing test scripts (invalid inputs, repeated feasible runs, valgrind and helgrind).

All AI-generated content was reviewed, understood and validated before inclusion.
