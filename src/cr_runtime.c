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
 * @file cr_runtime.c
 * @brief Runtime lifecycle and configuration
 */

#include <stdlib.h>
#include "cr.h"
#include "cr_internal.h"

/*============================================================================
 * Version
 *============================================================================*/

const char* cr_version(void) {
    return CR_VERSION_STRING;
}

/*============================================================================
 * Runtime Lifecycle
 *============================================================================*/

cr_runtime_t* cr_runtime_create(const cr_config_t* cfg) {
    if (!cfg) {
        return NULL;
    }

    if (cfg->embedding_dim <= 0 || cfg->max_memory_slots <= 0) {
        return NULL;
    }

    cr_runtime_t* rt = calloc(1, sizeof(*rt));
    if (!rt) {
        return NULL;
    }

    rt->dim = cfg->embedding_dim;
    rt->max_slots = cfg->max_memory_slots;

    return rt;
}

void cr_runtime_destroy(cr_runtime_t* rt) {
    if (!rt) {
        return;
    }
    free(rt);
}

int cr_runtime_config(const cr_runtime_t* rt, cr_config_t* out) {
    if (!rt || !out) {
        return -1;
    }

    out->embedding_dim = rt->dim;
    out->max_memory_slots = rt->max_slots;
    out->initial_plasticity = 1.0f;  /* Default */

    return 0;
}
