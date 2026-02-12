# MIND

**A self-evolving cognitive runtime and memory layer for AI systems.**

```
┌─────────────────────────────────────────────────────────────┐
│                                                             │
│   MIND is a C library that you embed into your programs.    │
│   It is not a standalone tool or executable.                │
│                                                             │
│   It accumulates experience from vector embeddings,         │
│   compresses repeated patterns into memory,                 │
│   and exposes its own epistemic maturity over time.         │
│                                                             │
│   Models are replaceable. Experience is not.                │
│                                                             │
└─────────────────────────────────────────────────────────────┘
```

## What This Is

MIND is a **library** (`libmind.a`) that provides persistent cognitive memory for AI applications. You link it into your own programs—it does nothing on its own.

**Think of it like SQLite for memory:** small, embeddable, no dependencies, survives restarts.

### Core Capabilities

| Capability | Description |
|------------|-------------|
| **Experience accumulation** | Feed embeddings, build compressed memory |
| **Mercy by construction** | Early inputs don't dominate; confidence requires repetition |
| **Time awareness** | Tracks experiential age, not wall-clock time |
| **Epistemic transparency** | Exposes plasticity, maturity, velocity |
| **S2S calibration** | Export signals for server-to-server coevolution |
| **Persistence** | Save/load cognitive state to files |
| **Determinism** | Same inputs → same evolution, always |

### What This Is NOT

- ❌ Not an executable you run directly
- ❌ Not a chatbot or agent
- ❌ Not a language model
- ❌ Not an embedding generator

MIND accepts embeddings—it doesn't create them. You bring the embeddings from whatever source you have (OpenAI, Ollama, local models, etc.).

---

## Goal

Create a **durable memory layer** that:

1. **Survives model churn** — When you swap LLMs, memory persists
2. **Evolves slowly** — Resists early contamination, requires repetition
3. **Knows itself** — Reports how confident/mature it is
4. **Enables calibration** — Multiple instances can calibrate without sharing beliefs

The design invariant:

> **The system prefers being wrong early to being rigid later.**

---

## How It Works

### Architecture

```
┌─────────────────────────────────────────────────────────────┐
│                   YOUR APPLICATION                          │
│           (Python, Node, Rust, C, whatever)                 │
├─────────────────────────────────────────────────────────────┤
│                          │                                  │
│                          ▼                                  │
│  ┌──────────────┐    ┌──────────────┐    ┌──────────────┐  │
│  │  Embedding   │───▶│    MIND      │───▶│    Hints     │  │
│  │  (float[])   │    │  (libmind.a) │    │ (confidence) │  │
│  └──────────────┘    └──────────────┘    └──────────────┘  │
│                          │                                  │
│                          ▼                                  │
│                   ┌──────────────┐                          │
│                   │ .state file  │                          │
│                   │ (persistent) │                          │
│                   └──────────────┘                          │
└─────────────────────────────────────────────────────────────┘
```

### The Math

```
Plasticity ∈ (0.05, 1.0]     — Never fully crystallizes (mercy)
Decay:     p × 0.995         — On reinforcement (crystallization)
Recovery:  p × 1.0005        — On novelty (preserve openness)
Threshold: similarity > 0.85 — Triggers reinforcement vs. new slot
Confidence = similarity × stability × (weight / (weight + 1))
Maturity   = age × stability
Velocity   = Δplasticity / Δt
```

### Core Loop

```c
// 1. Create runtime and state
cr_runtime_t* rt = cr_runtime_create(&config);
cr_state_t* st = cr_state_create(rt);

// 2. Feed experience (embeddings from your source)
cr_state_update(st, embedding, dim, delta_t);

// 3. Query for hints
cr_hint_t hint;
cr_state_query(st, query_embedding, dim, &hint);
// hint.confidence tells you how much to trust this

// 4. Observe epistemic state
cr_temporal_t t;
cr_state_temporal(st, &t);
// t.plasticity, t.age, t.maturity, t.velocity

// 5. Export calibration signal (for S2S)
cr_calibration_t cal;
cr_state_calibration(st, &cal);
// Exchange with peers—no memory shared, only meta-state

// 6. Persist
cr_state_save(st, "memory.state");
cr_state_load(st, "memory.state");
```

---

## Quick Start

### Build

```bash
git clone https://github.com/cemphlvn/MIND
cd MIND
make
```

This produces:
- `build/libmind.a` — The library you link to
- `build/mind_example` — Demo program

### Run Example

```bash
make example
```

### Run Tests

```bash
make test
```

### Use in Your Project

**C/C++:**
```c
#include "cr.h"

// Link with: -lmind -lm
```

**Python (via ctypes):**
```python
import ctypes
mind = ctypes.CDLL('./build/libmind.a')
# See docs/INTEGRATION.md for full wrapper
```

**Other languages:**
Any language with C FFI can use MIND.

---

## Integration Example

Here's what a real integration looks like:

```python
# example_integration.py (you write this)
import ollama
from mind_ffi import MindState  # You'd write this FFI wrapper

# Initialize MIND
mind = MindState(dim=768, slots=128)
mind.load("memory.state")  # Load previous experience

# Your application loop
while True:
    user_input = input("> ")

    # 1. Get embedding from your model
    emb = ollama.embeddings(model='nomic-embed-text', prompt=user_input)

    # 2. Feed to MIND
    mind.update(emb['embedding'], delta_t=1.0)

    # 3. Check epistemic state
    temporal = mind.get_temporal()
    print(f"Maturity: {temporal.maturity:.2f}")

    # 4. Save periodically
    mind.save("memory.state")
```

---

## Project Structure

```
MIND/
├── foundation/     # Layer 0: Pure math (never changes)
├── core/           # Layer 1: Cognitive memory
├── external/       # Bindings, integrations, protocols
├── docs/           # Documentation
├── examples/
└── tests/
```

See [LAYERS.md](LAYERS.md) for architecture details.

---

## Documentation

| Document | Description |
|----------|-------------|
| [LAYERS.md](LAYERS.md) | Layered architecture |
| [docs/architecture/API.md](docs/architecture/API.md) | Complete API reference |
| [docs/architecture/ARCHITECTURE.md](docs/architecture/ARCHITECTURE.md) | Internal design |
| [docs/architecture/S2S_PROTOCOL.md](docs/architecture/S2S_PROTOCOL.md) | Server-to-server calibration |
| [CONTRIBUTING.md](CONTRIBUTING.md) | How to contribute |
| [GOVERNANCE.md](GOVERNANCE.md) | Project governance |
| [SECURITY.md](SECURITY.md) | Security policy |

---

## Future Directions

### v0.2 — Robustness
- Safer persistence format
- State versioning
- Debug assertions

### v0.3 — Integration
- Python bindings (`pip install mind`)
- Node.js bindings
- Rust crate

### v0.4 — S2S Protocol
- Probe registry (shared coordinates)
- Probe-specific response export
- Multi-peer calibration

### v1.0 — Stability
- Long-term API guarantee
- Formal verification
- Production hardening

### Research Directions
- Eviction policies for full memory
- Embedding drift detection
- Hierarchical memory structures
- Federated calibration networks

---

## Status

**v0.1.0** — Core semantics frozen.

| Component | Status |
|-----------|--------|
| Core runtime | ✅ Complete |
| Memory/plasticity | ✅ Complete |
| Time awareness | ✅ Complete |
| S2S calibration export | ✅ Complete |
| Persistence | ✅ Complete |
| S2S probe protocol | ❌ Not yet |
| Language bindings | ❌ Not yet |
| Production hardening | ❌ Not yet |

---

## License

Apache License 2.0. See [LICENSE](LICENSE).

Commercial use permitted. Patent grant included.

---

## Philosophy

MIND embodies a specific stance on AI memory:

1. **Memory precedes intelligence** — Structure before performance
2. **Early certainty is a defect** — Distrust first impressions
3. **Time is experiential** — Age by exposure, not clocks
4. **Plasticity must be visible** — No hidden authority
5. **Models are replaceable** — Experience transcends implementations

For the full principles, see [docs/site/principles.html](docs/site/principles.html).
