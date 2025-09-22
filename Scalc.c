/* User space library for calc calls */

#include <lib.h>
#include <minix/rs.h>
#include <sys/cdefs.h>
#include <minix/callnr.h>

#define CALC(n)		((CALC_BASE)+(n))

/* Find calc service endpoint */
static endpoint_t get_calc_endpoint(void)
{
	endpoint_t ep;
	int r;
	
	if ((r = minix_rs_lookup("calc", &ep)) != OK) {
		errno = ENOENT;
		return -1;
	}
	
	return ep;
}

/* Add system call */
int add(int a, int b)
{
	message m;
	endpoint_t ep;
	
	if ((ep = get_calc_endpoint()) == -1)
		return -1;
	
	m.m1_i1 = a;
	m.m1_i2 = b;
	
	return _syscall(ep, CALC(0), &m);
}

/* Multiply system call */
int multiply(int a, int b)
{
	message m;
	endpoint_t ep;
	
	if ((ep = get_calc_endpoint()) == -1)
		return -1;
	
	m.m1_i1 = a;
	m.m1_i2 = b;
	
	return _syscall(ep, CALC(1), &m);
}