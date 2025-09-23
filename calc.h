#ifndef _MINIX_CALC_H
#define _MINIX_CALC_H

#include <sys/types.h>

/* Message structure for calc service */
typedef struct {
    int num1;
    int num2;
    int result;
} calc_message_t;

/* Function prototypes */
int calc_add(int a, int b);
int calc_multiply(int a, int b);

#endif /* _MINIX_CALC_H */
