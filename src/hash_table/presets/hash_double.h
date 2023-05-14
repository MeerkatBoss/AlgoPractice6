#include <math.h>

__always_inline
static int double_equal(double a, double b)
{
    const double eps = 1e-6;
    if (isnan(a) && isnan(b))
        return 1;
    if (a == b) return 1; 

    double m = fmax(fabs(a), fabs(b));
    double diff = m < eps ? 0.0 : fabs(a - b) / m;

    return diff < eps;
}

#define HASH_TABLE_KEY_DOUBLE

#define KEY_TYPE double
#define KEY_COPY(key) key
#define KEY_DTOR(key)
#define KEY_EQUAL(a, b) double_equal(a, b)

#ifndef HASH_FUNCTION
#define HASH_FUNCTION(key) hash_double_reinterpret(key)
#endif
