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
Deterministic reference embedder for MIND integrations.

This is NOT a semantic model. It exists only so integrations are runnable and
testable without a model or a network. It encodes no meaning and uses no
randomness and no wall-clock time: identical text always maps to an identical
vector, and different text (almost surely) maps to a different one. Use a real
embedding provider for anything beyond examples and tests.
"""

import hashlib
from typing import List


def hash_embedding(text: str, dim: int) -> List[float]:
    """Return a deterministic, L2-normalized vector of length ``dim`` from ``text``.

    The vector is derived from stable SHA-256 hashes of the input, so it is
    fully deterministic (no RNG, no clock). It is content-addressed, not
    semantic: it does not represent the meaning of ``text``.

    Args:
        text: The text to map to a vector.
        dim: Output dimension (must be positive).

    Returns:
        A list of ``dim`` floats with unit L2 norm.
    """
    if dim <= 0:
        raise ValueError("dim must be positive")

    values: List[float] = []
    block = 0
    while len(values) < dim:
        digest = hashlib.sha256(f"{block}:{text}".encode("utf-8")).digest()
        for byte in digest:
            # Map byte 0..255 deterministically into [-1.0, 1.0].
            values.append((byte / 255.0) * 2.0 - 1.0)
            if len(values) >= dim:
                break
        block += 1

    norm = sum(v * v for v in values) ** 0.5
    if norm == 0.0:
        # Degenerate (vanishingly unlikely); return a fixed unit axis.
        axis = [0.0] * dim
        axis[0] = 1.0
        return axis
    return [v / norm for v in values]
