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
 * @file minimal.c
 * @brief Minimal MIND example
 *
 * This example demonstrates:
 * - Creating a runtime and state
 * - Feeding experience
 * - Querying for hints
 * - Observing epistemic state
 * - Persistence
 * - S2S calibration export
 */

#include <stdio.h>
#include "cr.h"

/**
 * @brief Print a separator line
 */
static void separator(const char* title) {
    printf("\n--- %s ---\n", title);
}

int main(void) {
    printf("MIND Cognitive Runtime v%s\n", cr_version());
    printf("=========================================\n");

    /*========================================================================
     * Setup
     *========================================================================*/

    separator("Configuration");

    cr_config_t cfg = {
        .embedding_dim = 4,          /* Small for demo */
        .max_memory_slots = 8,
        .initial_plasticity = 1.0f
    };

    printf("Dimension:  %d\n", cfg.embedding_dim);
    printf("Max slots:  %d\n", cfg.max_memory_slots);

    cr_runtime_t* rt = cr_runtime_create(&cfg);
    if (!rt) {
        fprintf(stderr, "Failed to create runtime\n");
        return 1;
    }

    cr_state_t* st = cr_state_create(rt);
    if (!st) {
        fprintf(stderr, "Failed to create state\n");
        cr_runtime_destroy(rt);
        return 1;
    }

    /*========================================================================
     * Experience: Feed embeddings
     *========================================================================*/

    separator("Feeding Experience");

    /* Two patterns: similar (will be merged) */
    float pattern_a[4] = {1.0f, 0.0f, 0.0f, 0.0f};
    float pattern_b[4] = {0.98f, 0.1f, 0.0f, 0.0f};

    /* Different pattern (will be stored separately) */
    float pattern_c[4] = {0.0f, 1.0f, 0.0f, 0.0f};

    /* Feed experiences over "time" */
    for (int i = 0; i < 20; i++) {
        /* Alternate between patterns, with A appearing more often */
        if (i % 4 == 0) {
            cr_state_update(st, pattern_c, 4, 1.0f);
        } else if (i % 2 == 0) {
            cr_state_update(st, pattern_b, 4, 1.0f);
        } else {
            cr_state_update(st, pattern_a, 4, 1.0f);
        }

        /* Progress report every 5 updates */
        if ((i + 1) % 5 == 0) {
            cr_temporal_t t;
            cr_state_temporal(st, &t);
            printf("Update %2d: slots=%d  plasticity=%.4f  maturity=%.2f\n",
                   i + 1, cr_state_slot_count(st), t.plasticity, t.maturity);
        }
    }

    /*========================================================================
     * Query: Get hints
     *========================================================================*/

    separator("Querying");

    cr_hint_t hint;

    /* Query with pattern A */
    cr_state_query(st, pattern_a, 4, &hint);
    printf("Query(A): confidence=%.4f\n", hint.confidence);

    /* Query with pattern C */
    cr_state_query(st, pattern_c, 4, &hint);
    printf("Query(C): confidence=%.4f\n", hint.confidence);

    /* Query with unknown pattern */
    float unknown[4] = {0.5f, 0.5f, 0.5f, 0.5f};
    cr_state_query(st, unknown, 4, &hint);
    printf("Query(?): confidence=%.4f\n", hint.confidence);

    /*========================================================================
     * Epistemic State: Observe maturity
     *========================================================================*/

    separator("Epistemic State");

    cr_temporal_t temporal;
    cr_state_temporal(st, &temporal);

    printf("Age:                      %.2f\n", temporal.age);
    printf("Plasticity:               %.4f\n", temporal.plasticity);
    printf("Stability:                %.4f\n", 1.0f - temporal.plasticity);
    printf("Velocity:                 %.6f\n", temporal.velocity);
    printf("Maturity:                 %.2f\n", temporal.maturity);
    printf("Last reinforcement at:    %.2f\n", temporal.last_reinforcement_age);
    printf("Time since reinforcement: %.2f\n", temporal.time_since_reinforcement);
    printf("Total updates:            %d\n", temporal.total_updates);
    printf("Total reinforcements:     %d\n", temporal.total_reinforcements);

    /*========================================================================
     * S2S Calibration: Export signal
     *========================================================================*/

    separator("S2S Calibration Signal");

    cr_calibration_t cal;
    cr_state_calibration(st, &cal);

    printf("Age:                %.2f\n", cal.age);
    printf("Plasticity:         %.4f\n", cal.plasticity);
    printf("Velocity:           %.6f\n", cal.velocity);
    printf("Maturity:           %.2f\n", cal.maturity);
    printf("Reinforcement ratio: %.4f\n", cal.reinforcement_ratio);

    printf("\n(This signal can be exchanged with other MIND instances\n");
    printf(" for calibration—no memory or embeddings are shared.)\n");

    /*========================================================================
     * Persistence: Save and load
     *========================================================================*/

    separator("Persistence");

    const char* state_path = "/tmp/mind_example.state";

    printf("Saving to: %s\n", state_path);
    if (cr_state_save(st, state_path) != 0) {
        fprintf(stderr, "Failed to save state\n");
    } else {
        printf("Saved successfully.\n");
    }

    /* Reset and reload */
    printf("Resetting state...\n");
    cr_state_reset(st);

    cr_state_temporal(st, &temporal);
    printf("After reset: age=%.2f, slots=%d\n", temporal.age, cr_state_slot_count(st));

    printf("Loading from: %s\n", state_path);
    if (cr_state_load(st, state_path) != 0) {
        fprintf(stderr, "Failed to load state\n");
    } else {
        cr_state_temporal(st, &temporal);
        printf("After load: age=%.2f, slots=%d, maturity=%.2f\n",
               temporal.age, cr_state_slot_count(st), temporal.maturity);
    }

    /* Verify query still works */
    cr_state_query(st, pattern_a, 4, &hint);
    printf("Query(A) after reload: confidence=%.4f\n", hint.confidence);

    /*========================================================================
     * Cleanup
     *========================================================================*/

    separator("Cleanup");

    cr_state_destroy(st);
    cr_runtime_destroy(rt);

    printf("Done.\n");

    return 0;
}
