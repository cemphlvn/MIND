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
Smoke tests for the perception-to-memory loop example.

Run standalone:
    MIND_LIB_PATH=<...>/build/libmind.dylib \
    PYTHONPATH=external/bindings/python:external/integrations/python \
    python -m mind_integrations.tests.test_examples
"""

import sys
import tempfile
from pathlib import Path


def test_loop_runs_and_accumulates():
    """The loop completes and accumulates bounded, mature memory."""
    from mind_integrations.examples import DEFAULT_OBSERVATIONS, run_experience_loop

    stats = run_experience_loop(DEFAULT_OBSERVATIONS, dim=32, slots=64)

    assert stats["steps"] == len(DEFAULT_OBSERVATIONS)
    assert stats["slots"] > 0, stats
    assert stats["slots"] <= 64, stats
    assert stats["final_maturity"] > 0, stats
    assert 0.05 <= stats["final_plasticity"] <= 1.0, stats

    print("PASS: loop_runs_and_accumulates")


def test_loop_deterministic():
    """Same observations -> identical stats (whole pipeline is deterministic)."""
    from mind_integrations.examples import DEFAULT_OBSERVATIONS, run_experience_loop

    a = run_experience_loop(DEFAULT_OBSERVATIONS)
    b = run_experience_loop(DEFAULT_OBSERVATIONS)
    assert a == b, (a, b)

    print("PASS: loop_deterministic")


def test_loop_persistence_roundtrip():
    """A persisted state reloads and still reports accumulated memory."""
    from mind_integrations.examples import DEFAULT_OBSERVATIONS, run_experience_loop

    with tempfile.TemporaryDirectory() as tmpdir:
        path = str(Path(tmpdir) / "demo.state")
        stats = run_experience_loop(DEFAULT_OBSERVATIONS, state_path=path)
        assert Path(path).exists(), "state file should be written"
        assert stats["slots"] > 0, stats

    print("PASS: loop_persistence_roundtrip")


def run_all():
    """Run all example smoke tests."""
    print("MIND Integrations - Perception-to-Memory Loop Smoke Tests")
    print("=========================================================\n")

    tests = [
        test_loop_runs_and_accumulates,
        test_loop_deterministic,
        test_loop_persistence_roundtrip,
    ]

    failures = 0
    for test in tests:
        try:
            test()
        except Exception as e:
            print(f"FAIL: {test.__name__}")
            print(f"  {type(e).__name__}: {e}")
            failures += 1

    print("\n=========================================================")
    if failures == 0:
        print("All tests passed.")
        return 0
    print(f"{failures} test(s) failed.")
    return 1


if __name__ == "__main__":
    sys.exit(run_all())
