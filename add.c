#include <lib.h>
#include <minix/rs.h>
#include <minix/callnr.h>
#include <sys/types.h>

#define add _add
#include <calc.h>

int add(int a, int b)
{
    message m;
    m.m_lc_calc_add.num1 = a;
    m.m_lc_calc_add.num2 = b;
    
    return _syscall(PM_PROC_NR, PM_ADD, &m);
}
