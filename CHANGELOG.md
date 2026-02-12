# Changelog

All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [Unreleased]

### Added
- Nothing yet

### Changed
- Nothing yet

### Fixed
- Nothing yet

## [0.1.0] - 2026-02-12

### Added

#### Core Runtime
- `cr_runtime_create()` — Create runtime with configuration
- `cr_runtime_destroy()` — Clean up runtime
- `cr_state_create()` — Create cognitive state
- `cr_state_reset()` — Reset state to initial
- `cr_state_destroy()` — Clean up state

#### Experience Processing
- `cr_state_update()` — Feed embedding as experience
- Cosine similarity for invariant matching
- Reinforcement-based memory consolidation
- Bounded memory slots (no growth)

#### Recall
- `cr_state_query()` — Query for hints
- Confidence derivation from similarity, stability, weight

#### Epistemic State
- `cr_state_plasticity()` — Basic plasticity/stability/age
- `cr_state_temporal()` — Rich temporal awareness
- `cr_state_calibration()` — S2S calibration signal export

#### Temporal Awareness
- Continuous experiential age (not wall-clock)
- Plasticity velocity tracking
- Maturity index computation
- Reinforcement temporal landmarks

#### Persistence
- `cr_state_save()` — Save state to file
- `cr_state_load()` — Load state from file
- Magic number and version validation

#### Mercy
- Plasticity floor (ε = 0.05)
- Plasticity never reaches zero
- Early experience has limited long-term impact

### Design Decisions

- **No dependencies** — C11 standard library + libm only
- **No randomness** — Fully deterministic
- **No wall-clock time** — Age via caller-supplied delta_t
- **No semantics** — Operates on vectors, not meaning
- **No text** — Not a language model

### Known Limitations

- Slot saturation silently ignores new experiences
- No eviction policy for full memory
- Persistence format not optimized for size

---

## Version Policy

### Major Version (X.0.0)
- Breaking API changes
- Behavioral changes
- Semantic changes to existing functions

### Minor Version (0.X.0)
- New features (backwards compatible)
- New functions
- Performance improvements

### Patch Version (0.0.X)
- Bug fixes
- Documentation updates
- Build improvements

### Invariants (Never Change)

These will never change without major version bump AND RFC:
- Plasticity floor (ε = 0.05)
- Decay rate (0.995)
- Recovery rate (1.0005)
- Similarity threshold (0.85)
- Deterministic behavior
- Model agnosticism

---

[Unreleased]: https://github.com/cemphlvn/MIND/compare/v0.1.0...HEAD
[0.1.0]: https://github.com/cemphlvn/MIND/releases/tag/v0.1.0
