/*
 * Copyright 2026 The MIND Contributors
 * Licensed under the Apache License, Version 2.0
 */

/**
 * @file mind_vec.c
 * @brief Foundation Layer: Vector Operations
 *
 * Pure math. No semantics. Never changes.
 */

#include <math.h>
#include "mind_vec.h"

float mind_vec_dot(const float* a, const float* b, int dim) {
    float sum = 0.0f;
    for (int i = 0; i < dim; i++) {
        sum += a[i] * b[i];
    }
    return sum;
}

float mind_vec_norm(const float* v, int dim) {
    return sqrtf(mind_vec_dot(v, v, dim));
}

float mind_vec_cosine(const float* a, const float* b, int dim) {
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

void mind_vec_lerp(const float* a, const float* b, float t, float* out, int dim) {
    float one_minus_t = 1.0f - t;
    for (int i = 0; i < dim; i++) {
        out[i] = a[i] * one_minus_t + b[i] * t;
    }
}
