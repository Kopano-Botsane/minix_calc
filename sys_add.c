/* 
 * sys_add - User-space library function for the add system call
 */

#include <lib.h>
#include <minix/rs.h>
#include <minix/syslib.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

/* Make the _syscall macro available */
#define _SYSCALL

/* The public library function */
int sys_add(int a, int b)
{
    int r;
    endpoint_t calc_ep;
    message m;
    
    /* Look up the endpoint of the calc server */
    r = minix_rs_lookup("calc", &calc_ep);
    if (r != OK) {
        errno = ENOENT;
        return -1;
    }
    
    /* Prepare the message - we'll use generic fields for now */
    m.m_type = ADD;  /* Use the ADD call number we defined in callnr.h */
    
    /* Store operands in generic message fields */
    m.m1_i1 = a;     /* First operand */
    m.m1_i2 = b;     /* Second operand */
    
    /* Send the message to the calc server and wait for reply */
    r = _taskcall(calc_ep, ADD, &m);
    
    if (r != OK) {
        errno = r;
        return -1;
    }
    
    /* Return the result from the reply message */
    return m.m1_i1;  /* Result should be in m1_i1 */
}
