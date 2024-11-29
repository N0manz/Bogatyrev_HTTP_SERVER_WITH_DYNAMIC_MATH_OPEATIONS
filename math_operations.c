#include "math_operations.h"
#include "math.h"
#include "X.h"
#include <limits.h>

double add(long num1, long num2, int *error) {
    if ((num1 > 0 && num2 > 0 && num1 > LONG_MAX - num2) || 
        (num1 < 0 && num2 < 0 && num1 < LONG_MIN - num2)) {
        *error = 1;
        return 0;
    }
    *error = 0;
    return num1 + num2;
}

double subtract(long num1, long num2, int *error) {
    if ((num1 > 0 && num2 < 0 && num1 > LONG_MAX + num2) ||
        (num1 < 0 && num2 > 0 && num1 < LONG_MIN + num2)) {
        *error = 1; 
        return 0;
    }
    *error = 0;
    return num1 - num2;
}

double multiply(long num1, long num2, int *error) {
    if (num1 > 0 && num2 > 0 && num1 > LONG_MAX / num2) {
        *error = 1;
        return 0;
    }
    if (num1 > 0 && num2 < 0 && num2 < LONG_MIN / num1) {
        *error = 1;
        return 0;
    }
    if (num1 < 0 && num2 > 0 && num1 < LONG_MIN / num2) {
        *error = 1; 
        return 0;
    }
    if (num1 < 0 && num2 < 0 && num1 < LONG_MAX / num2) {
        *error = 1;
        return 0;
    }
    *error = 0;
    return num1 * num2;
}

double divide(long num1, long num2, int *error) {
    if (num2 == 0) {
        *error = 1; 
        return 0;
    }
    *error = 0;
    return (double)num1 / num2;
}


double logarithm(long base, long num, int *error) {
    if (base <= 0 || base == 1 || num <= 0) {
        *error = 1; 
        return 0;
    }
    *error = 0;
    return log(num) / log(base);
}


