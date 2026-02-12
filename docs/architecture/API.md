# MIND API Reference

Complete reference for the MIND Cognitive Runtime public API.

## Header

```c
#include <cr.h>
```

## Version

```c
#define CR_VERSION_MAJOR 0
#define CR_VERSION_MINOR 1
#define CR_VERSION_PATCH 0
#define CR_VERSION_STRING "0.1.0"

const char* cr_version(void);
```

## Types

### `cr_runtime_t`

Opaque runtime handle. Holds configuration.

### `cr_state_t`

Opaque state handle. Holds accumulated cognitive memory.

### `cr_f32`

Floating point type (`float`).

### `cr_config_t`

```c
typedef struct {
    int embedding_dim;        // Dimension of embeddings
    int max_memory_slots;     // Maximum invariant slots
    float initial_plasticity; // Starting plasticity (typically 1.0)
} cr_config_t;
```

### `cr_hint_t`

```c
typedef struct {
    cr_f32* vector;    // Pointer to invariant (internal buffer)
    int dim;           // Vector dimension
    float confidence;  // Derived confidence in [0, 1]
} cr_hint_t;
```

### `cr_plasticity_t`

```c
typedef struct {
    float plasticity;  // Current malleability in (ε, 1.0]
    float stability;   // 1 - plasticity
    float age;         // Experiential time
} cr_plasticity_t;
```

### `cr_temporal_t`

```c
typedef struct {
    float age;                      // Total experiential time
    float plasticity;               // Current malleability
    float velocity;                 // Rate of crystallization
    float maturity;                 // age × stability
    float last_reinforcement_age;   // Age at last reinforcement
    float time_since_reinforcement; // Current age - last reinforcement
    int total_updates;              // Update count
    int total_reinforcements;       // Reinforcement count
} cr_temporal_t;
```

### `cr_calibration_t`

```c
typedef struct {
    float age;                 // Experiential time
    float plasticity;          // Current malleability
    float velocity;            // Crystallization rate
    float maturity;            // age × stability
    float reinforcement_ratio; // reinforcements / updates
} cr_calibration_t;
```

## Lifecycle Functions

### `cr_runtime_create`

```c
cr_runtime_t* cr_runtime_create(const cr_config_t* cfg);
```

Create a new runtime.

**Parameters:**
- `cfg`: Configuration (must not be NULL)

**Returns:**
- Runtime handle, or NULL on failure

**Example:**
```c
cr_config_t cfg = {
    .embedding_dim = 128,
    .max_memory_slots = 64,
    .initial_plasticity = 1.0f
};
cr_runtime_t* rt = cr_runtime_create(&cfg);
```

### `cr_runtime_destroy`

```c
void cr_runtime_destroy(cr_runtime_t* rt);
```

Destroy a runtime. All states must be destroyed first.

**Parameters:**
- `rt`: Runtime (may be NULL)

### `cr_state_create`

```c
cr_state_t* cr_state_create(cr_runtime_t* rt);
```

Create a new state.

**Parameters:**
- `rt`: Runtime (must not be NULL)

**Returns:**
- State handle, or NULL on failure

### `cr_state_reset`

```c
void cr_state_reset(cr_state_t* st);
```

Reset state to initial condition.

**Parameters:**
- `st`: State (may be NULL)

### `cr_state_destroy`

```c
void cr_state_destroy(cr_state_t* st);
```

Destroy a state.

**Parameters:**
- `st`: State (may be NULL)

## Experience Functions

### `cr_state_update`

```c
int cr_state_update(
    cr_state_t* st,
    const cr_f32* embedding,
    int dim,
    float delta_t
);
```

Update state with new experience.

**Parameters:**
- `st`: State
- `embedding`: Input embedding vector
- `dim`: Dimension (must match config)
- `delta_t`: Time increment (must be positive)

**Returns:**
- 0 on success, -1 on error

**Behavior:**
1. Find closest invariant by cosine similarity
2. If similarity > 0.85: reinforce (interpolate)
3. Else if space available: create new invariant
4. Else: ignore (bounded memory)
5. Update plasticity
6. Advance age by delta_t

**Example:**
```c
float embedding[128] = { /* ... */ };
cr_state_update(st, embedding, 128, 1.0f);
```

## Query Functions

### `cr_state_query`

```c
int cr_state_query(
    cr_state_t* st,
    const cr_f32* query,
    int dim,
    cr_hint_t* out_hint
);
```

Query state for a hint.

**Parameters:**
- `st`: State
- `query`: Query embedding
- `dim`: Dimension (must match config)
- `out_hint`: Output hint

**Returns:**
- 0 on success, -1 on error

**Confidence formula:**
```
confidence = similarity × (1 - plasticity) × (weight / (weight + 1))
```

**Example:**
```c
cr_hint_t hint;
cr_state_query(st, query, 128, &hint);
printf("Confidence: %.4f\n", hint.confidence);
```

## Epistemic State Functions

### `cr_state_plasticity`

```c
int cr_state_plasticity(
    const cr_state_t* st,
    cr_plasticity_t* out
);
```

Get basic epistemic state.

**Parameters:**
- `st`: State
- `out`: Output structure

**Returns:**
- 0 on success, -1 on error

### `cr_state_temporal`

```c
int cr_state_temporal(
    const cr_state_t* st,
    cr_temporal_t* out
);
```

Get rich temporal awareness.

**Parameters:**
- `st`: State
- `out`: Output structure

**Returns:**
- 0 on success, -1 on error

### `cr_state_calibration`

```c
int cr_state_calibration(
    const cr_state_t* st,
    cr_calibration_t* out
);
```

Get S2S calibration signal.

**Parameters:**
- `st`: State
- `out`: Output structure

**Returns:**
- 0 on success, -1 on error

**Note:** This signal contains no memory content—only meta-cognitive state.

## Persistence Functions

### `cr_state_save`

```c
int cr_state_save(cr_state_t* st, const char* path);
```

Save state to file.

**Parameters:**
- `st`: State
- `path`: File path

**Returns:**
- 0 on success, -1 on error

### `cr_state_load`

```c
int cr_state_load(cr_state_t* st, const char* path);
```

Load state from file.

**Parameters:**
- `st`: State (must have matching config)
- `path`: File path

**Returns:**
- 0 on success, -1 on error

**Note:** Configuration (dim, max_slots) must match saved state.

## Utility Functions

### `cr_version`

```c
const char* cr_version(void);
```

Get version string.

**Returns:**
- Version string (e.g., "0.1.0")

### `cr_runtime_config`

```c
int cr_runtime_config(const cr_runtime_t* rt, cr_config_t* out);
```

Get runtime configuration.

**Parameters:**
- `rt`: Runtime
- `out`: Output configuration

**Returns:**
- 0 on success, -1 on error

### `cr_state_slot_count`

```c
int cr_state_slot_count(const cr_state_t* st);
```

Get current slot count.

**Parameters:**
- `st`: State

**Returns:**
- Number of occupied slots, or -1 on error

## Error Handling

All functions that can fail return an error code:
- `0`: Success
- `-1`: Error

Errors occur when:
- NULL pointers are passed
- Dimension mismatches
- Invalid parameters (negative delta_t)
- I/O failures
- Format mismatches on load

MIND does not set `errno` or provide detailed error messages.
This is intentional—simplicity over diagnostics.
