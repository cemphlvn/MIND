# MIND — Layered Architecture

```
┌─────────────────────────────────────────────────────────────────┐
│  EXTERNAL                                                       │
│  Bindings, integrations, protocol implementations               │
│  → external/                                                    │
├─────────────────────────────────────────────────────────────────┤
│  CORE (Layer 1) - Cognitive Memory                              │
│  Experience accumulation, plasticity, confidence                │
│  → core/                                                        │
├─────────────────────────────────────────────────────────────────┤
│  FOUNDATION (Layer 0) - Pure Math                               │
│  Vector operations, no semantics, NEVER CHANGES                 │
│  → foundation/                                                  │
└─────────────────────────────────────────────────────────────────┘
```

## Principles

1. **Foundation never breaks** — Pure math, frozen interface
2. **Core only depends downward** — Memory uses foundation, never external
3. **External is optional** — System works without bindings

## Directory Structure

```
MIND/
├── foundation/           # Layer 0: Pure math
│   ├── include/
│   │   └── mind_vec.h    # Vector operations
│   └── src/
│       └── mind_vec.c
│
├── core/                 # Layer 1: Cognitive memory
│   ├── include/
│   │   ├── cr.h          # Public API
│   │   └── cr_internal.h # Internal structures
│   └── src/
│       ├── cr_runtime.c  # Lifecycle
│       ├── cr_state.c    # Memory & learning
│       ├── cr_query.c    # Hints
│       ├── cr_temporal.c # Time awareness
│       └── cr_persist.c  # Persistence
│
├── external/             # Everything outside core
│   ├── bindings/
│   │   └── python/       # Python FFI
│   ├── integrations/
│   │   └── (ollama, openai, etc.)
│   └── protocols/
│       └── s2s/          # Server-to-server
│
├── docs/
│   ├── architecture/     # Design documents
│   ├── spec/             # Formal specifications
│   └── site/             # Website
│
├── examples/
├── tests/
└── articles/
```

## Build Outputs

```
build/
├── libmind_foundation.a  # Foundation only
├── libmind_core.a        # Core only (needs foundation)
└── libmind.a             # Combined (use this)
```

## Dependency Rules

```
external → core → foundation
              ↓
          libm (system)
```

- Foundation depends only on libm
- Core depends only on foundation
- External can depend on anything

## Future Layers

```
┌─────────────────────────────────────────────────────────────────┐
│  LAYER 5: REFLECTION  (future)                                  │
├─────────────────────────────────────────────────────────────────┤
│  LAYER 4: ACTION  (future)                                      │
├─────────────────────────────────────────────────────────────────┤
│  LAYER 3: REASONING  (future)                                   │
├─────────────────────────────────────────────────────────────────┤
│  LAYER 2: PERCEPTION  (future)                                  │
├─────────────────────────────────────────────────────────────────┤
│  LAYER 1: MEMORY  ← current (core/)                             │
├─────────────────────────────────────────────────────────────────┤
│  LAYER 0: SUBSTRATE  ← current (foundation/)                    │
└─────────────────────────────────────────────────────────────────┘
```

Each future layer will be a separate directory with its own include/src structure, depending only on layers below it.
