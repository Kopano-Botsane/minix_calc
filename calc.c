/* Main calc server implementation */

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/cdefs.h>
#include <minix/endpoint.h>
#include <minix/syslib.h>
#include <minix/type.h>

#include "nsys.h"

/* Forward declarations */
int do_add(void);
int do_multiply(void);

/* Message buffer */
static message m;

/* Main function */
int main(void)
{
	int r, call_nr;
	
	/* Announce we're up */
	sys_announce(TRUE);
	
	/* Main message loop */
	while (TRUE) {
		/* Get a message */
		if ((r = receive(ANY, &m)) != OK) {
			printf("CALC: receive error: %d\n", r);
			continue;
		}
		
		/* Get call number */
		call_nr = m.m_type - CALC_BASE;
		
		/* Check if valid call */
		if (call_nr < 0 || call_nr >= NR_CALC_CALLS) {
			printf("CALC: invalid call %d\n", call_nr);
			m.m_type = EINVAL;
			send(m.m_source, &m);
			continue;
		}
		
		/* Dispatch the call */
		m.m_type = call_table[call_nr].func();
		
		/* Send reply */
		send(m.m_source, &m);
	}
	
	return OK;
}

/* Add system call implementation */
int do_add(void)
{
	int a = m.m1_i1;
	int b = m.m1_i2;
	
	printf("CALC: add(%d, %d) called\n", a, b);
	
	m.m1_i1 = a + b;
	return OK;
}

/* Multiply system call implementation */
int do_multiply(void)
{
	int a = m.m1_i1;
	int b = m.m1_i2;
	
	printf("CALC: multiply(%d, %d) called\n", a, b);
	
	/* This will be handled by kernel call */
	/* For now, just do local multiplication */
	m.m1_i1 = a * b;
	return OK;
}