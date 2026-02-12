# MIND Python Bindings

Pure Python bindings for the MIND cognitive runtime using ctypes.

**Zero dependencies.** Uses only Python standard library.

## Installation

### From source (requires built libmind)

```bash
# Build the C library first
cd /path/to/MIND
make

# Install Python bindings
cd external/bindings/python
pip install -e .
```

### Set library path (if not in standard location)

```bash
export MIND_LIB_PATH=/path/to/MIND/build/libmind.dylib  # macOS
export MIND_LIB_PATH=/path/to/MIND/build/libmind.so     # Linux
```

## Quick Start

```python
from mind import MindState

# Create state
state = MindState(dim=768, slots=128)

# Feed experience (embeddings from your source)
embedding = [0.1, 0.2, ...]  # 768-dimensional vector
state.update(embedding, delta_t=1.0)

# Query for hints
hint = state.query(embedding)
print(f"Confidence: {hint.confidence}")

# Check epistemic state
temporal = state.temporal()
print(f"Plasticity: {temporal.plasticity}")
print(f"Maturity: {temporal.maturity}")

# S2S calibration signal
cal = state.calibration()
print(f"Age: {cal.age}, Velocity: {cal.velocity}")

# Persistence
state.save("memory.state")
state.load("memory.state")
```

## API Reference

### MindState

```python
MindState(dim: int, slots: int, initial_plasticity: float = 1.0)
```

Main class for cognitive memory.

**Methods:**

| Method | Description |
|--------|-------------|
| `update(embedding, delta_t)` | Feed experience |
| `query(embedding) -> Hint` | Get hint with confidence |
| `plasticity() -> Plasticity` | Basic epistemic state |
| `temporal() -> Temporal` | Rich temporal awareness |
| `calibration() -> Calibration` | S2S calibration signal |
| `reset()` | Reset to initial state |
| `save(path)` | Save to file |
| `load(path)` | Load from file |

**Properties:**

| Property | Description |
|----------|-------------|
| `dim` | Embedding dimension |
| `slot_count` | Number of occupied slots |

### Data Classes

```python
@dataclass
class Hint:
    vector: Optional[List[float]]
    dim: int
    confidence: float

@dataclass
class Plasticity:
    plasticity: float
    stability: float
    age: float

@dataclass
class Temporal:
    age: float
    plasticity: float
    velocity: float
    maturity: float
    last_reinforcement_age: float
    time_since_reinforcement: float
    total_updates: int
    total_reinforcements: int

@dataclass
class Calibration:
    age: float
    plasticity: float
    velocity: float
    maturity: float
    reinforcement_ratio: float
```

## Testing

```bash
# Run tests (no pytest required)
python -m mind.tests.test_basic

# Or with pytest
pytest mind/tests/
```

## Integration Example

```python
import ollama
from mind import MindState

# Initialize
state = MindState(dim=768, slots=128)
state.load("memory.state")  # Load previous experience

# Application loop
while True:
    user_input = input("> ")

    # Get embedding from your model
    result = ollama.embeddings(model='nomic-embed-text', prompt=user_input)

    # Feed to MIND
    state.update(result['embedding'], delta_t=1.0)

    # Check epistemic state
    t = state.temporal()
    print(f"Maturity: {t.maturity:.2f}, Plasticity: {t.plasticity:.4f}")

    # Save periodically
    state.save("memory.state")
```

## Architecture

```
┌─────────────────────────────────────────────────────────────┐
│  Your Python Application                                     │
├─────────────────────────────────────────────────────────────┤
│  mind (Python package)                                       │
│  ├── MindState (Pythonic API)                               │
│  └── _ffi.py (ctypes bindings)                              │
├─────────────────────────────────────────────────────────────┤
│  libmind.so / libmind.dylib (C library)                     │
│  ├── core/ (cognitive memory)                               │
│  └── foundation/ (vector math)                              │
└─────────────────────────────────────────────────────────────┘
```

## License

Apache License 2.0
