/*
 * Copyright 2026 The MIND Contributors
 * Licensed under the Apache License, Version 2.0
 */

/**
 * @file mind_vec.h
 * @brief Foundation Layer: Vector Operations
 *
 * Pure math. No semantics. Never changes.
 */

#ifndef MIND_VEC_H
#define MIND_VEC_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Cosine similarity between two vectors
 *
 * @param a First vector
 * @param b Second vector
 * @param dim Dimension
 * @return Similarity in [-1, 1], or 0 if either vector is zero
 */
float mind_vec_cosine(const float* a, const float* b, int dim);

/**
 * @brief Dot product of two vectors
 */
float mind_vec_dot(const float* a, const float* b, int dim);

/**
 * @brief L2 norm of a vector
 */
float mind_vec_norm(const float* v, int dim);

/**
 * @brief Interpolate between two vectors
 *
 * result = a * (1 - t) + b * t
 *
 * @param a First vector
 * @param b Second vector
 * @param t Interpolation factor in [0, 1]
 * @param out Output vector (must be pre-allocated)
 * @param dim Dimension
 */
void mind_vec_lerp(const float* a, const float* b, float t, float* out, int dim);

#ifdef __cplusplus
}
#endif

#endif /* MIND_VEC_H */
