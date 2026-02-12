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
 * @file cr_internal.h
 * @brief MIND Cognitive Runtime Internal Structures
 *
 * This header is internal. Do not include directly.
 */

#ifndef CR_INTERNAL_H
#define CR_INTERNAL_H

/*============================================================================
 * Constants (Frozen v0.1 Semantics)
 *============================================================================*/

/**
 * @brief Plasticity floor (mercy)
 *
 * Plasticity never drops below this value.
 * This ensures the system is always somewhat malleable.
 */
#define CR_EPSILON 0.05f

/**
 * @brief Similarity threshold for reinforcement
 *
 * If cosine similarity exceeds this, the pattern is reinforced
 * rather than stored as a new invariant.
 */
#define CR_SIM_THRESHOLD 0.85f

/**
 * @brief Plasticity decay rate (on reinforcement)
 *
 * When a pattern is reinforced, plasticity is multiplied by this.
 * < 1.0 means crystallization occurs.
 */
#define CR_DECAY_RATE 0.995f

/**
 * @brief Plasticity recovery rate (on novelty)
 *
 * When a new pattern is encountered, plasticity is multiplied by this.
 * > 1.0 means novelty preserves openness.
 */
#define CR_RECOVERY_RATE 1.0005f

/**
 * @brief Persistence magic number ("MIND" in ASCII)
 */
#define CR_MAGIC 0x4D494E44

/**
 * @brief Persistence format version
 */
#define CR_PERSIST_VERSION 1

/*============================================================================
 * Internal Structures
 *============================================================================*/

/**
 * @brief Memory slot
 *
 * Each slot holds one compressed invariant.
 */
typedef struct {
    float* vector;  /**< Invariant vector (dimension = rt->dim) */
    float weight;   /**< Reinforcement weight */
} cr_slot_t;

/**
 * @brief Runtime structure (opaque)
 *
 * Holds configuration. Does not hold state.
 */
struct cr_runtime {
    int dim;        /**< Embedding dimension */
    int max_slots;  /**< Maximum memory slots */
};

/**
 * @brief State structure (opaque)
 *
 * Holds accumulated cognitive memory.
 */
struct cr_state {
    struct cr_runtime* rt;  /**< Parent runtime */

    /* Memory */
    cr_slot_t* slots;       /**< Array of memory slots */
    int slot_count;         /**< Number of occupied slots */

    /* Core epistemic state */
    float plasticity;       /**< Current malleability in (ε, 1.0] */
    float age;              /**< Continuous experiential time */

    /* Temporal tracking */
    float plasticity_prev;          /**< Previous plasticity (for velocity) */
    float velocity;                 /**< Rate of crystallization */
    float last_reinforcement_age;   /**< Age at last reinforcement */
    int total_updates;              /**< Total update count */
    int total_reinforcements;       /**< Total reinforcement count */
};

/*============================================================================
 * Internal Functions
 *============================================================================*/

/**
 * @brief Compute cosine similarity
 *
 * @param a First vector
 * @param b Second vector
 * @param dim Dimension
 * @return Cosine similarity in [-1, 1], or 0 if either vector is zero
 */
float cr_cosine_similarity(const float* a, const float* b, int dim);

#endif /* CR_INTERNAL_H */
