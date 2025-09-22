/* 
 * sys_add - User-space library function for the add system call
 */

#include <lib.h>
#include <minix/rs.h>
#include <minix/syslib.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

#include "calc.h"

/* Make the _syscall macro available */
#define _SYSCALL

/* Declare the message structure */
message m;

/* The public library function */
int add(int a, int b)
{
    int r;
    endpoint_t calc_ep;
    
    /* Look up the endpoint of the calc server */
    r = minix_rs_lookup("calc", &calc_ep);
    if (r != OK) {
        errno = ENOENT;
        return -1;
    }
    
    /* Prepare the message */
    m.m_type = CALC_ADD_REQUEST;
    m.m_calc_operand1 = a;
    m.m_calc_operand2 = b;
    m.m_calc_result = 0;
    m.m_calc_status = 0;
    
    /* Send the message to the calc server and wait for reply */
    r = _syscall(calc_ep, CALC_ADD_REQUEST, &m);
    
    if (r != OK) {
        errno = r;
        return -1;
    }
    
    /* Check if the operation was successful */
    if (m.m_calc_status != 0) {
        errno = m.m_calc_status;
        return -1;
    }
    
    /* Return the result */
    return m.m_calc_result;
}
