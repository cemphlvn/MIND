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
Grounding adapter: feed items -> observations -> MIND.

Source-agnostic and fixture-first. You supply feed items (parsed from RSS/Atom,
JSON Feed, or any source); the adapter turns each item into an observation
string and feeds it through the embedding adapter into a MIND state.

Live grounding (fetching real feeds over the network) is intentionally NOT
implemented here. The chosen live sources are RSS-Bridge (social media ->
RSS/Atom) and the Fediverse APIs (Mastodon/Lemmy, ActivityPub) -- both open
source and feed/API shaped -- with a headless-browser path only as a fallback.
This module reads only local feed content. See GROUNDING.md for the live-source
posture (public content only; respect ToS, robots, and rate limits; per-source
allowlist). Parsing uses the standard library only (no third-party deps).
"""

import json
import xml.etree.ElementTree as ET
from dataclasses import dataclass
from pathlib import Path
from typing import List, Union

from mind_integrations.adapter import EmbeddingAdapter


@dataclass
class FeedItem:
    """One normalized feed item."""

    id: str
    title: str
    summary: str
    url: str = ""
    source: str = ""


def _localname(tag: str) -> str:
    """Strip an XML namespace, returning the local tag name."""
    return tag.rsplit("}", 1)[-1] if "}" in tag else tag


def _child_text(element, *localnames: str) -> str:
    """Return stripped text of the first child matching any local name."""
    for child in element:
        if _localname(child.tag) in localnames:
            return (child.text or "").strip()
    return ""


def _child_link(element) -> str:
    """Return a link from RSS <link>text</link> or Atom <link href=...>."""
    for child in element:
        if _localname(child.tag) == "link":
            href = child.get("href")
            if href:
                return href.strip()
            if child.text:
                return child.text.strip()
    return ""


def read_rss_atom(xml_text: str, source: str = "") -> List[FeedItem]:
    """Parse RSS 2.0 or Atom XML into FeedItems (standard library only)."""
    root = ET.fromstring(xml_text)
    # Source precedence: explicit caller value, else the feed's own title.
    feed_title = source
    if not feed_title:
        for element in root.iter():
            if _localname(element.tag) in ("channel", "feed"):
                feed_title = _child_text(element, "title")
                break
    items: List[FeedItem] = []
    for element in root.iter():
        if _localname(element.tag) not in ("item", "entry"):
            continue
        title = _child_text(element, "title")
        summary = _child_text(element, "description", "summary", "content")
        url = _child_link(element)
        ident = _child_text(element, "guid", "id") or url or title
        items.append(
            FeedItem(id=ident, title=title, summary=summary, url=url, source=feed_title)
        )
    return items


def read_json_feed(json_text: str, source: str = "") -> List[FeedItem]:
    """Parse a JSON Feed (jsonfeed.org v1.1) into FeedItems."""
    data = json.loads(json_text)
    feed_source = source or (data.get("title") or "").strip()
    items: List[FeedItem] = []
    for raw in data.get("items", []):
        title = (raw.get("title") or "").strip()
        summary = (raw.get("summary") or raw.get("content_text") or "").strip()
        items.append(
            FeedItem(
                id=str(raw.get("id") or raw.get("url") or title),
                title=title,
                summary=summary,
                url=(raw.get("url") or "").strip(),
                source=feed_source,
            )
        )
    return items


def read_feed(text: str, source: str = "") -> List[FeedItem]:
    """Sniff JSON vs XML and parse accordingly."""
    if text.lstrip()[:1] == "{":
        return read_json_feed(text, source=source)
    return read_rss_atom(text, source=source)


def read_feed_file(path: Union[str, Path], source: str = "") -> List[FeedItem]:
    """Read and parse a local feed file. No network access."""
    p = Path(path)
    return read_feed(p.read_text(encoding="utf-8"), source=source)


def item_to_text(item: FeedItem) -> str:
    """Turn a feed item into one observation string (title + summary)."""
    parts = [part for part in (item.title.strip(), item.summary.strip()) if part]
    return ". ".join(parts)


class GroundingAdapter:
    """Feeds normalized feed items into MIND via an embedding adapter."""

    def __init__(self, embedding_adapter: EmbeddingAdapter):
        """Bind to an existing :class:`EmbeddingAdapter`."""
        self._adapter = embedding_adapter

    def ingest(self, items, delta_t: float = 1.0) -> int:
        """Feed each item's observation text into MIND. Returns the count fed."""
        count = 0
        for item in items:
            text = item_to_text(item)
            if not text:
                continue
            self._adapter.feed(text, delta_t=delta_t)
            count += 1
        return count

    def ingest_feed_file(self, path: Union[str, Path], delta_t: float = 1.0) -> int:
        """Read a local feed file and ingest its items."""
        return self.ingest(read_feed_file(path), delta_t=delta_t)
