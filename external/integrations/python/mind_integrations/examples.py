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
Example: a perception-to-memory loop built on the MIND embedding adapter.

This is an *example*, not part of MIND core and not an agent. It feeds a
deterministic stream of text observations through the embedding adapter into a
MIND state, reads back a hint each step, derives a trivial recognized/novel
label from the hint's confidence, and optionally persists and reloads the
state. It exists to show, end to end, how text becomes accumulated experience.

Run from the repository root:
    make integration-test           # runs this module's smoke tests
or directly:
    MIND_LIB_PATH=$(pwd)/build/libmind.dylib \\
    PYTHONPATH=external/bindings/python:external/integrations/python \\
    python -m mind_integrations.examples
"""

from typing import List, Optional, Sequence

from mind import MindState
from mind_integrations.adapter import EmbeddingAdapter
from mind_integrations.embedders import hash_embedding

# A deterministic, synthetic observation stream — a stand-in for a real feed.
# Some topics recur (so memory crystallizes); others appear once (novelty).
DEFAULT_OBSERVATIONS: List[str] = [
    "market: prices rising",
    "weather: clear skies",
    "market: prices rising",
    "sports: local team wins",
    "market: prices rising",
    "weather: clear skies",
    "tech: new chip announced",
    "market: prices rising",
    "weather: clear skies",
    "market: prices rising",
]


def _label(confidence: float) -> str:
    """Trivial read of a hint: 'recognized' once confidence is positive."""
    return "recognized" if confidence > 0.0 else "novel"


def run_experience_loop(
    observations: Sequence[str],
    *,
    dim: int = 32,
    slots: int = 64,
    state_path: Optional[str] = None,
    verbose: bool = False,
) -> dict:
    """Feed ``observations`` into a MIND state via the adapter; return stats.

    Args:
        observations: The text observations to record, in order.
        dim: Embedding dimension.
        slots: Maximum memory slots.
        state_path: If set, the state is saved here and reloaded before stats
            are read (a persistence round-trip).
        verbose: If True, print a per-step trace.

    Returns:
        A dict with: steps, slots, final_maturity, final_plasticity,
        last_confidence, recognized.
    """

    def embed(text: str) -> List[float]:
        return hash_embedding(text, dim)

    state = MindState(dim=dim, slots=slots)
    adapter = EmbeddingAdapter(state, embed)

    recognized = 0
    last_confidence = 0.0
    for i, observation in enumerate(observations, start=1):
        adapter.feed(observation)               # observe -> embed -> update
        hint = adapter.hint(observation)        # query
        last_confidence = hint.confidence
        label = _label(hint.confidence)         # act (trivial)
        if label == "recognized":
            recognized += 1
        if verbose:
            t = state.temporal()
            print(
                f"[{i:02d}] {label:10s} conf={hint.confidence:.3f} "
                f"plasticity={t.plasticity:.3f} maturity={t.maturity:.3f} "
                f"slots={state.slot_count}  <- {observation!r}"
            )

    if state_path is not None:                  # persist + reload
        state.save(state_path)
        reloaded = MindState(dim=dim, slots=slots)
        reloaded.load(state_path)
        state = reloaded

    t = state.temporal()
    return {
        "steps": len(observations),
        "slots": state.slot_count,
        "final_maturity": t.maturity,
        "final_plasticity": t.plasticity,
        "last_confidence": last_confidence,
        "recognized": recognized,
    }


def main() -> int:
    """Run the example loop with the default observations and print a trace."""
    print("MIND example - perception-to-memory loop")
    print("========================================\n")
    stats = run_experience_loop(DEFAULT_OBSERVATIONS, verbose=True)
    print("\nsummary:", stats)
    return 0


if __name__ == "__main__":
    import sys

    sys.exit(main())
