/* System call table for calc server */

#include <sys/cdefs.h>
#include "nsys.h"
#include <minix/com.h>
#include <minix/syslib.h>
#include <minix/callnr.h>

#define CALC(n)		((CALC_BASE)+(n))

/* System call table */
PUBLIC struct {
	int	(*func)(void);
	int	nr;
} call_table[NR_CALC_CALLS] = {
	{ do_add,	CALC(0) },	/* add system call */
	{ do_multiply,	CALC(1) },	/* multiply system call */
};