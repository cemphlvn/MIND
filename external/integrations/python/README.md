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

## Example

A runnable perception-to-memory loop (observe → embed → update → query → persist),
using a deterministic synthetic observation stream — see `mind_integrations/examples.py`:

```bash
make shared
MIND_LIB_PATH=$(pwd)/build/libmind.dylib \
  PYTHONPATH=external/bindings/python:external/integrations/python \
  python3 -m mind_integrations.examples
```

## Grounding (fixture-first)

`mind_integrations.grounding` turns feed items (RSS/Atom or JSON Feed) into
observations and feeds them into MIND. It is **source-agnostic** and reads only
local files by default — see the fixtures under `mind_integrations/fixtures/`.

```python
from mind import MindState
from mind_integrations import EmbeddingAdapter, GroundingAdapter, hash_embedding

state = MindState(dim=32, slots=64)
adapter = EmbeddingAdapter(state, lambda t: hash_embedding(t, 32))
GroundingAdapter(adapter).ingest_feed_file("path/to/feed.xml")
```

Live fetching is intentionally not implemented yet and is gated on an allowlist.
The chosen live sources are **RSS-Bridge** and the **Fediverse APIs** (Chromium
fallback). See [GROUNDING.md](GROUNDING.md) for the live-source posture.
