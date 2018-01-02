#include "library.h"
#include <stdio.h>
#include <stdarg.h>

int amount2(int n, ...) {
    va_list vlist;
    va_start(vlist, n);
    int value;
    int amount = 0;
    for(int i = 0; i < n; ++i) {
        value = va_arg(vlist, int);
        if(value % 2 ==0)
            ++amount;
    }
    va_end(vlist);
    return amount;
}