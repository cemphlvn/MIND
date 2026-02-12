# Server-to-Server Calibration Protocol

This document describes the S2S cognitive coevolution protocol.

## Overview

S2S calibration enables independent MIND instances to achieve mutual
epistemic calibration without sharing memory or beliefs.

The goal is **calibration**, not **consensus**.

## What Is Exchanged

Systems exchange a minimal calibration signal:

```c
typedef struct {
    float age;                 // Experiential time
    float plasticity;          // Current malleability
    float velocity;            // Rate of crystallization
    float maturity;            // age × stability
    float reinforcement_ratio; // repetitions / updates
} cr_calibration_t;
```

**Size:** 20 bytes (5 × float32)

## What Is NOT Exchanged

- Memory content
- Invariant vectors
- Embeddings
- Beliefs
- Confidence in specific facts

## Protocol Flow

```
┌──────────────┐                      ┌──────────────┐
│   Server A   │                      │   Server B   │
└──────┬───────┘                      └──────┬───────┘
       │                                     │
       │──── cr_calibration_t ──────────────▶│
       │                                     │
       │◀─── cr_calibration_t ───────────────│
       │                                     │
       ▼                                     ▼
   Compare                              Compare
   - relative age                       - relative age
   - relative maturity                  - relative maturity
   - relative velocity                  - relative velocity
       │                                     │
       ▼                                     ▼
   Adjust meta-calibration             Adjust meta-calibration
   (NOT memory)                        (NOT memory)
```

## Calibration Questions

After exchange, each system can answer:

| Question | How |
|----------|-----|
| Am I older? | Compare age |
| Am I more mature? | Compare maturity |
| Am I crystallizing faster? | Compare velocity |
| Is my confidence unusual? | Compare plasticity |
| Am I seeing more repetition? | Compare reinforcement_ratio |

## Use Cases

### 1. Confidence Checking

```c
cr_calibration_t mine, theirs;
cr_state_calibration(my_state, &mine);
receive_calibration(&theirs);

if (mine.plasticity < theirs.plasticity * 0.5f) {
    // I am much more crystallized—maybe overconfident?
}
```

### 2. Maturity-Aware Routing

Route queries to more mature systems:

```c
if (theirs.maturity > mine.maturity) {
    // They have more developed knowledge
    forward_query(them);
}
```

### 3. Developmental Stage Matching

Only calibrate with peers at similar stages:

```c
float stage_diff = fabsf(mine.age - theirs.age);
if (stage_diff < 100.0f) {
    // Similar developmental stage—calibrate
}
```

### 4. Drift Detection

Monitor velocity divergence over time:

```c
float velocity_drift = mine.velocity - theirs.velocity;
if (fabsf(velocity_drift) > threshold) {
    // Our development trajectories are diverging
}
```

## Transport

MIND does not specify transport. Options:

- HTTP/REST
- gRPC
- Raw TCP
- Message queues
- Shared memory

The signal is 20 bytes. Any transport works.

## Authentication

MIND does not handle authentication.

**Recommendation:** Authenticate at the transport layer.

Unauthenticated calibration signals should be treated as untrusted.

## Frequency

Calibration can occur:

- Periodically (e.g., every N updates)
- On significant events (e.g., maturity milestone)
- On demand

There is no required frequency. Calibration is optional.

## What Calibration Does NOT Do

- Synchronize memory
- Transfer beliefs
- Create consensus
- Merge states
- Align invariants

Each system remains fully independent.

## Threat Model

### Malicious Calibration

An adversary could send false signals to:
- Make you think you're under-confident
- Make you think you're immature
- Skew your self-perception

**Mitigation:**
- Authenticate peers
- Use multiple peers (median filter)
- Weight by trust

### Inference Attacks

Calibration signals leak meta-information:
- How much experience you have
- How confident you are
- Your learning velocity

**Mitigation:**
- Only share with trusted peers
- Consider adding noise (violates determinism)

## Implementation Example

### Sender

```c
void send_calibration(int socket, cr_state_t* st) {
    cr_calibration_t cal;
    cr_state_calibration(st, &cal);
    send(socket, &cal, sizeof(cal), 0);
}
```

### Receiver

```c
void receive_calibration(int socket, cr_calibration_t* cal) {
    recv(socket, cal, sizeof(*cal), 0);
}
```

### Comparison

```c
void compare_calibration(
    cr_calibration_t* mine,
    cr_calibration_t* theirs
) {
    printf("Age ratio: %.2f\n", mine->age / theirs->age);
    printf("Maturity delta: %.2f\n", mine->maturity - theirs->maturity);
    printf("Velocity delta: %.6f\n", mine->velocity - theirs->velocity);
}
```

## Future Extensions

Possible extensions (not in v0.1):

- Signed calibration signals
- Calibration history
- Multi-peer aggregation
- Confidence intervals

These would be specified in future protocol versions.

## Design Philosophy

S2S calibration embodies the MIND philosophy:

> Systems that remain independent, yet do not drift incoherently.

Independence is preserved. Coherence emerges through observation, not control.
