# S2S Protocol Implementation

Server-to-server cognitive coevolution protocol.

## Current Status

**Layer 0-1 implemented** in core:
- Calibration signal export (`cr_state_calibration`)

**Layer 2-5 not yet implemented:**
- Probe registry
- Probe-specific responses
- Multi-peer coordination
- Federated calibration

## Architecture

```
┌──────────┐         ┌──────────┐
│  MIND A  │◄───────►│  MIND B  │
│          │ signals │          │
└──────────┘         └──────────┘
     │                     │
     └────────┬────────────┘
              │
        ┌─────▼─────┐
        │   Probe   │
        │  Registry │
        └───────────┘
```

See [docs/architecture/S2S_PROTOCOL.md](../../../docs/architecture/S2S_PROTOCOL.md) for specification.
