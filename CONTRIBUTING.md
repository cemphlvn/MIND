# Contributing to MIND

Thank you for your interest in contributing to MIND.

This document explains how to contribute effectively while maintaining
the project's core principles and quality standards.

## Code of Conduct

This project follows the [Contributor Covenant Code of Conduct](CODE_OF_CONDUCT.md).
By participating, you agree to uphold this code.

## Philosophy

Before contributing, understand what MIND is and is not:

**MIND is infrastructure.** It is designed to be:
- Small
- Boring
- Deterministic
- Long-lived

**MIND is not:**
- A feature-rich framework
- A model
- An agent
- A demo

Contributions should make MIND more reliable, not more impressive.

## Types of Contributions

### 1. Bug Reports

Good bug reports include:
- MIND version
- Platform and compiler
- Minimal reproduction steps
- Expected vs actual behavior
- Relevant configuration

File issues at: [GitHub Issues](https://github.com/cemphlvn/MIND/issues)

### 2. Documentation

Documentation improvements are always welcome:
- Clarifications
- Examples
- Typo fixes
- Translations

### 3. Code

Code contributions must:
- Follow the existing style
- Include tests
- Update documentation
- Pass all CI checks
- Include Apache 2.0 headers

### 4. Design Proposals

For significant changes, open a discussion first:
- Describe the problem
- Propose a solution
- Consider alternatives
- Address backwards compatibility

## Development Setup

```bash
# Clone
git clone https://github.com/cemphlvn/MIND.git
cd MIND

# Build
mkdir build && cd build
cmake ..
make

# Test
make test

# Check formatting
make format-check
```

## Code Style

### C Style

- C11 standard
- 4-space indentation
- No tabs
- 80-character line limit (soft)
- Braces on same line
- Snake_case for functions and variables
- UPPER_CASE for constants

### Header Style

Every file must include the Apache 2.0 header:

```c
/*
 * Copyright 2026 The MIND Contributors
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
```

### Commit Messages

Format:
```
<type>: <short summary>

<detailed description if needed>

Signed-off-by: Your Name <your.email@example.com>
```

Types:
- `fix:` Bug fixes
- `feat:` New features
- `docs:` Documentation
- `test:` Tests
- `refactor:` Code restructuring
- `chore:` Maintenance

## Pull Request Process

1. **Fork** the repository
2. **Create** a feature branch from `main`
3. **Implement** your changes
4. **Test** thoroughly
5. **Submit** a pull request

### PR Requirements

- [ ] All tests pass
- [ ] Code follows style guidelines
- [ ] Documentation updated
- [ ] Commit messages follow format
- [ ] Apache 2.0 headers present
- [ ] No new dependencies introduced
- [ ] Backwards compatible (or justified breaking change)

### Review Process

1. Automated CI checks run
2. At least one maintainer reviews
3. Address feedback
4. Maintainer approves and merges

## What We Will Not Accept

To maintain project integrity, we will reject:

- Features that violate determinism
- External dependencies
- GPU-specific code paths
- Randomness or entropy sources
- Wall-clock time usage
- Semantic assumptions about embeddings
- "Clever" optimizations that sacrifice clarity
- Marketing language in documentation

## Becoming a Maintainer

MIND follows a meritocratic model:

1. Contribute consistently over time
2. Demonstrate understanding of project principles
3. Help review others' contributions
4. Be nominated by existing maintainers
5. Consensus approval by maintainer team

See [GOVERNANCE.md](GOVERNANCE.md) for details.

## Questions?

- Open a [Discussion](https://github.com/cemphlvn/MIND/discussions)
- Review existing [Issues](https://github.com/cemphlvn/MIND/issues)

## Recognition

All contributors are listed in [CONTRIBUTORS.md](CONTRIBUTORS.md).

Thank you for helping MIND remain small, reliable, and long-lived.
