/* test_calc.c */
#include <stdio.h>
#include <stdlib.h>
#include <minix/calc.h>

int main(int argc, char *argv[])
{
    int a = 5, b = 7;
    int result;
    
    printf("Testing calc server...\n");
    
    /* Test add (user-space system call) */
    result = calc_add(a, b);
    printf("calc_add(%d, %d) = %d\n", a, b, result);
    
    /* Test multiply (kernel call) */
    result = calc_multiply(a, b);
    printf("calc_multiply(%d, %d) = %d\n", a, b, result);
    
    return 0;
}
