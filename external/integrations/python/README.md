# mind-integrations (Python)

Provider-agnostic adapter wiring text into a MIND state:

```
text → embed_fn → MIND.update → (MIND.query) → hint
```

The adapter makes **no assumptions about embedding semantics**. You supply
`embed_fn` (text → sequence of floats) from any source — a local model, a
hosted API, or the bundled deterministic reference embedder used for tests.

## Use

```python
from mind import MindState                       # the MIND binding
from mind_integrations import EmbeddingAdapter, hash_embedding

state = MindState(dim=32, slots=64)
adapter = EmbeddingAdapter(state, lambda t: hash_embedding(t, 32))

adapter.feed("a recurring observation")
hint = adapter.hint("a recurring observation")
print(hint.confidence)
```

To use a real provider, pass its embedding call as `embed_fn`, e.g.
`EmbeddingAdapter(state, lambda t: my_provider.embed(t))`.

`hash_embedding` is a **deterministic placeholder**, not a semantic model — it
exists only so the integration is runnable and testable without a model or a
network.

## Requirements

- The `mind` Python binding on the `PYTHONPATH` (`external/bindings/python`).
- The MIND shared library built (`make shared`); point `MIND_LIB_PATH` at it.
- Python 3.8+. No third-party dependencies.

## Test

From the repository root:

```bash
make integration-test
```
