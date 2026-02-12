# Governance

This document describes how the MIND project is governed.

## Principles

MIND governance follows the [Apache Way](https://www.apache.org/theapacheway/):

1. **Community Over Code** — A healthy community is more important than code
2. **Earned Authority** — Decision-making power is earned through contribution
3. **Consensus Seeking** — Decisions made through discussion, not dictation
4. **Open Development** — All decisions made in public
5. **Independence** — No single entity controls the project

## Roles

### Users

Anyone who uses MIND. Users can:
- Use the software
- Report bugs
- Request features
- Participate in discussions

### Contributors

Anyone who contributes to MIND. Contributors can:
- Submit pull requests
- Improve documentation
- Help other users
- Review code

### Committers

Contributors with direct commit access. Committers can:
- Merge pull requests
- Triage issues
- Guide contributors

**Becoming a Committer**: Nomination by existing committer, followed by
consensus vote among committers.

### Maintainers

Committers with project-wide decision authority. Maintainers can:
- Make architectural decisions
- Manage releases
- Update governance
- Represent the project

**Becoming a Maintainer**: Nomination by existing maintainer, followed by
consensus vote among maintainers.

## Decision Making

### Lazy Consensus

Most decisions use lazy consensus:
1. Proposal is made publicly
2. Reasonable time for objections (typically 72 hours)
3. If no objections, proposal passes
4. Silence is consent

### Voting

For significant decisions, formal voting may be used:

| Vote | Meaning |
|------|---------|
| +1 | Yes / Approve |
| 0 | Abstain / No opinion |
| -1 | No / Veto (must include justification) |

Voting rules:
- Votes are public
- Minimum 72 hours voting period
- Majority wins for most decisions
- Vetoes require technical justification

### What Requires Voting

| Decision | Required |
|----------|----------|
| Code changes | Lazy consensus |
| New committers | Committer vote |
| New maintainers | Maintainer vote |
| Major releases | Maintainer vote |
| Governance changes | Maintainer vote (unanimous) |
| License changes | Maintainer vote (unanimous) |

## Communication

All project communication happens in public:

| Channel | Purpose |
|---------|---------|
| GitHub Issues | Bug reports, feature requests |
| GitHub Discussions | General discussion |
| GitHub Pull Requests | Code review |
| Mailing list | Announcements |

Private communication is reserved for:
- Security vulnerabilities (see SECURITY.md)
- Code of Conduct enforcement
- Personal matters

## Releases

Release process:
1. Maintainer proposes release
2. Maintainer vote (majority)
3. Release candidate prepared
4. Testing period (minimum 7 days)
5. Final release

Release requirements:
- All tests pass
- Documentation updated
- CHANGELOG updated
- No known critical bugs

## Project Direction

### What MIND Will Always Be

- Small
- Deterministic
- Model-agnostic
- Dependency-free
- Apache 2.0 licensed

### What MIND Will Never Do

- Generate text
- Require specific models
- Include randomness
- Use wall-clock time
- Depend on external services

These invariants can only be changed by unanimous maintainer vote AND
a project-wide RFC process.

## Conflict Resolution

1. **Discussion** — Parties discuss in public
2. **Mediation** — Maintainer mediates
3. **Vote** — If unresolved, maintainer vote
4. **Escalation** — In extreme cases, external mediation

## Amendments

This governance document can be amended by:
1. Proposal by any maintainer
2. Discussion period (minimum 14 days)
3. Unanimous maintainer approval

## Current Governance

### Maintainers

| Name | GitHub | Since |
|------|--------|-------|
| (Founding maintainers to be listed) | | 2026-02 |

### Committers

| Name | GitHub | Since |
|------|--------|-------|
| (Committers to be listed) | | |

## Attribution

This governance model draws inspiration from:
- [Apache Software Foundation](https://www.apache.org/)
- [Rust Project](https://www.rust-lang.org/governance)
- [Node.js](https://nodejs.org/en/about/governance/)
