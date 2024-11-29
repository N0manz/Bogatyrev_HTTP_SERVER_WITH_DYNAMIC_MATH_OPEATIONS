#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include "math_operations.h"
#include "server_operations.h"
#include "X.h"
#include <errno.h>
#include <string.h>

int validate_number(char *num_str, long *num) {
    char *endptr;
    errno = 0;
    *num = strtol(num_str, &endptr, 10);
    if ((errno == ERANGE) || *endptr != '\0') { 
        return -1;
    }
    return 0;
}

double process_operation(char operation, long num1, long num2, int *error) {
    double result = 0;
    switch (operation) {
        case '+':
            result = add(num1, num2, error);
            break;
        case '-':
            result = subtract(num1, num2, error);
            break;
        case '*':
            result = multiply(num1, num2, error);
            break;
        case '/':
            result = divide(num1, num2, error);
            break;
        case 'l':
            result = logarithm(num1, num2, error);
            break;
        default:
            *error = 1;
    }
    return result;
}
