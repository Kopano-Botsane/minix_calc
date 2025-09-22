#include <lib.h>
#include <minix/rs.h>
#include <calc.h>

#define CALC_ENDPOINT "calc"

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

int add(int a, int b)
{
    message m;
    endpoint_t calc_ep;
    
    if ((calc_ep = get_calc_endpoint()) == -1) {
        return -1;
    }
    
    m.m_type = CALC_ADD;
    m.m_calc_add.a = a;
    m.m_calc_add.b = b;
    
    if (_syscall(calc_ep, &m) != OK) {
        return -1;
    }
    
    return m.m_calc_add_response.result;
}

int multiply(int a, int b)
{
    message m;
    endpoint_t calc_ep;
    
    if ((calc_ep = get_calc_endpoint()) == -1) {
        return -1;
    }
    
    m.m_type = CALC_MULTIPLY;
    m.m_calc_multiply.a = a;
    m.m_calc_multiply.b = b;
    
    if (_syscall(calc_ep, &m) != OK) {
        return -1;
    }
    
    return m.m_calc_multiply_response.result;
}