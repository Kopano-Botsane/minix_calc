/* 
 * sys_add - User-space library function for the add system call
 */

#include <lib.h>
#include <minix/rs.h>
#include <minix/syslib.h>
#include <errno.h>

/* Define the message structure for our add call */
#define CALC_ADD_REQUEST   1

/* Public library function - user programs will call this */
int sys_add(int a, int b)
{
    message m;
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
    m.m1_i1 = a;  /* First operand */
    m.m1_i2 = b;  /* Second operand */
    
    /* Send the message to the calc server using the kernel call mechanism */
    r = _taskcall(calc_ep, CALC_ADD_REQUEST, &m);
    
    if (r != OK) {
        errno = r;
        return -1;
    }
    
    /* Return the result from the server */
    return m.m1_i1;  /* Result should be in m1_i1 */
}
