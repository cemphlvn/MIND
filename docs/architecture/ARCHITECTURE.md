# MIND Architecture

This document describes the internal architecture of MIND.

## Overview

MIND is structured as a minimal, layered system:

```
┌─────────────────────────────────────────────────────────────────┐
│                        Public API (cr.h)                        │
├─────────────────────────────────────────────────────────────────┤
│  Lifecycle │ Experience │ Query │ Temporal │ Persistence       │
├─────────────────────────────────────────────────────────────────┤
│                    Internal Structures                          │
│                    (cr_internal.h)                              │
├─────────────────────────────────────────────────────────────────┤
│                    C11 Standard Library                         │
└─────────────────────────────────────────────────────────────────┘
```

## Components

### Runtime (`cr_runtime_t`)

The runtime holds configuration and provides context for states.

```c
struct cr_runtime {
    int dim;        // Embedding dimension
    int max_slots;  // Maximum memory slots
};
```

Properties:
- Immutable after creation
- Multiple states can share one runtime
- No memory allocation after creation

### State (`cr_state_t`)

A state holds accumulated cognitive memory.

```c
struct cr_state {
    cr_runtime_t* rt;           // Parent runtime

    // Memory
    cr_slot_t* slots;           // Array of invariant slots
    int slot_count;             // Occupied slots

    // Epistemic state
    float plasticity;           // Current malleability
    float age;                  // Experiential time

    // Temporal tracking
    float plasticity_prev;      // For velocity calculation
    float velocity;             // Rate of crystallization
    float last_reinforcement_age;
    int total_updates;
    int total_reinforcements;
};
```

### Memory Slot (`cr_slot_t`)

Each slot holds one compressed invariant.

```c
typedef struct {
    float* vector;  // Invariant vector
    float weight;   // Reinforcement count
} cr_slot_t;
```

## Data Flow

### Update Flow

```
Input: embedding, delta_t
         │
         ▼
┌─────────────────┐
│ Find closest    │◄─── Cosine similarity
│ invariant       │
└────────┬────────┘
         │
    ┌────┴────┐
    │         │
    ▼         ▼
similar?    no match
    │         │
    ▼         ▼
REINFORCE   CREATE
    │         │
    └────┬────┘
         │
         ▼
┌─────────────────┐
│ Update          │
│ plasticity      │
└────────┬────────┘
         │
         ▼
┌─────────────────┐
│ Advance age     │
│ Update velocity │
└─────────────────┘
```

### Query Flow

```
Input: query embedding
         │
         ▼
┌─────────────────┐
│ Find closest    │
│ invariant       │
└────────┬────────┘
         │
         ▼
┌─────────────────┐
│ Compute         │
│ confidence      │
│                 │
│ sim × stability │
│ × weight_factor │
└────────┬────────┘
         │
         ▼
Output: hint
```

## Memory Model

### Allocation Strategy

All memory is allocated at creation time:
- Runtime: single allocation
- State: single allocation + N slot vectors

No reallocation occurs during operation.

### Bounds

| Resource | Bound | Source |
|----------|-------|--------|
| Memory slots | `max_slots` | Configuration |
| Vector size | `dim × sizeof(float)` | Configuration |
| Total memory | `O(max_slots × dim)` | Fixed |

### Ownership

```
runtime ─┬─► state_1 ─► slots_1
         │
         └─► state_2 ─► slots_2
```

- Runtime owns nothing (just config)
- State owns its slots
- Slots own their vectors

## Invariants

### Constants (Never Change)

| Constant | Value | Meaning |
|----------|-------|---------|
| `CR_EPSILON` | 0.05 | Plasticity floor |
| `CR_SIM_THRESHOLD` | 0.85 | Reinforcement threshold |
| `CR_DECAY_RATE` | 0.995 | Crystallization rate |
| `CR_RECOVERY_RATE` | 1.0005 | Openness recovery |

### Behavioral Invariants

1. **Determinism**: Same inputs → same outputs (always)
2. **Boundedness**: Memory never exceeds `max_slots`
3. **Mercy**: `plasticity ≥ CR_EPSILON` (always)
4. **Monotonic age**: Age only increases

## Persistence Format

```
┌──────────────────────────────────────┐
│ Header (16 bytes)                    │
│   magic: uint32 ("MIND")             │
│   version: uint32 (1)                │
│   dim: int32                         │
│   max_slots: int32                   │
├──────────────────────────────────────┤
│ State (28 bytes)                     │
│   slot_count: int32                  │
│   plasticity: float32                │
│   age: float32                       │
│   plasticity_prev: float32           │
│   velocity: float32                  │
│   last_reinforcement_age: float32    │
│   total_updates: int32               │
│   total_reinforcements: int32        │
├──────────────────────────────────────┤
│ Slots (variable)                     │
│   For each slot:                     │
│     vector: float32[dim]             │
│     weight: float32                  │
└──────────────────────────────────────┘
```

## Thread Safety

MIND is **not thread-safe** by design.

Rationale:
- Simplicity
- Determinism
- No hidden synchronization

For concurrent use:
- Use one state per thread
- Synchronize externally
- Consider state cloning

## Extension Points

MIND is intentionally minimal. Extension happens externally:

| Extension | Approach |
|-----------|----------|
| Different similarity | Fork, modify `cr_cosine_similarity` |
| Different decay | Fork, modify constants |
| Networking | Wrap with transport layer |
| Encryption | Encrypt state files externally |

The core remains simple. Complexity lives outside.
