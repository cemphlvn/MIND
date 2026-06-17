# Grounding — live-source posture

Read this before adding any live (network) feed source. The grounding adapter
(`mind_integrations.grounding`) is **fixture-first**: by default it reads only
local feed files. Live fetching is intentionally not implemented in code yet,
and is gated on an explicit per-source allowlist.

## Chosen live sources (decision 2026-06-17): RSS-Bridge + Fediverse first

1. **RSS-Bridge** (open source) — turn social media and feed-less sites into
   RSS/Atom/JSON, then read those feeds. Preferred: feed-shaped and self-hostable.
2. **Fediverse APIs** (Mastodon, Lemmy, Misskey — ActivityPub) — public,
   documented APIs; the ToS-cleanest direct social source.
3. **Chromium / Playwright / OpenSandbox** — render-and-extract **fallback only**,
   for public pages with no feed, and only within the rules below.

## Rules

1. **Public content only.** No authenticated, private, or paywalled content; no
   login-walled scraping.
2. **Respect each source.** Honor `robots.txt`, rate limits, and the platform's
   Terms of Service. Prefer official APIs and RSS over scraping.
3. **Per-source allowlist.** A source is fetched only after it is explicitly
   added to an allowlist, with its access terms recorded.
4. **Minimize and don't profile.** Store the minimum content needed; this
   pipeline is for concept/feed content, not for profiling individuals.
5. **Determinism in tests.** Tests never touch the network; they use the local
   fixtures under `mind_integrations/fixtures/`.

This note is documentation for the ecosystem layer, not part of MIND core.
