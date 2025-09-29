#include <lib.h>
#include <minix/callnr.h>
#include <minix/com.h> /* For CALC_PROC_NR definition */
#include <sys/cdefs.h>
#include <unistd.h>

/*
 * add - performs addition via the CALC server (CALC_PROC_NR).
 * The system call uses CALC_ADD.
 * Arguments are passed in the generic m1_i1 and m1_i2 fields.
 * The result is returned via the reply message's m1_i1 field.
 */
__warn_unused_result
int add(int a, int b)
{
  message m;
  
  /* Pack input arguments (a, b) into the message structure */
  m.m1_i1 = a;
  m.m1_i2 = b;

  /* * Execute the system call: 
   * Send the message to CALC_PROC_NR with the call number CALC_ADD. 
   */
  return _syscall(CALC_PROC_NR, CALC_ADD, &m);
}
