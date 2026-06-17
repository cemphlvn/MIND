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
Tests for the grounding adapter (fixture-first; no network).

Run standalone:
    MIND_LIB_PATH=<...>/build/libmind.dylib \
    PYTHONPATH=external/bindings/python:external/integrations/python \
    python -m mind_integrations.tests.test_grounding
"""

import sys
from pathlib import Path

FIXTURES = Path(__file__).resolve().parent.parent / "fixtures"
DIM = 32


def test_read_rss_fixture():
    """RSS fixture parses into items; repeated text is preserved identically."""
    from mind_integrations.grounding import item_to_text, read_feed_file

    items = read_feed_file(FIXTURES / "sample_feed.xml")
    assert len(items) == 3, items
    assert items[0].title == "Market prices rising"
    assert "indices" in items[0].summary
    assert items[0].url == "https://example.invalid/1"
    assert item_to_text(items[0]) == item_to_text(items[2])  # duplicate content

    print("PASS: read_rss_fixture")


def test_read_json_fixture():
    """JSON Feed fixture parses into items with ids and titles."""
    from mind_integrations.grounding import read_feed_file

    items = read_feed_file(FIXTURES / "sample_feed.json")
    assert len(items) == 2, items
    assert items[0].title == "New chip announced"
    assert items[0].id == "fedi-1"
    assert items[0].source == "Sample Fediverse Feed"

    print("PASS: read_json_fixture")


def test_ground_into_mind():
    """Grounding feeds items into MIND; duplicate content reinforces one slot."""
    from mind import MindState
    from mind_integrations import EmbeddingAdapter, hash_embedding
    from mind_integrations.grounding import GroundingAdapter, read_feed_file

    state = MindState(dim=DIM, slots=32)
    adapter = EmbeddingAdapter(state, lambda t: hash_embedding(t, DIM))
    grounding = GroundingAdapter(adapter)

    fed = grounding.ingest(read_feed_file(FIXTURES / "sample_feed.xml"))
    assert fed == 3, fed
    # 3 items, 2 distinct texts -> 2 slots (the duplicate reinforces).
    assert state.slot_count == 2, state.slot_count

    print("PASS: ground_into_mind")


def test_ground_deterministic():
    """The whole feed->observation->MIND pipeline is deterministic."""
    from mind import MindState
    from mind_integrations import EmbeddingAdapter, hash_embedding
    from mind_integrations.grounding import (
        GroundingAdapter,
        item_to_text,
        read_feed_file,
    )

    def run():
        state = MindState(dim=DIM, slots=32)
        adapter = EmbeddingAdapter(state, lambda t: hash_embedding(t, DIM))
        items = read_feed_file(FIXTURES / "sample_feed.xml")
        GroundingAdapter(adapter).ingest(items)
        return (state.slot_count, adapter.hint(item_to_text(items[0])).confidence)

    assert run() == run()

    print("PASS: ground_deterministic")


def run_all():
    """Run all grounding tests."""
    print("MIND Integrations - Grounding Adapter Test Suite")
    print("================================================\n")

    tests = [
        test_read_rss_fixture,
        test_read_json_fixture,
        test_ground_into_mind,
        test_ground_deterministic,
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
