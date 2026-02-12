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
 * @file cr.h
 * @brief MIND Cognitive Runtime Public API
 *
 * This is the complete public interface for MIND. All other headers are
 * internal and should not be included directly.
 *
 * MIND is a deterministic runtime for accumulating experience from vector
 * embeddings. It does not generate text, reason symbolically, or depend
 * on any specific model.
 */

#ifndef CR_H
#define CR_H

#ifdef __cplusplus
extern "C" {
#endif

/*============================================================================
 * Version Information
 *============================================================================*/

#define CR_VERSION_MAJOR 0
#define CR_VERSION_MINOR 1
#define CR_VERSION_PATCH 0
#define CR_VERSION_STRING "0.1.0"

/*============================================================================
 * Types
 *============================================================================*/

/**
 * @brief Opaque runtime handle
 *
 * The runtime holds configuration and provides context for states.
 * Create with cr_runtime_create(), destroy with cr_runtime_destroy().
 */
typedef struct cr_runtime cr_runtime_t;

/**
 * @brief Opaque state handle
 *
 * A state represents accumulated cognitive memory. Multiple states can
 * share a runtime. Create with cr_state_create(), destroy with cr_state_destroy().
 */
typedef struct cr_state cr_state_t;

/**
 * @brief Floating point type used throughout
 */
typedef float cr_f32;

/**
 * @brief Runtime configuration
 */
typedef struct {
    int embedding_dim;      /**< Dimension of embedding vectors */
    int max_memory_slots;   /**< Maximum number of invariant slots */
    float initial_plasticity; /**< Starting plasticity (typically 1.0) */
} cr_config_t;

/**
 * @brief Query result (hint)
 *
 * Hints are the output of queries. They include a vector reference,
 * its dimension, and a derived confidence score.
 */
typedef struct {
    cr_f32* vector;    /**< Pointer to invariant vector (internal buffer) */
    int dim;           /**< Dimension of the vector */
    float confidence;  /**< Derived confidence in [0, 1] */
} cr_hint_t;

/**
 * @brief Basic epistemic state
 *
 * Simple view of the system's epistemic maturity.
 */
typedef struct {
    float plasticity;  /**< Current malleability in (ε, 1.0] */
    float stability;   /**< 1 - plasticity */
    float age;         /**< Continuous experiential time */
} cr_plasticity_t;

/**
 * @brief Rich temporal awareness
 *
 * Detailed view of the system's developmental state over time.
 */
typedef struct {
    float age;                      /**< Total experiential time */
    float plasticity;               /**< Current malleability */
    float velocity;                 /**< Rate of crystallization (Δplasticity/Δt) */
    float maturity;                 /**< age × stability */
    float last_reinforcement_age;   /**< Age at last pattern repetition */
    float time_since_reinforcement; /**< Age - last_reinforcement_age */
    int total_updates;              /**< Discrete update count */
    int total_reinforcements;       /**< Pattern repetition count */
} cr_temporal_t;

/**
 * @brief S2S calibration signal
 *
 * Minimal signal for server-to-server cognitive coevolution.
 * This is what gets exchanged between systems—not memory, not embeddings.
 */
typedef struct {
    float age;                 /**< Experiential time */
    float plasticity;          /**< Current malleability */
    float velocity;            /**< Rate of crystallization */
    float maturity;            /**< age × stability */
    float reinforcement_ratio; /**< reinforcements / updates */
} cr_calibration_t;

/*============================================================================
 * Lifecycle Functions
 *============================================================================*/

/**
 * @brief Create a new runtime
 *
 * @param cfg Configuration (must not be NULL)
 * @return Runtime handle, or NULL on failure
 */
cr_runtime_t* cr_runtime_create(const cr_config_t* cfg);

/**
 * @brief Destroy a runtime
 *
 * All states created from this runtime must be destroyed first.
 *
 * @param rt Runtime to destroy (may be NULL)
 */
void cr_runtime_destroy(cr_runtime_t* rt);

/**
 * @brief Create a new state
 *
 * States hold accumulated experience. Multiple states can share a runtime.
 *
 * @param rt Runtime (must not be NULL)
 * @return State handle, or NULL on failure
 */
cr_state_t* cr_state_create(cr_runtime_t* rt);

/**
 * @brief Reset state to initial condition
 *
 * Clears all memory and resets plasticity/age to initial values.
 *
 * @param st State to reset (may be NULL)
 */
void cr_state_reset(cr_state_t* st);

/**
 * @brief Destroy a state
 *
 * @param st State to destroy (may be NULL)
 */
void cr_state_destroy(cr_state_t* st);

/*============================================================================
 * Experience Functions
 *============================================================================*/

/**
 * @brief Update state with new experience
 *
 * This is the core learning function. It:
 * - Finds the closest existing invariant
 * - If similar enough, reinforces it (interpolation)
 * - Otherwise, creates a new invariant (if space available)
 * - Updates plasticity based on reinforcement
 * - Advances age by delta_t
 *
 * @param st State to update
 * @param embedding Embedding vector
 * @param dim Dimension (must match config)
 * @param delta_t Time increment (must be positive)
 * @return 0 on success, -1 on error
 */
int cr_state_update(
    cr_state_t* st,
    const cr_f32* embedding,
    int dim,
    float delta_t
);

/*============================================================================
 * Query Functions
 *============================================================================*/

/**
 * @brief Query state for a hint
 *
 * Finds the closest invariant to the query and returns a hint.
 * Confidence is derived from similarity, stability, and weight.
 *
 * @param st State to query
 * @param query Query embedding
 * @param dim Dimension (must match config)
 * @param out_hint Output hint (must not be NULL)
 * @return 0 on success, -1 on error
 */
int cr_state_query(
    cr_state_t* st,
    const cr_f32* query,
    int dim,
    cr_hint_t* out_hint
);

/*============================================================================
 * Epistemic State Functions
 *============================================================================*/

/**
 * @brief Get basic epistemic state
 *
 * @param st State to query
 * @param out Output structure (must not be NULL)
 * @return 0 on success, -1 on error
 */
int cr_state_plasticity(
    const cr_state_t* st,
    cr_plasticity_t* out
);

/**
 * @brief Get rich temporal awareness
 *
 * Provides detailed information about developmental state.
 *
 * @param st State to query
 * @param out Output structure (must not be NULL)
 * @return 0 on success, -1 on error
 */
int cr_state_temporal(
    const cr_state_t* st,
    cr_temporal_t* out
);

/**
 * @brief Get S2S calibration signal
 *
 * Returns the minimal signal needed for server-to-server calibration.
 * This signal contains no memory content, only meta-cognitive state.
 *
 * @param st State to query
 * @param out Output structure (must not be NULL)
 * @return 0 on success, -1 on error
 */
int cr_state_calibration(
    const cr_state_t* st,
    cr_calibration_t* out
);

/*============================================================================
 * Persistence Functions
 *============================================================================*/

/**
 * @brief Save state to file
 *
 * Writes the complete state to a binary file. The file format includes
 * a magic number and version for validation during load.
 *
 * @param st State to save
 * @param path File path
 * @return 0 on success, -1 on error
 */
int cr_state_save(cr_state_t* st, const char* path);

/**
 * @brief Load state from file
 *
 * Loads state from a previously saved file. The state must have been
 * created with matching configuration (dimension, max_slots).
 *
 * @param st State to load into
 * @param path File path
 * @return 0 on success, -1 on error
 */
int cr_state_load(cr_state_t* st, const char* path);

/*============================================================================
 * Utility Functions
 *============================================================================*/

/**
 * @brief Get version string
 *
 * @return Version string (e.g., "0.1.0")
 */
const char* cr_version(void);

/**
 * @brief Get runtime configuration
 *
 * @param rt Runtime
 * @param out Output configuration (must not be NULL)
 * @return 0 on success, -1 on error
 */
int cr_runtime_config(const cr_runtime_t* rt, cr_config_t* out);

/**
 * @brief Get current slot count
 *
 * @param st State
 * @return Number of occupied slots, or -1 on error
 */
int cr_state_slot_count(const cr_state_t* st);

#ifdef __cplusplus
}
#endif

#endif /* CR_H */
