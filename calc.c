#include <lib.h>
#include <minix/calc.h>
#include <minix/rs.h>
#include <sys/types.h>
#include <errno.h>

#define CALC_ENDPOINT "calc"

/* Helper function to find calc endpoint */
static endpoint_t get_calc_endpoint(void)
{
    endpoint_t ep;
    int r;
    
    if ((r = minix_rs_lookup(CALC_ENDPOINT, &ep)) != OK) {
        errno = ESRCH;
        return -1;
    }
    
    return ep;
}

/* User-space add function */
int calc_add(int a, int b)
{
    message m;
    endpoint_t calc_ep;
    
    calc_ep = get_calc_endpoint();
    if (calc_ep == -1) return -1;
    
    m.m_calc.num1 = a;
    m.m_calc.num2 = b;
    m.m_type = CALC_ADD;
    
    if (_syscall(calc_ep, &m) < 0) {
        return -1;
    }
    
    return m.m_calc.result;
}

/* User-space multiply function */
int calc_multiply(int a, int b)
{
    message m;
    endpoint_t calc_ep;
    
    calc_ep = get_calc_endpoint();
    if (calc_ep == -1) return -1;
    
    m.m_calc.num1 = a;
    m.m_calc.num2 = b;
    m.m_type = CALC_MULTIPLY;
    
    if (_syscall(calc_ep, &m) < 0) {
        return -1;
    }
    
    return m.m_calc.result;
}
