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
Basic tests for MIND Python bindings.

Can run with pytest or standalone:
    python -m mind.tests.test_basic
"""

import sys
import tempfile
from pathlib import Path


def test_lifecycle():
    """Test basic lifecycle: create, use, destroy."""
    from mind import MindState

    state = MindState(dim=4, slots=8)
    assert state.dim == 4
    assert state.slot_count == 0

    print("PASS: lifecycle")


def test_update_and_query():
    """Test feeding experience and querying."""
    from mind import MindState

    state = MindState(dim=4, slots=8)

    # Feed experience - need multiple updates to build confidence
    # Single update creates slot but confidence=0 (plasticity=1.0, stability=0.0)
    pattern = [1.0, 0.0, 0.0, 0.0]
    for _ in range(10):
        state.update(pattern, delta_t=1.0)

    assert state.slot_count == 1  # Same pattern reinforces, doesn't create new slot

    # Query - now confidence > 0 because plasticity has decayed
    hint = state.query(pattern)
    assert hint.confidence > 0, f"Expected confidence > 0, got {hint.confidence}"
    assert hint.dim == 4

    print("PASS: update_and_query")


def test_plasticity_bounds():
    """Test mercy floor holds."""
    from mind import MindState

    state = MindState(dim=4, slots=8)
    pattern = [1.0, 0.0, 0.0, 0.0]

    # Many updates to try to force plasticity to zero
    for _ in range(1000):
        state.update(pattern, delta_t=1.0)

    p = state.plasticity()
    assert p.plasticity >= 0.05, f"Plasticity {p.plasticity} below mercy floor"
    assert p.plasticity <= 1.0

    print("PASS: plasticity_bounds")


def test_temporal():
    """Test temporal awareness."""
    from mind import MindState

    state = MindState(dim=4, slots=8)
    pattern = [1.0, 0.0, 0.0, 0.0]

    for _ in range(10):
        state.update(pattern, delta_t=1.0)

    t = state.temporal()
    assert t.age == 10.0
    assert t.total_updates == 10
    assert t.maturity > 0

    print("PASS: temporal")


def test_calibration():
    """Test S2S calibration signal."""
    from mind import MindState

    state = MindState(dim=4, slots=8)
    pattern = [1.0, 0.0, 0.0, 0.0]

    for _ in range(50):
        state.update(pattern, delta_t=1.0)

    cal = state.calibration()
    assert cal.age > 0
    assert cal.plasticity > 0
    assert cal.maturity > 0

    print("PASS: calibration")


def test_persistence():
    """Test save and load."""
    from mind import MindState

    with tempfile.TemporaryDirectory() as tmpdir:
        path = Path(tmpdir) / "test.state"

        # Create and save
        state1 = MindState(dim=4, slots=8)
        pattern = [1.0, 0.0, 0.0, 0.0]
        for _ in range(20):
            state1.update(pattern, delta_t=1.0)

        hint1 = state1.query(pattern)
        state1.save(path)

        # Load into new state
        state2 = MindState(dim=4, slots=8)
        state2.load(path)

        hint2 = state2.query(pattern)
        assert hint1.confidence == hint2.confidence, "Persistence failed"

    print("PASS: persistence")


def test_determinism():
    """Test deterministic behavior."""
    from mind import MindState

    patterns = [
        [1.0, 0.0, 0.0, 0.0],
        [0.0, 1.0, 0.0, 0.0],
        [0.5, 0.5, 0.0, 0.0],
    ]

    confidences = []

    for _ in range(2):
        state = MindState(dim=4, slots=16)
        for i in range(100):
            state.update(patterns[i % 3], delta_t=1.0)
        hint = state.query(patterns[0])
        confidences.append(hint.confidence)

    assert confidences[0] == confidences[1], "Non-deterministic behavior"

    print("PASS: determinism")


def test_context_manager():
    """Test context manager interface."""
    from mind import MindState

    with MindState(dim=4, slots=8) as state:
        state.update([1.0, 0.0, 0.0, 0.0], delta_t=1.0)
        assert state.slot_count == 1

    print("PASS: context_manager")


def run_all():
    """Run all tests."""
    print("MIND Python Bindings Test Suite")
    print("================================\n")

    tests = [
        test_lifecycle,
        test_update_and_query,
        test_plasticity_bounds,
        test_temporal,
        test_calibration,
        test_persistence,
        test_determinism,
        test_context_manager,
    ]

    failures = 0
    for test in tests:
        try:
            test()
        except Exception as e:
            print(f"FAIL: {test.__name__}")
            print(f"  {type(e).__name__}: {e}")
            failures += 1

    print("\n================================")
    if failures == 0:
        print("All tests passed.")
        return 0
    else:
        print(f"{failures} test(s) failed.")
        return 1


if __name__ == "__main__":
    sys.exit(run_all())
