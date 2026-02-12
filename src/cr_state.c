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
 * @file cr_state.c
 * @brief State lifecycle and experience processing
 *
 * This file contains the core learning logic.
 */

#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "cr.h"
#include "cr_internal.h"

/*============================================================================
 * Utility Functions
 *============================================================================*/

/**
 * @brief Compute cosine similarity between two vectors
 *
 * Cosine similarity measures the angle between vectors, ignoring magnitude.
 * This is appropriate because we care about shape, not scale.
 *
 * @param a First vector
 * @param b Second vector
 * @param dim Dimension
 * @return Similarity in [-1, 1], or 0 if either vector is zero
 */
float cr_cosine_similarity(const float* a, const float* b, int dim) {
    float dot = 0.0f;
    float norm_a = 0.0f;
    float norm_b = 0.0f;

    for (int i = 0; i < dim; i++) {
        dot += a[i] * b[i];
        norm_a += a[i] * a[i];
        norm_b += b[i] * b[i];
    }

    if (norm_a == 0.0f || norm_b == 0.0f) {
        return 0.0f;
    }

    return dot / (sqrtf(norm_a) * sqrtf(norm_b));
}

/*============================================================================
 * State Lifecycle
 *============================================================================*/

cr_state_t* cr_state_create(cr_runtime_t* rt) {
    if (!rt) {
        return NULL;
    }

    cr_state_t* st = calloc(1, sizeof(*st));
    if (!st) {
        return NULL;
    }

    st->rt = rt;

    /* Initialize epistemic state */
    st->plasticity = 1.0f;
    st->plasticity_prev = 1.0f;
    st->velocity = 0.0f;
    st->age = 0.0f;
    st->last_reinforcement_age = 0.0f;
    st->slot_count = 0;
    st->total_updates = 0;
    st->total_reinforcements = 0;

    /* Allocate memory slots */
    st->slots = calloc(rt->max_slots, sizeof(cr_slot_t));
    if (!st->slots) {
        free(st);
        return NULL;
    }

    /* Allocate vectors for each slot */
    for (int i = 0; i < rt->max_slots; i++) {
        st->slots[i].vector = calloc(rt->dim, sizeof(float));
        if (!st->slots[i].vector) {
            /* Cleanup on failure */
            for (int j = 0; j < i; j++) {
                free(st->slots[j].vector);
            }
            free(st->slots);
            free(st);
            return NULL;
        }
        st->slots[i].weight = 0.0f;
    }

    return st;
}

void cr_state_reset(cr_state_t* st) {
    if (!st) {
        return;
    }

    /* Reset epistemic state */
    st->plasticity = 1.0f;
    st->plasticity_prev = 1.0f;
    st->velocity = 0.0f;
    st->age = 0.0f;
    st->last_reinforcement_age = 0.0f;
    st->slot_count = 0;
    st->total_updates = 0;
    st->total_reinforcements = 0;

    /* Clear memory slots */
    for (int i = 0; i < st->rt->max_slots; i++) {
        memset(st->slots[i].vector, 0, st->rt->dim * sizeof(float));
        st->slots[i].weight = 0.0f;
    }
}

void cr_state_destroy(cr_state_t* st) {
    if (!st) {
        return;
    }

    /* Free slot vectors */
    if (st->slots) {
        for (int i = 0; i < st->rt->max_slots; i++) {
            free(st->slots[i].vector);
        }
        free(st->slots);
    }

    free(st);
}

int cr_state_slot_count(const cr_state_t* st) {
    if (!st) {
        return -1;
    }
    return st->slot_count;
}

/*============================================================================
 * Experience Processing
 *============================================================================*/

/**
 * @brief Update state with new experience
 *
 * This is the core learning function implementing mercy-based memory.
 *
 * Algorithm:
 * 1. Find the closest existing invariant (by cosine similarity)
 * 2. If similarity > threshold: REINFORCE
 *    - Interpolate toward new pattern (weighted by plasticity)
 *    - Increase weight
 *    - Decay plasticity (crystallization)
 * 3. Else if space available: CREATE
 *    - Store as new invariant
 *    - Recovery plasticity slightly (preserve openness)
 * 4. Else: IGNORE (memory full)
 * 5. Advance age by delta_t
 * 6. Compute velocity (rate of plasticity change)
 *
 * Properties guaranteed:
 * - Plasticity never drops below CR_EPSILON (mercy)
 * - Memory never exceeds max_slots (bounded)
 * - Result is deterministic (no randomness)
 */
int cr_state_update(
    cr_state_t* st,
    const float* embedding,
    int dim,
    float delta_t
) {
    /* Validate inputs */
    if (!st || !embedding) {
        return -1;
    }
    if (dim != st->rt->dim) {
        return -1;
    }
    if (delta_t <= 0.0f) {
        return -1;
    }

    /* Store previous plasticity for velocity calculation */
    st->plasticity_prev = st->plasticity;

    /* Find closest existing invariant */
    cr_slot_t* best = NULL;
    float best_sim = 0.0f;

    for (int i = 0; i < st->slot_count; i++) {
        float sim = cr_cosine_similarity(
            embedding,
            st->slots[i].vector,
            dim
        );
        if (sim > best_sim) {
            best_sim = sim;
            best = &st->slots[i];
        }
    }

    int reinforced = 0;

    if (best && best_sim > CR_SIM_THRESHOLD) {
        /*
         * REINFORCE existing invariant
         *
         * Interpolation formula:
         *   new = old × (1 - plasticity) + input × plasticity
         *
         * When plastic (=1.0): new = input (full adoption)
         * When stable (=ε):    new ≈ old (minimal change)
         */
        for (int i = 0; i < dim; i++) {
            best->vector[i] =
                best->vector[i] * (1.0f - st->plasticity) +
                embedding[i] * st->plasticity;
        }
        best->weight += 1.0f;
        reinforced = 1;

        /* Track temporal landmark */
        st->last_reinforcement_age = st->age + delta_t;
        st->total_reinforcements++;
    }
    else if (st->slot_count < st->rt->max_slots) {
        /*
         * CREATE new invariant
         *
         * Store the embedding as a new pattern.
         */
        cr_slot_t* slot = &st->slots[st->slot_count];
        memcpy(slot->vector, embedding, sizeof(float) * dim);
        slot->weight = 1.0f;
        st->slot_count++;
    }
    /* else: memory full, experience silently ignored (bounded) */

    /*
     * Plasticity update
     *
     * Reinforcement → crystallization (decay)
     * Novelty → preserved openness (recovery)
     *
     * This implements "mercy by construction":
     * - Early inputs have limited impact (high plasticity dampens commitment)
     * - Repetition is required for confidence
     */
    if (reinforced) {
        st->plasticity *= CR_DECAY_RATE;
    } else {
        st->plasticity *= CR_RECOVERY_RATE;
    }

    /* Clamp with mercy floor */
    if (st->plasticity < CR_EPSILON) {
        st->plasticity = CR_EPSILON;
    }
    if (st->plasticity > 1.0f) {
        st->plasticity = 1.0f;
    }

    /* Time accumulation (continuous, not discrete) */
    st->age += delta_t;
    st->total_updates++;

    /*
     * Velocity: rate of crystallization
     *
     * Positive = crystallizing (plasticity decreasing)
     * Negative = softening (plasticity increasing)
     * Zero = stable
     */
    st->velocity = (st->plasticity_prev - st->plasticity) / delta_t;

    return 0;
}
