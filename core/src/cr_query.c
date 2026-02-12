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

/**
 * @file cr_query.c
 * @brief Query and hint generation
 *
 * Queries return hints, not answers. Confidence is derived, not asserted.
 */

#include <stddef.h>
#include "cr.h"
#include "cr_internal.h"

/**
 * @brief Query state for a hint
 *
 * The query finds the closest invariant and returns a hint with derived
 * confidence. Confidence is computed from three independent signals:
 *
 * 1. Similarity: How close is the query to the invariant?
 * 2. Stability: How crystallized is the system? (1 - plasticity)
 * 3. Weight: How reinforced is this invariant? (weight / (weight + 1))
 *
 * confidence = similarity × stability × weight_factor
 *
 * This ensures:
 * - A plastic system expresses low confidence (regardless of similarity)
 * - An unreinforced invariant expresses low confidence
 * - Only stable, reinforced, similar patterns yield high confidence
 *
 * Crucially: confidence is NEVER asserted, only computed.
 */
int cr_state_query(
    cr_state_t* st,
    const float* query,
    int dim,
    cr_hint_t* out
) {
    /* Validate inputs */
    if (!st || !query || !out) {
        return -1;
    }
    if (dim != st->rt->dim) {
        return -1;
    }

    /* Find closest invariant */
    cr_slot_t* best = NULL;
    float best_sim = 0.0f;

    for (int i = 0; i < st->slot_count; i++) {
        float sim = cr_cosine_similarity(
            query,
            st->slots[i].vector,
            dim
        );
        if (sim > best_sim) {
            best_sim = sim;
            best = &st->slots[i];
        }
    }

    /* Handle empty state */
    if (!best) {
        out->vector = NULL;
        out->dim = 0;
        out->confidence = 0.0f;
        return 0;
    }

    /*
     * Confidence derivation
     *
     * stability: 1 - plasticity
     *   High plasticity → low stability → low confidence
     *   Low plasticity → high stability → higher confidence
     *
     * weight_factor: weight / (weight + 1)
     *   Converges to 1 as weight increases
     *   Starts at 0.5 for weight = 1
     *
     * Final confidence is the product of all three factors.
     */
    float stability = 1.0f - st->plasticity;
    float weight_factor = best->weight / (best->weight + 1.0f);

    out->vector = best->vector;
    out->dim = dim;
    out->confidence = best_sim * stability * weight_factor;

    return 0;
}
