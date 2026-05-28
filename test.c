#include <stdio.h>
#include "calc_lib.h"

int main()
{
    printf("3+5 = %d\n", calc_add(3,5));
    printf("9*2 = %d\n", calc_multiply(9,2));
    return 0;
}
