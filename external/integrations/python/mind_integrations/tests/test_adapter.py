# Copyright 2026 The MIND Contributors
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

"""
Tests for the MIND embedding adapter.

Requires the `mind` binding and the shared library. Run standalone:
    MIND_LIB_PATH=<...>/build/libmind.dylib \
    PYTHONPATH=external/bindings/python:external/integrations/python \
    python -m mind_integrations.tests.test_adapter
"""

import sys

DIM = 32


def test_embedder_deterministic_unit_norm():
    """Reference embedder is deterministic, unit-norm, and content-sensitive."""
    from mind_integrations import hash_embedding

    a = hash_embedding("hello", DIM)
    b = hash_embedding("hello", DIM)
    c = hash_embedding("world", DIM)

    assert len(a) == DIM
    assert a == b, "embedder must be deterministic"
    assert a != c, "different text should give different vectors"
    norm = sum(x * x for x in a) ** 0.5
    assert abs(norm - 1.0) < 1e-5, f"expected unit norm, got {norm}"

    print("PASS: embedder_deterministic_unit_norm")


def test_feed_creates_and_reinforces():
    """Distinct texts create slots; repeated text reinforces rather than adds."""
    from mind import MindState
    from mind_integrations import EmbeddingAdapter, hash_embedding

    state = MindState(dim=DIM, slots=32)
    adapter = EmbeddingAdapter(state, lambda t: hash_embedding(t, DIM))

    adapter.feed("apple")
    adapter.feed("banana")
    assert state.slot_count == 2, f"expected 2 slots, got {state.slot_count}"

    for _ in range(10):
        adapter.feed("apple")
    assert state.slot_count == 2, (
        f"expected 2 slots after reinforcement, got {state.slot_count}"
    )

    print("PASS: feed_creates_and_reinforces")


def test_hint_confidence_rises_with_repetition():
    """Repeated observation yields a positive-confidence hint."""
    from mind import MindState
    from mind_integrations import EmbeddingAdapter, hash_embedding

    state = MindState(dim=DIM, slots=32)
    adapter = EmbeddingAdapter(state, lambda t: hash_embedding(t, DIM))

    for _ in range(20):
        adapter.feed("recurring observation")

    hint = adapter.hint("recurring observation")
    assert hint.confidence > 0, f"expected confidence > 0, got {hint.confidence}"

    print("PASS: hint_confidence_rises_with_repetition")


def test_feed_many_counts():
    """feed_many returns the number fed and accumulates slots."""
    from mind import MindState
    from mind_integrations import EmbeddingAdapter, hash_embedding

    state = MindState(dim=DIM, slots=32)
    adapter = EmbeddingAdapter(state, lambda t: hash_embedding(t, DIM))

    n = adapter.feed_many(["a", "b", "c"])
    assert n == 3
    assert state.slot_count == 3

    print("PASS: feed_many_counts")


def test_adapter_determinism():
    """The whole text->embed->MIND pipeline is deterministic."""
    from mind import MindState
    from mind_integrations import EmbeddingAdapter, hash_embedding

    texts = ["alpha", "beta", "gamma"]
    confidences = []
    for _ in range(2):
        state = MindState(dim=DIM, slots=32)
        adapter = EmbeddingAdapter(state, lambda t: hash_embedding(t, DIM))
        for i in range(60):
            adapter.feed(texts[i % len(texts)])
        confidences.append(adapter.hint("alpha").confidence)

    assert confidences[0] == confidences[1], "adapter pipeline must be deterministic"

    print("PASS: adapter_determinism")


def run_all():
    """Run all adapter tests."""
    print("MIND Integrations - Embedding Adapter Test Suite")
    print("================================================\n")

    tests = [
        test_embedder_deterministic_unit_norm,
        test_feed_creates_and_reinforces,
        test_hint_confidence_rises_with_repetition,
        test_feed_many_counts,
        test_adapter_determinism,
    ]

    failures = 0
    for test in tests:
        try:
            test()
        except Exception as e:
            print(f"FAIL: {test.__name__}")
            print(f"  {type(e).__name__}: {e}")
            failures += 1

    print("\n================================================")
    if failures == 0:
        print("All tests passed.")
        return 0
    print(f"{failures} test(s) failed.")
    return 1


if __name__ == "__main__":
    sys.exit(run_all())
