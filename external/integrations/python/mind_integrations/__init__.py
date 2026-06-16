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
MIND integrations: provider-agnostic adapters from text to cognitive memory.

This package wires text through a caller-supplied embedding function into a
MIND state. It depends on the `mind` Python binding at runtime (put it on the
PYTHONPATH) but makes no assumptions about embedding semantics.
"""

from mind_integrations.adapter import EmbeddingAdapter, feed_text
from mind_integrations.embedders import hash_embedding

__version__ = "0.1.0"
__all__ = ["EmbeddingAdapter", "feed_text", "hash_embedding"]
