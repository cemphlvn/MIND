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
 * @file cr_persist.c
 * @brief State persistence (save/load)
 *
 * Persistence format:
 *
 * Header (16 bytes):
 *   - magic: uint32 (0x4D494E44 = "MIND")
 *   - version: uint32 (1)
 *   - dim: int32
 *   - max_slots: int32
 *
 * State (variable):
 *   - slot_count: int32
 *   - plasticity: float32
 *   - age: float32
 *   - plasticity_prev: float32
 *   - velocity: float32
 *   - last_reinforcement_age: float32
 *   - total_updates: int32
 *   - total_reinforcements: int32
 *
 * Slots (slot_count × (dim × 4 + 4) bytes):
 *   For each occupied slot:
 *     - vector: float32[dim]
 *     - weight: float32
 *
 * Note: This format is not optimized for size. Future versions may
 * add compression or more efficient encoding while maintaining
 * backwards compatibility.
 */

#include <stdio.h>
#include <stdint.h>
#include "cr.h"
#include "cr_internal.h"

/**
 * @brief Save state to file
 */
int cr_state_save(cr_state_t* st, const char* path) {
    if (!st || !path) {
        return -1;
    }

    FILE* f = fopen(path, "wb");
    if (!f) {
        return -1;
    }

    /* Write header */
    uint32_t magic = CR_MAGIC;
    uint32_t version = CR_PERSIST_VERSION;
    int32_t dim = st->rt->dim;
    int32_t max_slots = st->rt->max_slots;

    if (fwrite(&magic, sizeof(magic), 1, f) != 1) goto error;
    if (fwrite(&version, sizeof(version), 1, f) != 1) goto error;
    if (fwrite(&dim, sizeof(dim), 1, f) != 1) goto error;
    if (fwrite(&max_slots, sizeof(max_slots), 1, f) != 1) goto error;

    /* Write state */
    int32_t slot_count = st->slot_count;
    int32_t total_updates = st->total_updates;
    int32_t total_reinforcements = st->total_reinforcements;

    if (fwrite(&slot_count, sizeof(slot_count), 1, f) != 1) goto error;
    if (fwrite(&st->plasticity, sizeof(float), 1, f) != 1) goto error;
    if (fwrite(&st->age, sizeof(float), 1, f) != 1) goto error;
    if (fwrite(&st->plasticity_prev, sizeof(float), 1, f) != 1) goto error;
    if (fwrite(&st->velocity, sizeof(float), 1, f) != 1) goto error;
    if (fwrite(&st->last_reinforcement_age, sizeof(float), 1, f) != 1) goto error;
    if (fwrite(&total_updates, sizeof(total_updates), 1, f) != 1) goto error;
    if (fwrite(&total_reinforcements, sizeof(total_reinforcements), 1, f) != 1) goto error;

    /* Write occupied slots */
    for (int i = 0; i < st->slot_count; i++) {
        if (fwrite(st->slots[i].vector, sizeof(float), dim, f) != (size_t)dim) goto error;
        if (fwrite(&st->slots[i].weight, sizeof(float), 1, f) != 1) goto error;
    }

    fclose(f);
    return 0;

error:
    fclose(f);
    return -1;
}

/**
 * @brief Load state from file
 */
int cr_state_load(cr_state_t* st, const char* path) {
    if (!st || !path) {
        return -1;
    }

    FILE* f = fopen(path, "rb");
    if (!f) {
        return -1;
    }

    /* Read and verify header */
    uint32_t magic, version;
    int32_t dim, max_slots;

    if (fread(&magic, sizeof(magic), 1, f) != 1) goto error;
    if (fread(&version, sizeof(version), 1, f) != 1) goto error;
    if (fread(&dim, sizeof(dim), 1, f) != 1) goto error;
    if (fread(&max_slots, sizeof(max_slots), 1, f) != 1) goto error;

    /* Validate header */
    if (magic != CR_MAGIC) {
        goto error;  /* Not a MIND state file */
    }
    if (version != CR_PERSIST_VERSION) {
        goto error;  /* Incompatible version */
    }
    if (dim != st->rt->dim || max_slots != st->rt->max_slots) {
        goto error;  /* Configuration mismatch */
    }

    /* Read state */
    int32_t slot_count, total_updates, total_reinforcements;

    if (fread(&slot_count, sizeof(slot_count), 1, f) != 1) goto error;
    if (fread(&st->plasticity, sizeof(float), 1, f) != 1) goto error;
    if (fread(&st->age, sizeof(float), 1, f) != 1) goto error;
    if (fread(&st->plasticity_prev, sizeof(float), 1, f) != 1) goto error;
    if (fread(&st->velocity, sizeof(float), 1, f) != 1) goto error;
    if (fread(&st->last_reinforcement_age, sizeof(float), 1, f) != 1) goto error;
    if (fread(&total_updates, sizeof(total_updates), 1, f) != 1) goto error;
    if (fread(&total_reinforcements, sizeof(total_reinforcements), 1, f) != 1) goto error;

    st->slot_count = slot_count;
    st->total_updates = total_updates;
    st->total_reinforcements = total_reinforcements;

    /* Clear existing slots */
    for (int i = 0; i < st->rt->max_slots; i++) {
        st->slots[i].weight = 0.0f;
    }

    /* Read slots */
    for (int i = 0; i < slot_count; i++) {
        if (fread(st->slots[i].vector, sizeof(float), dim, f) != (size_t)dim) goto error;
        if (fread(&st->slots[i].weight, sizeof(float), 1, f) != 1) goto error;
    }

    fclose(f);
    return 0;

error:
    fclose(f);
    return -1;
}
