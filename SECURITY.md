# Security Policy

## Supported Versions

| Version | Supported          |
| ------- | ------------------ |
| 0.1.x   | :white_check_mark: |

## Security Model

MIND is designed with specific security properties:

### What MIND Guarantees

| Property | Description |
|----------|-------------|
| **Determinism** | No randomness, no hidden state |
| **Bounded memory** | Fixed allocation, no growth |
| **No network** | No outbound connections |
| **No filesystem** | Only explicit save/load |
| **No execution** | Cannot run arbitrary code |

### What MIND Does NOT Guarantee

| Non-Goal | Reason |
|----------|--------|
| Adversarial embedding resistance | Out of scope |
| Side-channel protection | Not designed for |
| Cryptographic security | Not applicable |
| Memory encryption | Host responsibility |

### Threat Model

MIND assumes:
- The host process is trusted
- Embeddings come from trusted sources
- State files are stored securely
- The runtime environment is not compromised

MIND does not defend against:
- Malicious embeddings designed to poison memory
- Compromised host processes
- Physical access attacks
- Memory inspection by other processes

## Reporting a Vulnerability

**Do not report security vulnerabilities through public GitHub issues.**

Instead, please report them via email to:

**security@mind-project.org**

Include:
1. Description of the vulnerability
2. Steps to reproduce
3. Potential impact
4. Suggested fix (if any)

### Response Timeline

| Stage | Timeframe |
|-------|-----------|
| Acknowledgment | 48 hours |
| Initial assessment | 7 days |
| Fix development | 30 days |
| Public disclosure | 90 days (coordinated) |

### What to Expect

1. **Acknowledgment** — We will confirm receipt of your report
2. **Assessment** — We will evaluate severity and impact
3. **Communication** — We will keep you informed of progress
4. **Credit** — We will credit you in the advisory (unless you prefer anonymity)

## Security Best Practices

When using MIND:

### State Files

```bash
# Restrict permissions
chmod 600 mind.state

# Store in secure location
mv mind.state /secure/path/

# Consider encryption at rest
```

### Embedding Sources

- Validate embeddings before feeding to MIND
- Use trusted embedding models
- Monitor for anomalous patterns

### Deployment

- Run MIND in isolated processes
- Use sandboxing where available
- Limit file system access
- Monitor resource usage

## Security Audits

MIND welcomes security audits. If you are conducting a security assessment:

1. Contact us at **security@mind-project.org**
2. We will provide a point of contact
3. We can arrange access to development builds
4. We will coordinate disclosure

## Known Limitations

### Memory Slot Saturation

When all memory slots are full, new experiences are silently ignored.
This is by design (bounded memory) but could be exploited to prevent
learning.

**Mitigation**: Monitor slot utilization via `cr_state_temporal()`.

### Plasticity Floor

Plasticity never reaches zero (ε = 0.05), meaning the system is always
slightly malleable. This is by design (mercy) but means beliefs can
always be influenced given enough repetition.

**Mitigation**: This is a feature, not a bug. Design your system accordingly.

### Calibration Signal Trust

S2S calibration signals are not authenticated. A malicious peer could
send false calibration data.

**Mitigation**: Authenticate peers at the transport layer. MIND does not
handle authentication.

## Changelog

| Date | Change |
|------|--------|
| 2026-02-12 | Initial security policy |
