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
 * @file cr_temporal.c
 * @brief Temporal awareness and S2S calibration
 *
 * This file provides time understanding for server-to-server coevolution.
 */

#include "cr.h"
#include "cr_internal.h"

/**
 * @brief Get basic epistemic state
 */
int cr_state_plasticity(
    const cr_state_t* st,
    cr_plasticity_t* out
) {
    if (!st || !out) {
        return -1;
    }

    out->plasticity = st->plasticity;
    out->stability = 1.0f - st->plasticity;
    out->age = st->age;

    return 0;
}

/**
 * @brief Get rich temporal awareness
 *
 * Provides complete developmental state information:
 * - Age: total experiential time
 * - Plasticity: current malleability
 * - Velocity: rate of crystallization (positive = crystallizing)
 * - Maturity: age × stability (compound metric)
 * - Temporal landmarks: when reinforcement occurred
 * - Event counts: discrete update/reinforcement counts
 */
int cr_state_temporal(
    const cr_state_t* st,
    cr_temporal_t* out
) {
    if (!st || !out) {
        return -1;
    }

    float stability = 1.0f - st->plasticity;

    out->age = st->age;
    out->plasticity = st->plasticity;
    out->velocity = st->velocity;
    out->maturity = st->age * stability;
    out->last_reinforcement_age = st->last_reinforcement_age;
    out->time_since_reinforcement = st->age - st->last_reinforcement_age;
    out->total_updates = st->total_updates;
    out->total_reinforcements = st->total_reinforcements;

    return 0;
}

/**
 * @brief Get S2S calibration signal
 *
 * This is the minimal signal for server-to-server cognitive coevolution.
 * The signal contains NO memory content, NO embeddings—only meta-cognitive
 * state about developmental maturity.
 *
 * What this enables:
 * - Systems can compare relative age
 * - Systems can compare relative maturity
 * - Systems can compare crystallization velocity
 * - Systems can understand if their confidence is unusual
 *
 * What this does NOT enable:
 * - Transfer of beliefs
 * - Synchronization of memory
 * - Consensus on any fact
 *
 * The goal is CALIBRATION, not CONVERGENCE.
 */
int cr_state_calibration(
    const cr_state_t* st,
    cr_calibration_t* out
) {
    if (!st || !out) {
        return -1;
    }

    float stability = 1.0f - st->plasticity;

    out->age = st->age;
    out->plasticity = st->plasticity;
    out->velocity = st->velocity;
    out->maturity = st->age * stability;

    /* Reinforcement ratio: repetition vs novelty */
    if (st->total_updates > 0) {
        out->reinforcement_ratio =
            (float)st->total_reinforcements / (float)st->total_updates;
    } else {
        out->reinforcement_ratio = 0.0f;
    }

    return 0;
}
