# Integrations

Pre-built integrations with embedding providers.

## Planned Integrations

- **ollama/** — Local models via Ollama
- **openai/** — OpenAI embeddings API
- **anthropic/** — Claude embeddings (when available)
- **huggingface/** — HuggingFace models

## Integration Pattern

Each integration should:

1. Accept text input
2. Generate embedding via provider
3. Feed to MIND
4. Return hints

```
User Text → Provider → Embedding → MIND → Hints
```

## Ecosystem Design Note

For how MIND sits inside a larger continuous loop (grounding sources, inference, accelerator
choice) — and why none of it touches core — see [`ecosystem.tr.md`](ecosystem.tr.md) (Türkçe).
