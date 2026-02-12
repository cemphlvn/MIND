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
 * @file test_basic.c
 * @brief Basic tests for MIND
 *
 * These tests verify core invariants.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "cr.h"

#define ASSERT(cond, msg) do { \
    if (!(cond)) { \
        fprintf(stderr, "FAIL: %s\n  at %s:%d\n", msg, __FILE__, __LINE__); \
        return 1; \
    } \
} while(0)

#define PASS(name) printf("PASS: %s\n", name)

/*============================================================================
 * Test: Lifecycle
 *============================================================================*/

static int test_lifecycle(void) {
    cr_config_t cfg = {4, 8, 1.0f};

    cr_runtime_t* rt = cr_runtime_create(&cfg);
    ASSERT(rt != NULL, "runtime creation");

    cr_state_t* st = cr_state_create(rt);
    ASSERT(st != NULL, "state creation");

    cr_state_destroy(st);
    cr_runtime_destroy(rt);

    PASS("lifecycle");
    return 0;
}

/*============================================================================
 * Test: Plasticity bounds (mercy)
 *============================================================================*/

static int test_plasticity_bounds(void) {
    cr_config_t cfg = {4, 8, 1.0f};
    cr_runtime_t* rt = cr_runtime_create(&cfg);
    cr_state_t* st = cr_state_create(rt);

    float pattern[4] = {1.0f, 0.0f, 0.0f, 0.0f};

    /* Run many updates to try to force plasticity to zero */
    for (int i = 0; i < 10000; i++) {
        cr_state_update(st, pattern, 4, 1.0f);
    }

    cr_plasticity_t p;
    cr_state_plasticity(st, &p);

    /* Plasticity must stay above epsilon (0.05) */
    ASSERT(p.plasticity >= 0.05f, "plasticity floor (mercy)");
    ASSERT(p.plasticity <= 1.0f, "plasticity ceiling");

    cr_state_destroy(st);
    cr_runtime_destroy(rt);

    PASS("plasticity_bounds");
    return 0;
}

/*============================================================================
 * Test: Determinism
 *============================================================================*/

static int test_determinism(void) {
    cr_config_t cfg = {4, 16, 1.0f};

    float patterns[3][4] = {
        {1.0f, 0.0f, 0.0f, 0.0f},
        {0.0f, 1.0f, 0.0f, 0.0f},
        {0.5f, 0.5f, 0.0f, 0.0f}
    };

    /* Run twice with same inputs */
    float confidence1, confidence2;

    for (int run = 0; run < 2; run++) {
        cr_runtime_t* rt = cr_runtime_create(&cfg);
        cr_state_t* st = cr_state_create(rt);

        for (int i = 0; i < 100; i++) {
            cr_state_update(st, patterns[i % 3], 4, 1.0f);
        }

        cr_hint_t hint;
        cr_state_query(st, patterns[0], 4, &hint);

        if (run == 0) {
            confidence1 = hint.confidence;
        } else {
            confidence2 = hint.confidence;
        }

        cr_state_destroy(st);
        cr_runtime_destroy(rt);
    }

    /* Must be exactly equal (determinism) */
    ASSERT(confidence1 == confidence2, "deterministic results");

    PASS("determinism");
    return 0;
}

/*============================================================================
 * Test: Bounded memory
 *============================================================================*/

static int test_bounded_memory(void) {
    cr_config_t cfg = {4, 4, 1.0f};  /* Only 4 slots */
    cr_runtime_t* rt = cr_runtime_create(&cfg);
    cr_state_t* st = cr_state_create(rt);

    /* Try to add 10 different patterns */
    for (int i = 0; i < 10; i++) {
        float pattern[4] = {
            (float)i,
            (float)(i * 2),
            (float)(i * 3),
            (float)(i * 4)
        };
        cr_state_update(st, pattern, 4, 1.0f);
    }

    /* Slot count must not exceed max */
    int slots = cr_state_slot_count(st);
    ASSERT(slots <= 4, "bounded slot count");

    cr_state_destroy(st);
    cr_runtime_destroy(rt);

    PASS("bounded_memory");
    return 0;
}

/*============================================================================
 * Test: Age is monotonic
 *============================================================================*/

static int test_age_monotonic(void) {
    cr_config_t cfg = {4, 8, 1.0f};
    cr_runtime_t* rt = cr_runtime_create(&cfg);
    cr_state_t* st = cr_state_create(rt);

    float pattern[4] = {1.0f, 0.0f, 0.0f, 0.0f};
    float prev_age = 0.0f;

    for (int i = 0; i < 100; i++) {
        cr_state_update(st, pattern, 4, 0.5f);

        cr_temporal_t t;
        cr_state_temporal(st, &t);

        ASSERT(t.age > prev_age, "age increases");
        prev_age = t.age;
    }

    cr_state_destroy(st);
    cr_runtime_destroy(rt);

    PASS("age_monotonic");
    return 0;
}

/*============================================================================
 * Test: Persistence
 *============================================================================*/

static int test_persistence(void) {
    cr_config_t cfg = {4, 8, 1.0f};
    const char* path = "/tmp/mind_test.state";

    float pattern[4] = {1.0f, 0.0f, 0.0f, 0.0f};
    float saved_confidence, loaded_confidence;

    /* Create, update, save */
    {
        cr_runtime_t* rt = cr_runtime_create(&cfg);
        cr_state_t* st = cr_state_create(rt);

        for (int i = 0; i < 50; i++) {
            cr_state_update(st, pattern, 4, 1.0f);
        }

        cr_hint_t hint;
        cr_state_query(st, pattern, 4, &hint);
        saved_confidence = hint.confidence;

        int err = cr_state_save(st, path);
        ASSERT(err == 0, "save succeeds");

        cr_state_destroy(st);
        cr_runtime_destroy(rt);
    }

    /* Load, query */
    {
        cr_runtime_t* rt = cr_runtime_create(&cfg);
        cr_state_t* st = cr_state_create(rt);

        int err = cr_state_load(st, path);
        ASSERT(err == 0, "load succeeds");

        cr_hint_t hint;
        cr_state_query(st, pattern, 4, &hint);
        loaded_confidence = hint.confidence;

        cr_state_destroy(st);
        cr_runtime_destroy(rt);
    }

    ASSERT(saved_confidence == loaded_confidence, "persistence preserves state");

    PASS("persistence");
    return 0;
}

/*============================================================================
 * Test: S2S calibration signal
 *============================================================================*/

static int test_calibration(void) {
    cr_config_t cfg = {4, 8, 1.0f};
    cr_runtime_t* rt = cr_runtime_create(&cfg);
    cr_state_t* st = cr_state_create(rt);

    float pattern[4] = {1.0f, 0.0f, 0.0f, 0.0f};

    for (int i = 0; i < 100; i++) {
        cr_state_update(st, pattern, 4, 1.0f);
    }

    cr_calibration_t cal;
    int err = cr_state_calibration(st, &cal);

    ASSERT(err == 0, "calibration export");
    ASSERT(cal.age > 0.0f, "calibration has age");
    ASSERT(cal.plasticity > 0.0f, "calibration has plasticity");
    ASSERT(cal.maturity > 0.0f, "calibration has maturity");

    cr_state_destroy(st);
    cr_runtime_destroy(rt);

    PASS("calibration");
    return 0;
}

/*============================================================================
 * Main
 *============================================================================*/

int main(void) {
    printf("MIND Test Suite\n");
    printf("================\n\n");

    int failures = 0;

    failures += test_lifecycle();
    failures += test_plasticity_bounds();
    failures += test_determinism();
    failures += test_bounded_memory();
    failures += test_age_monotonic();
    failures += test_persistence();
    failures += test_calibration();

    printf("\n================\n");
    if (failures == 0) {
        printf("All tests passed.\n");
        return 0;
    } else {
        printf("%d test(s) failed.\n", failures);
        return 1;
    }
}
