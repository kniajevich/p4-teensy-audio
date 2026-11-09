#ifndef ARM_MATH_MOCK_H
#define ARM_MATH_MOCK_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

// Mock __SSAT (Signed Saturate)
static inline int32_t __SSAT(int32_t val, uint32_t sat) {
    if (sat >= 32) return val;
    int32_t max = (1 << (sat - 1)) - 1;
    int32_t min = -(1 << (sat - 1));
    if (val > max) return max;
    if (val < min) return min;
    return val;
}

// Mock __SMLALD (Signed Multiply Accumulate Long Dual)
static inline int64_t __SMLALD(uint32_t val1, uint32_t val2, int64_t acc) {
    int16_t v1_l = (int16_t)(val1 & 0xFFFF);
    int16_t v1_h = (int16_t)((val1 >> 16) & 0xFFFF);
    int16_t v2_l = (int16_t)(val2 & 0xFFFF);
    int16_t v2_h = (int16_t)((val2 >> 16) & 0xFFFF);
    return acc + ((int64_t)v1_l * v2_l) + ((int64_t)v1_h * v2_h);
}

#ifdef __cplusplus
}
#endif

#endif // ARM_MATH_MOCK_H
