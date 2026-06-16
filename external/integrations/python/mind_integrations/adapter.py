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
Provider-agnostic embedding adapter: text -> embedding -> MIND.

The adapter wires text through a caller-supplied embedding function into a
MIND state. It is provider-agnostic by construction: you pass ``embed_fn``,
a callable mapping text to a sequence of floats. The adapter makes no
assumptions about what an embedding means; it only forwards vectors to MIND
and reads back hints. Any provider (a local model, a hosted API, the bundled
deterministic reference embedder) can be supplied as ``embed_fn``.
"""

from typing import Callable, Iterable, Sequence

EmbedFn = Callable[[str], Sequence[float]]


def feed_text(state, text, embed_fn, delta_t=1.0):
    """Embed ``text`` with ``embed_fn`` and feed it to ``state`` as one experience.

    Args:
        state: A MIND state (anything exposing ``update(embedding, delta_t)``).
        text: The text observation to record.
        embed_fn: Callable mapping ``text`` to a sequence of floats.
        delta_t: Experiential time increment (must be positive).
    """
    state.update(embed_fn(text), delta_t=delta_t)


class EmbeddingAdapter:
    """Feeds text into a MIND state via a caller-supplied embedding function.

    Example:
        >>> from mind import MindState
        >>> from mind_integrations import EmbeddingAdapter, hash_embedding
        >>> state = MindState(dim=32, slots=64)
        >>> adapter = EmbeddingAdapter(state, lambda t: hash_embedding(t, 32))
        >>> adapter.feed("a recurring observation")
        >>> hint = adapter.hint("a recurring observation")
    """

    def __init__(self, state, embed_fn: EmbedFn):
        """Bind an adapter to a MIND ``state`` and an ``embed_fn``."""
        self._state = state
        self._embed = embed_fn

    def feed(self, text: str, delta_t: float = 1.0) -> None:
        """Embed ``text`` and feed it to MIND as one experience."""
        self._state.update(self._embed(text), delta_t=delta_t)

    def feed_many(self, texts: Iterable[str], delta_t: float = 1.0) -> int:
        """Feed many texts in order. Returns the count fed."""
        count = 0
        for text in texts:
            self.feed(text, delta_t=delta_t)
            count += 1
        return count

    def hint(self, text: str):
        """Embed ``text`` and return MIND's hint (with confidence) for it."""
        return self._state.query(self._embed(text))
