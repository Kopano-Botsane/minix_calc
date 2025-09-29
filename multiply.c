#include <lib.h>
#include <minix/callnr.h>
#include <minix/com.h> /* For CALC_PROC_NR definition */
#include <sys/cdefs.h>
#include <unistd.h>

/*
 * multiply - performs multiplication via the CALC server, which in turn
 * invokes a kernel call to perform the calculation.
 */
__warn_unused_result
int multiply(int a, int b)
{
  message m;
  
  /* Pack input arguments (a, b) into the message structure */
  m.m1_i1 = a;
  m.m1_i2 = b;

  /* * Execute the system call: 
   * Send the message to CALC_PROC_NR with the call number CALC_MULTIPLY. 
   */
  return _syscall(CALC_PROC_NR, CALC_MULTIPLY, &m);
}
