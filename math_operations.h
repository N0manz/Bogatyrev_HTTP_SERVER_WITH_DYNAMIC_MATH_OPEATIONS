#pragma once
#define MATH_OPERATIONS_H

double add(long num1, long num2);
double subtract(long num1, long num2);
double multiply(long num1, long num2);
double divide(long num1, long num2, int *error);
double logarithm(long base, long num, int *error);
double power(long base, long exp);
