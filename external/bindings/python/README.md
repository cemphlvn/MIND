# Python Bindings

Python FFI wrapper for MIND.

## Status

Not yet implemented. See roadmap in main README.

## Planned API

```python
from mind import MindState

# Create state
state = MindState(dim=768, slots=128)

# Feed experience
state.update(embedding, delta_t=1.0)

# Query
hint = state.query(embedding)
print(f"Confidence: {hint.confidence}")

# Epistemic state
temporal = state.temporal()
print(f"Plasticity: {temporal.plasticity}")
print(f"Maturity: {temporal.maturity}")

# Persistence
state.save("memory.state")
state.load("memory.state")

# S2S calibration
cal = state.calibration()
```
