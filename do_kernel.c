#include <minix/calc.h>

/* Add this function to handle kernel-level multiplication */
int do_calc_multiply(struct proc *caller_ptr, message *m_ptr)
{
    int num1 = m_ptr->m_calc_lsys_multiply.num1;
    int num2 = m_ptr->m_calc_lsys_multiply.num2;
    
    /* Perform the multiplication */
    m_ptr->m_calc_lsys_multiply.result = num1 * num2;
    
    return OK;
}
