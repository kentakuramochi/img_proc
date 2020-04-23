///
/// @file   util.h
/// @brief  utilities
///

#ifndef UTIL_H
#define UTIL_H

#include <stdint.h>

///
/// @def    M_PI
/// @brief  pi constant used for calculation
///
#define M_PI 3.14159265358979

///
/// @fn     cmp_ascend
/// @brief  compare function for qsort() with ascending order
/// @param  a   [in]    value0 to be sorted
/// @param  b   [in]    value1 to be sorted
/// @return positive if a > b, negative if a < b, 0 otherwise
///
static int cmp_ascend(const void *a, const void *b)
{
    return *(uint8_t*)a - *(uint8_t*)b;
}

#endif // UTIL_H
